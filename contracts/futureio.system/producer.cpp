/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#include "futureio.system.hpp"

#include <futureiolib/crypto.h>
#include <futureiolib/datastream.hpp>
#include <futureiolib/serialize.hpp>
#include <futureiolib/transaction.hpp>
#include <futureio.token/futureio.token.hpp>

#include <algorithm>
#include <cmath>

namespace futureiosystem {
    using namespace futureio;

    void system_contract::regproducer(const account_name producer,
                                      const std::string& producer_key,
                                      const std::string& bls_key,
                                      account_name rewards_account,
                                      const std::string& url,
                                      name location,
                                      uint64_t chain_type) {
        bool has_futureio_auth = true;
        if(!has_auth(_self)) {
            bool is_allow_self_register = false;
            for(auto extension : _gstate.table_extension){
                if(extension.key == futureio_global_state::global_state_exten_type_key::is_allow_producer_self_register
                    && extension.value == "1") {
                   is_allow_self_register = true;
                   break;
                }
            }
            futureio_assert(is_allow_self_register, "only allow futureio account to register producer");
            require_auth(producer);

            futureio_assert(location == default_chain_name, "no permission to assign producer to a specific location, please set location as default");
            has_futureio_auth = false;
        }
        futureio_assert( url.size() < 512, "url too long" );
        // key is hex encoded, ed25519 64 and sm2 53
        futureio_assert( producer_key.size() == 64 || producer_key.size() == 53, "public key should be of size 64 or 53" );
        futureio_assert( bls_key.size() == 130, "public bls key should be of size 130" );
        futureio_assert( is_account( producer ), "producer account not exists" );
        futureio_assert( is_account( rewards_account ), "rewards account not exists" );
        prodexten_table prodext(_self, _self);
        auto ite_ext = prodext.find(producer);
        if(location == default_chain_name) {
            chaintypes_table type_tbl(_self, _self);
            auto type_ite = type_tbl.find(chain_type);
            futureio_assert(type_ite != type_tbl.end(), "chain type id is not found");
            if(ite_ext == prodext.end()) {
                prodext.emplace([&]( producer_ext& prod_ext ) {
                    prod_ext.owner = producer;
                    prod_ext.chain_type = chain_type;
                });
            } else if(ite_ext->chain_type != chain_type) {
                prodext.modify(ite_ext, [&]( producer_ext& prod_ext ) {
                    prod_ext.chain_type = chain_type;
                });
            }
        } else if(ite_ext != prodext.end()) {
           //producer ext only has 1 member: chain_type, erase it directly.
           //If some other members are added to this table someday, please use modify() to reset chain type to 0;
           prodext.erase(ite_ext);
        }
        check_producer_evillist( producer );

        if(location != self_chain_name && location != default_chain_name) {
            futureio_assert(location != N(master), "wrong location name");
            futureio_assert(_chains.find(location) != _chains.end(),
                                 "wrong location, chain is not existed");
        }
        uint64_t curblocknum = (uint64_t)head_block_number() + 1;
        auto briefprod = _briefproducers.find(producer);
        if(briefprod == _briefproducers.end()) {
            //new producer, add to disabled table for now
            disabled_producers_table dp_tbl(_self, _self);
            dp_tbl.emplace( [&]( disabled_producer& dis_prod ) {
                dis_prod.owner                   = producer;
                dis_prod.producer_key            = producer_key;
                dis_prod.bls_key                 = bls_key;
                dis_prod.url                     = url;
                dis_prod.last_operate_blocknum   = curblocknum;
                dis_prod.delegated_cons_blocknum = 0;
                dis_prod.claim_rewards_account   = rewards_account;
            });
            _briefproducers.emplace([&]( producer_brief& brief_prod ) {
                brief_prod.owner        = producer;
                brief_prod.location     = location;
                brief_prod.in_disable   = true;
            });
        } else {
            futureio_assert( briefprod->in_disable, "producer has been enabled" );
            if(briefprod->location != location) {
                _briefproducers.modify(briefprod, [&]( producer_brief& brief_prod ) {
                    brief_prod.location     = location;
                });
            }
            disabled_producers_table dp_tbl(_self, _self);
            auto it_disable = dp_tbl.find(producer);
            futureio_assert(it_disable != dp_tbl.end(), "error: producer is not in disabled table");
            futureio_assert(it_disable->last_operate_blocknum < curblocknum, "only one action can be performed in a block");
            dp_tbl.modify(it_disable, [&]( disabled_producer& dis_prod ) {
               dis_prod.producer_key            = producer_key;
               dis_prod.bls_key                 = bls_key;
               dis_prod.url                     = url;
               dis_prod.last_operate_blocknum   = curblocknum;
               dis_prod.claim_rewards_account   = rewards_account;
            });
        }

        if ( !has_futureio_auth ) {
            futureio_assert( _gstate.is_master_chain(), "only master chain allow regproducer" );
            asset cur_tokens =
                futureio::token(N(futio.token)).get_balance( producer,symbol_type(CORE_SYMBOL).name());
            futureio_assert( cur_tokens.amount >= 50000,
                              "The current action fee is 5 FGAS, please ensure that the account is fully funded" );
            INLINE_ACTION_SENDER(futureio::token, transfer)( N(futio.token), {producer,N(active)},
                { producer, N(futio.fee), asset(50000), std::string("regproducer fee") } );
            for(auto ite_chain = _chains.begin(); ite_chain != _chains.end(); ++ite_chain) {
                if(ite_chain->chain_name == N(master))
                    continue;
                std::string errorlog = std::string("producer account must be synchronized to all the subchains, \
so that we can schedule and secure the subchains. Please perform empoweruser action \
producer:") + name{producer}.to_string() + std::string(" not synchronized to subchain:") + name{ite_chain->chain_name}.to_string();
                futureio_assert(is_empowered(producer, ite_chain->chain_name), errorlog.c_str());
            }
        }
    }

