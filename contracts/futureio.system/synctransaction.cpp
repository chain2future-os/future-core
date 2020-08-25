/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#include "futureio.system.hpp"

#include <futureiolib/futureio.hpp>
#include <futureio.token/futureio.token.hpp>
#include <futureiolib/merkle_proof.hpp>
#include <futureiolib/transaction.hpp>

namespace futureiosystem {
    using namespace futureio;
    using futureio::asset;
    using futureio::bytes;
    using futureio::print;
    /*
    struct chain_balance {
            name     chain_name;
            asset    balance;
            uint64_t primary_key()const { return chain_name; }

            FUTURELIB_SERIALIZE(chain_balance, (chain_name)(balance))
    };
    typedef futureio::multi_index<N(chainbalance), chain_balance> chainbalance;
    */
    struct TransferActionParam {
        public:
            account_name from;
            account_name to;
            asset        val;
            std::string  memo;

            FUTURELIB_SERIALIZE(TransferActionParam, (from)(to)(val)(memo))
    };

    struct EmpowerUserParam {
        account_name    user;
        name            chain_name;
        std::string     owner_pk;
        std::string     active_pk;
        bool            updateable;
        FUTURELIB_SERIALIZE(EmpowerUserParam, (user)(chain_name)(owner_pk)(active_pk)(updateable))
    };

    struct ResleaseActionParam {
        public:
            account_name from;
            account_name receiver;
            uint16_t combosize;
            uint64_t period;
            name location;
            FUTURELIB_SERIALIZE(ResleaseActionParam, (from)(receiver)(combosize)(period)(location))
    };

    struct CommitteeChangeParam {
        account_name   producer;
        std::string    producerkey;
        std::string    blskey;
        bool           from_disable;
        name           from_chain;
        bool           to_disable;
        name           to_chain;

        FUTURELIB_SERIALIZE(CommitteeChangeParam, (producer)(producerkey)(blskey)(from_disable)
                             (from_chain)(to_disable)(to_chain))
    };

    struct UpdateAuthParam {
        account_name account;
        permission_name permission;
        permission_name parent;
        authority auth;

        FUTURELIB_SERIALIZE(UpdateAuthParam, (account)(permission)(parent)(auth))
    };

    struct DeleteAuthParam {
        account_name account;
        permission_name permission;

        FUTURELIB_SERIALIZE(DeleteAuthParam, (account)(permission))
    };

   static std::string checksum256_to_string( const uint8_t* d, uint32_t s )
   {
      std::string r;
      const char* to_hex="0123456789abcdef";
      uint8_t* c = (uint8_t*)d;
      for( uint32_t i = 0; i < s; ++i )
            (r += to_hex[(c[i]>>4)]) += to_hex[(c[i] &0x0f)];
      return r;
   }
   void system_contract::synclwctx( name chain_name, uint32_t block_number, std::vector<std::string> merkle_proofs, std::vector<char> tx_bytes ) {
      require_auth(current_sender());
      block_table subchain_block_tbl(_self, chain_name);
      auto block_ite = subchain_block_tbl.find(block_number);
      futureio_assert(block_ite != subchain_block_tbl.end(), "can not find this subchain block_number");
      merkle_proof mklp;
      mklp.proofs = merkle_proofs;
      mklp.tx_bytes = tx_bytes;
      std::string merkle_mroot = checksum256_to_string( block_ite->transaction_mroot.hash, sizeof(block_ite->transaction_mroot.hash));
      bool r = mklp.verify(merkle_mroot);
      futureio_assert(r, "syncTransfer failed: verify merkle proof failed.");

      stateful_transaction tx = mklp.recover_transaction();
      futureio_assert(tx.status == stateful_transaction::executed, "this transaction is not executed.");
      futureio_assert(!tx.tx_id.empty(), "this transaction tx_id is empty.");
      futureio_assert(block_ite->trx_ids.count(tx.tx_id) != 1, "syncTransfer failed: current transfer transaction already synchronized");
      subchain_block_tbl.modify( block_ite, [&]( block_header_digest& header ) {
         header.trx_ids.insert(tx.tx_id);
      });
      futureio_assert(tx.actions.size() > 0, "no context related actions contains in this transaction.");
      exec_actions( tx.actions, chain_name );
   }