    std::vector<name> system_contract::get_all_chainname() {
        std::vector<name> scopes;
        for(auto ite_chain = _chains.begin(); ite_chain != _chains.end(); ++ite_chain) {
            if(ite_chain->chain_name == N(master))
                continue;
            scopes.emplace_back(ite_chain->chain_name);
        }
        return scopes;
    }

    void system_contract::moveprod(account_name producer, const std::string&  producerkey, const std::string& blskey,
                                   bool from_disable, name from_chain, bool to_disable, name to_chain) {
        require_auth(_self);
        if(from_disable && to_disable) {
            futureio_assert(false, "error: cannot move a producer from disable to disable");
        } else if (!from_disable && !to_disable) {
            futureio_assert(_gstate.is_master_chain(), "move producer between chains can only be performed on master chain");
        }

        auto briefprod = _briefproducers.find(producer);
        futureio_assert(briefprod != _briefproducers.end(), "not a producer");

        auto current_block_number = uint64_t(head_block_number() + 1);
        producer_info prod_info;
        if(from_disable) {
            disabled_producers_table dp_tbl(_self, _self);
            auto it_disable = dp_tbl.find(producer);
            futureio_assert(it_disable != dp_tbl.end(), "error: producer is not in disabled table");
            prod_info = producer_info(*it_disable, remove_rewards_for_enableproducer( producer ), 0, 0);
            dp_tbl.erase(it_disable);

            print("move producer ", name{producer}, " from disable\n");
        } else {
            producers_table _producers(_self, from_chain);
            auto producer_iter = _producers.find(producer);
            futureio_assert(producer_iter != _producers.end(), "error: producer to move out is not found in its location");
            prod_info = *producer_iter;
            remove_from_chain(from_chain, producer, current_block_number);
            print("move producer ", name{producer}, " from ", from_chain, "\n");
            if(default_chain_name != from_chain) {
                cmtbulletin  cb_tbl(_self, from_chain);
                auto record = cb_tbl.find(current_block_number);
                if(record == cb_tbl.end()) {
                    cb_tbl.emplace([&](auto& new_change) {
                        new_change.block_num = current_block_number;
                        new_change.change_type = remove_producer;
                    });
                } else if ((record->change_type & remove_producer) == 0) {
                    cb_tbl.modify(record, [&](auto& _change) {
                        _change.change_type |= remove_producer;
                    });
                }
            }
        }

        if(to_disable) {
            disabled_producers_table dp_tbl(_self, _self);
            dp_tbl.emplace( [&]( disabled_producer& dis_prod ) {
                dis_prod = prod_info;
                dis_prod.delegated_cons_blocknum = current_block_number;
            });
            add_rewards_for_disproducer( prod_info.owner, prod_info.claim_rewards_account, prod_info.unpaid_balance );
            _briefproducers.modify(briefprod, [&](producer_brief& producer_brf) {
                producer_brf.in_disable = true;
            });
            print("move producer ", name{producer}, "to disable\n");
        } else {
            futureio_assert( producerkey == prod_info.producer_key, "error: public producer key not consistent with the original chain" );
            futureio_assert( blskey == prod_info.bls_key, "error: public bls key not consistent with the original chain" );
            remove_producer_from_evillist( producer );  //Will be moved to enabled producer, should be removed from the evil list
            bool enable_by_heartbeat = from_disable && from_chain == disable_heartbeat;
            add_to_chain(to_chain, prod_info, current_block_number, enable_by_heartbeat);
            _briefproducers.modify(briefprod, [&](producer_brief& producer_brf) {
                producer_brf.in_disable = false;
                producer_brf.location = to_chain;
            });
            print("move producer ", name{producer}, " to ", to_chain, "\n");
            if(default_chain_name != to_chain) {
                cmtbulletin  cb_tbl(_self, to_chain);
                auto record = cb_tbl.find(current_block_number);
                if(record == cb_tbl.end()) {
                    cb_tbl.emplace([&](auto& new_change) {
                        new_change.block_num = current_block_number;
                        new_change.change_type = add_producer;
                    });
                } else if ((record->change_type & add_producer) == 0) {
                    cb_tbl.modify(record, [&](auto& _change) {
                        _change.change_type |= add_producer;
                    });
                }
            }
        }
    }

   /*
   verifyprodevil function manually parses input data (instead of taking parsed arguments from dispatcher)
   because parsing data in the dispatcher uses too much CPU in case if evil_info is big

   If we use dispatcher the function signature should be:

   void system_contract::verifyprodevil( account_name producer, const std::string& evil_info )
   */
   void system_contract::verifyprodevil() {
      //require_auth( _self );
      constexpr size_t max_stack_buffer_size = 512;
      size_t size = action_data_size();
      char* buffer = (char*)( max_stack_buffer_size < size ? malloc(size) : alloca(size) );
      read_action_data( buffer, size );
      account_name evil;
      std::string evidence;
      datastream<const char*> ds( buffer, size );
      ds >> evil >> evidence;
      auto briefprod = _briefproducers.find( evil );
      futureio_assert( briefprod != _briefproducers.end(), "not a producer" );
      futureio_assert( !briefprod->in_disable, "producer in disabled" );
      producers_table _producers(_self, briefprod->location);
      const auto& it = _producers.find( evil );
      futureio_assert(it != _producers.end(), "producer is not found in its location");
      producer_info prod = *it;
      evildoer e = {evil, prod.producer_key, prod.bls_key};
      int type = verify_evil(evidence, e);
      futureio_assert(type != producer_evil_type::not_evil, "can not verify whether is evil");
      uint128_t sendid = N(evilprod) + evil;
      cancel_deferred(sendid);
      futureio::transaction out;
      out.actions.emplace_back( permission_level{ _self, N(active) }
                              , _self
                              , NEX(procevilprod)
                              , std::make_tuple(evil, producer_evil_type::sign_multi_propose)
                              );
      out.delay_sec = 0;
      out.send( sendid, _self, true );
   }