   void system_contract::exec_actions( const vector<action> & actios, name chain_name){
      uint32_t  exec_succ = 0;
      for (const auto& act : actios) {
         if (act.account == N(futio.token) && act.name == NEX(transfer)) {
            TransferActionParam tap = unpack<TransferActionParam>(act.data);
            if (tap.val.symbol != symbol_type(CORE_SYMBOL))
                continue;
            asset cur_tokens = futureio::token(N(futio.token)).get_balance( N(futio.bank),symbol_type(CORE_SYMBOL).name());
            if(tap.to != N(futio.bank) ||
              string_to_name(tap.memo.c_str()) != _gstate.chain_name
              )
               continue;

            if( cur_tokens < tap.val && !_gstate.is_master_chain()){ //if master chain no issue tokens
               INLINE_ACTION_SENDER(futureio::token, issue)( N(futio.token), {{N(futureio),N(active)}},
               {N(futio.bank),(tap.val - cur_tokens), std::string( bank_issue_memo )} );
            } else if(_gstate.is_master_chain()){
               chainbalance  chainbalan(N(futio.bank), N(futio.bank));
               auto it_chain = chainbalan.find( chain_name );
               if(cur_tokens < tap.val || it_chain == chainbalan.end() || it_chain->balance < tap.val){
                  print(" ERROR The futio.bank of the masterchain should never be smaller than the amount of money to be transferred\n");
                  continue;
               }
            }

            exec_succ++;
            INLINE_ACTION_SENDER(futureio::token, transfer)( N(futio.token), {N(futio.bank), N(active)},
               { N(futio.bank), tap.from, tap.val, name{chain_name}.to_string() } );
            print("synctransfer  from : ", name{tap.from}, ", to: ", name{tap.to});
            print(", asset: "); tap.val.print();
            print(", memo: ", tap.memo, "\n");
         } else if(act.account == N(futureio) && act.name == NEX(empoweruser)) {
             EmpowerUserParam eup = unpack<EmpowerUserParam>(act.data);
             if(eup.chain_name != _gstate.chain_name)
                continue;
             exec_succ++;
             proposeaccount_info new_acc;
             new_acc.account = eup.user;
             new_acc.owner_key = eup.owner_pk;
             new_acc.active_key = eup.active_pk;
             new_acc.updateable = eup.updateable;
             new_acc.location = name{N(futureio)};
             add_subchain_account(new_acc);
             print("sync user ", name{eup.user}, "\n");
         }else if (act.account == N(futio.res) && act.name == NEX(resourcelease)) {
            ResleaseActionParam rap = unpack<ResleaseActionParam>(act.data);
            if(rap.location != _gstate.chain_name)
                continue;
            exec_succ++;
            INLINE_ACTION_SENDER(futureiores::resource, resourcelease)( N(futio.res), {N(futureio), N(active)},
                  { N(futureio), rap.receiver, rap.combosize, rap.period, self_chain_name} );
            print("sync resource of ", name{rap.receiver}, " leasenum: ", uint32_t(rap.combosize), " period: ",rap.period, "\n");
         } else if (act.account == N(futureio) && act.name == NEX(moveprod)) {
            CommitteeChangeParam ccp = unpack<CommitteeChangeParam>(act.data);
            if(ccp.from_chain == _gstate.chain_name) {
                //move producer out
                print("synclwctx, disable producer:",name{ccp.producer}, "\n");
                exec_succ++;
                INLINE_ACTION_SENDER(futureiosystem::system_contract, undelegatecons)( N(futureio), {N(futio.stake), N(active)},
                      { N(futio.stake), ccp.producer} );
            } else if(ccp.to_chain == _gstate.chain_name) {
                //add new producer
                print("synclwctx, add new producer:",name{ccp.producer}, "\n");
                exec_succ++;
                vector<permission_level>   authorization;
                authorization.emplace_back(permission_level{ N(futureio), N(active)});
                authorization.emplace_back(permission_level{ccp.producer, N(active)});
                INLINE_ACTION_SENDER(futureiosystem::system_contract, regproducer)( N(futureio), authorization,
                    { ccp.producer, ccp.producerkey, ccp.blskey, N(futureio), " ", self_chain_name, 0 } );
                INLINE_ACTION_SENDER(futureiosystem::system_contract, delegatecons)( N(futureio), {N(futio.stake), N(active)},
                    { N(futio.stake), ccp.producer, asset(_gstate.min_activated_stake)} );
            }
         } else if(act.account == N(futureio) && act.name == NEX(updateauth)) {
             //update auth
             UpdateAuthParam uap = unpack<UpdateAuthParam>(act.data);
             print("synclwctx, update auth of ", name{uap.account}, ", permission: ", name{uap.permission}, ", parent: ", name{uap.parent}, "\n");
             vector<permission_level>   action_auth = act.authorization;
             action_auth.emplace_back(N(futureio), N(active));
             INLINE_ACTION_SENDER(futureiosystem::native, updateauth)( N(futureio), action_auth,
                    {uap.account, uap.permission, uap.parent, uap.auth});
             exec_succ++;
         } else if(act.account == N(futureio) && act.name == NEX(deleteauth)) {
             //delete auth
             DeleteAuthParam dap = unpack<DeleteAuthParam>(act.data);
             print("synclwctx, delete auth of ", name{dap.account}, ", permission: ", name{dap.permission}, "\n");
             vector<permission_level>   action_auth = act.authorization;
             action_auth.emplace_back(N(futureio), N(active));
             INLINE_ACTION_SENDER(futureiosystem::native, deleteauth)( N(futureio), action_auth,
                    {dap.account, dap.permission});
             exec_succ++;
         }
      }
      futureio_assert(exec_succ > 0, "The current transaction has no execution");
   }
} //namespace futureiosystem