   void system_contract::procevilprod( account_name producer, uint16_t evil_type ) {
      require_auth( _self );
      auto briefprod = _briefproducers.find( producer );
      //futureio_assert( briefprod != _briefproducers.end(), "not a producer" );
      //futureio_assert( !briefprod->in_disable, "producer in disabled" );
      producers_table _producers(_self, briefprod->location);
      const auto& it = _producers.find( producer );
      futureio_assert(it != _producers.end(), "producer is not found in its location");
      if( _gstate.is_master_chain() ) {
         evilprodtab evilprod( _self, _self );
         auto evilprod_itr = evilprod.find( producer );
         if( evilprod_itr == evilprod.end() ) {
            evilprod.emplace([&](auto& e) {
               e.owner = producer;
               e.evil_type = evil_type;
               if( producer_evil_type::sign_multi_propose == evil_type ){  //Freeze delegate tokens
                  e.is_freeze = true;
               }
            });
         } else {
            evilprod.modify(evilprod_itr, [&]( auto& e ) {
               e.evil_type = evil_type;
               if( producer_evil_type::sign_multi_propose == evil_type ){  //Freeze delegate tokens
                  e.is_freeze = true;
               }
            });
         }

         //remove committee
         moveprod_param mv_prod(it->owner, it->producer_key, it->bls_key, false, briefprod->location, true, name{N(disable)});
         send_defer_moveprod_action( mv_prod );
      } else {  //sidechain record bulletin
         futureio_assert( producer_evil_type::sign_multi_propose == evil_type
                           , " Non specified evil msg cannot emplace sidechain bulletin board" );
         bulletintab  bltn( _self, N(master) );
         auto current_block_number = uint64_t(head_block_number() + 1);
         auto bltn_itr = bltn.find(current_block_number);
         futureio_assert( bltn_itr == bltn.end(), " evil bulletin current blockheight already exist" );
         bltn.emplace([&](auto& b) {
            b.block_num = current_block_number;
            b.action_name.insert("procevilprod");
         });
      }
   }

   void system_contract::send_defer_moveprod_action( const moveprod_param& prodparam ) const {
      uint128_t sendid = N(moveprod) + prodparam.producer;
      cancel_deferred(sendid);
      futureio::transaction out;
      out.actions.emplace_back( permission_level{ _self, N(active) }, _self, NEX(moveprod), prodparam );
      out.delay_sec = 0;
      out.send( sendid, _self, true );
   }

   void system_contract::check_producer_evillist( const account_name& producer ) const {
      evilprodtab evilprod( _self, _self );
      auto evilprod_itr = evilprod.find( producer );
      if( evilprod_itr == evilprod.end() ) {
         return;
      }
      //freeze not allowed to be producer again
      futureio_assert( !evilprod_itr->is_freeze, " evil producer not allowed to be producer again" );
   }

   void system_contract::remove_producer_from_evillist( const account_name& producer ) const {
      evilprodtab evilprod( _self, _self );
      auto evilprod_itr = evilprod.find( producer );
      if( evilprod_itr != evilprod.end() ) {
         evilprod.erase( evilprod_itr );
      }
   }

   void system_contract::check_producer_lastblock( const name& chain_name, uint64_t block_height ) const {
      if( !_gstate.is_master_chain() ) {
         return;
      }
      uint64_t produce_critical_number = 3 * seconds_per_day / block_interval_seconds();
      uint32_t interval_num = seconds_per_day/block_interval_seconds() + 3;   // check_producer_lastblock once about an day
      if(block_height < produce_critical_number || block_height%interval_num != 0) {
         return;
      }
      auto last_allow_produce_block = block_height - produce_critical_number;
      producers_table _producers(_self, chain_name);
      for(auto itr = _producers.begin(); itr != _producers.end(); ++itr){
         if( itr->last_record_blockheight > last_allow_produce_block ){
            continue;
         }
         uint128_t sendid = N(procevilprod) + itr->owner;
         cancel_deferred(sendid);
         futureio::transaction out;
         out.actions.emplace_back( permission_level{ _self, N(active) }
                                 , _self
                                 , NEX(procevilprod)
                                 , std::make_tuple(itr->owner, producer_evil_type::limit_time_not_produce)
                                 );
         out.delay_sec = 1;
         out.send( sendid, _self, true );
      }
   }

   void system_contract::prodheartbeat(account_name producer) {
       require_auth(producer);
       fixed_committee_table fixed_cmt( _self, _self );
       futureio_assert( fixed_cmt.find( producer ) == fixed_cmt.end(), "producer is on fixed committee,should not heartbeat");
       auto briefprod = _briefproducers.find(producer);
       futureio_assert(briefprod != _briefproducers.end(), "not a producer");
       futureio_assert(_gstate.is_master_chain(), "only handle producer heartbeat in master chain");
       if(briefprod->in_disable) {
           check_producer_evillist(producer);
           disabled_producers_table dp_tbl(_self, _self);
           auto it_disable = dp_tbl.find(producer);
           futureio_assert(it_disable != dp_tbl.end(), "error: producer is not in disabled table");
           futureio_assert(it_disable->total_cons_staked >= get_producer_min_stake(producer, briefprod->location), "producer has been undelegated");

           auto ite_chain = _chains.find(briefprod->location);
           futureio_assert(ite_chain != _chains.end(), "destination chain is not found");
           futureio_assert(ite_chain->is_schedulable, "a committee change is ongoing in destination chain");
           moveprod_param mv_prod(producer, it_disable->producer_key, it_disable->bls_key, true, disable_heartbeat, false, briefprod->location);
           send_defer_moveprod_action(mv_prod);
           return;
       }
       producers_table _producers(_self, briefprod->location);
       auto prod = _producers.find(producer);
       futureio_assert(prod != _producers.end(), "producer is not found in its location");
       uint64_t cur_block_height = (uint64_t)head_block_number() + 1;
       futureio_assert(cur_block_height != prod->get_block_height_by_key(producer_info::last_heartbeat_block_height), "repeated heartbeat transaction");
       _producers.modify(prod, [&](auto& _producer) {
           _producer.set_block_height_for_key(producer_info::last_heartbeat_block_height, cur_block_height);
       });
   }
    /*
    * Add or remove fixed committee actions
    */
    void system_contract::opefixedprod( uint16_t ope_type, const std::vector<account_name>& prod_vec ) {
        require_auth(_self);
        fixed_committee_table fixed_cmt( _self, _self );
        futureio_assert( !prod_vec.empty(), "prod_vec is empty");
        futureio_assert( ope_type < committee_ope_type::fixed_committee_end, "committee_ope_type not found");
        for( auto const& prod : prod_vec ) {
            auto const prod_itr = fixed_cmt.find( prod );
            if( ope_type == committee_ope_type::fixed_committee_add && prod_itr == fixed_cmt.end() ){
                fixed_cmt.emplace([&]( auto& f ) {
                    f.producer = prod;
                });
            }
            if( ope_type == committee_ope_type::fixed_committee_remove && prod_itr != fixed_cmt.end() ){
                fixed_cmt.erase( prod_itr );
            }
        }
    }

   int64_t system_contract::get_producer_min_stake(const account_name& producer, name chain_name) {
       if(chain_name == self_chain_name) {
           return _gstate.min_activated_stake;
       }
       uint64_t chain_type_id = 0;
       if(chain_name != default_chain_name) {
           auto ite_chain = _chains.find(chain_name);
           futureio_assert(ite_chain != _chains.end(), "get_producer_min_stake: wrong chain name");
           chain_type_id = ite_chain->chain_type;
       } else {
           prodexten_table prodext(_self, _self);
           auto ite_ext = prodext.find(producer);
           if(ite_ext != prodext.end()) {
               if(ite_ext->chain_type > 0) {
                   chain_type_id = ite_ext->chain_type;
               }
           }
       }
       if(chain_type_id > 0 ) {
           chaintypes_table type_tbl(_self, _self);
           auto ite_type = type_tbl.find(chain_type_id);
           futureio_assert(ite_type != type_tbl.end(), "get_producer_min_stake: chain type is not found");
           auto min_stake = ite_type->get_min_activated_stake();
           if(min_stake > 0) {
               return min_stake;
           }
       }
       return _gstate.min_activated_stake;
   }
} /// namespace futureiosystem
