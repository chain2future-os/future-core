#include "futureio.res.hpp"
//#include <futureio.system/futureio.system.hpp>
#include <futureiolib/system.h>
#include <futureiolib/types.hpp>
#include <futureiolib/transaction.hpp>
namespace futureiores {
    void resource::onblock() {
        require_auth(N(futureio));
        print( ("futureio.res onblock blockheight:" + std::to_string((uint32_t)head_block_number() + 1) + "\n").c_str() );
        set_next_period_res();
        del_expire_table(); //Delete the expired account table
        check_res_expire();
    }
    void resource::recycleresource(const account_name owner) {
        require_auth( _self );
        int64_t ram_bytes = 0;
        get_account_ram_usage( owner, &ram_bytes );
        print("checkresexpire  recycleresource account:",name{owner}," ram_used:",ram_bytes, "\n");
        ram_bytes = 0;
        set_resource_limits( owner, ram_bytes, 0, 0 );
    }

    void resource::check_res_expire() {
        uint32_t block_height = (uint32_t)head_block_number() + 1;
        uint32_t interval_num = 1;
        if( _gstate.is_pending_check ) {
            interval_num = seconds_per_day/block_interval_seconds()/48; //check every half hour
        } else {
            interval_num = seconds_per_day/block_interval_seconds()/3; //check every eight hours
        }
        interval_num = 6;
        if(block_height < 120 || block_height%interval_num != 0) {
            return;
        }
        uint64_t bytes = _gstate.max_ram_size/_gstate.max_resources_number;
        auto add_pending_deltab_func = [this] ( account_name owner ) {
                penddeltable pendingdel(_self,_self);
                auto deltab_itr = pendingdel.find(owner);
                if (deltab_itr == pendingdel.end()) {
                    pendingdel.emplace( [&]( auto& d ){ d.owner = owner; });
                }
            };
        int64_t ram_bytes = 0;
        int64_t net_bytes = 0;
        int64_t cpu_bytes = 0;
        uint32_t  calc_num = 0;
        uint64_t cur_period_id = block_height * block_interval_seconds()/ seconds_per_period + 1;
        _gstate.is_pending_check = false;
        resources_lease_table _reslease_tbl( _self, 0 );
        for(auto leaseiter = _reslease_tbl.begin(); leaseiter != _reslease_tbl.end(); ) {
            const auto& owner = leaseiter->owner;
            get_resource_limits( owner, &ram_bytes, &net_bytes, &cpu_bytes );
            if(leaseiter->end_block_height < block_height) {
                calc_num++;
                if(calc_num > 100) {
                    _gstate.is_pending_check = true;
                    break;
                }
                if(ram_bytes == 0 && net_bytes == 0 && cpu_bytes == 0) {
                    if(_gstate.total_resources_used_number >= leaseiter->lease_num)
                        _gstate.total_resources_used_number -= leaseiter->lease_num;
                    else
                        _gstate.total_resources_used_number = 0;
                    leaseiter = _reslease_tbl.erase(leaseiter);
                } else {
                    set_resource_limits( owner, ram_bytes, 0, 0 ); //Resource expired, no action allowed
                    add_pending_deltab_func( owner );
                    del_expired_res_order(owner, cur_period_id - 1);
                    ++leaseiter;
                }
            } else {
                if ( (uint64_t)ram_bytes > bytes * leaseiter->lease_num ) {
                    add_pending_deltab_func( owner );
                }
                del_expired_res_order(owner, cur_period_id - 1);
                ++leaseiter;
            }
        }
        print("check_res_expire end\n");
    }

    void resource::del_expire_table() {
        penddeltable pendingdeltab(_self,_self);
        if(pendingdeltab.begin() != pendingdeltab.end()) {
            auto del_iter = pendingdeltab.begin();
            auto  owner = del_iter->owner;
            int dropstatus = db_drop_table(owner);   //drop contract account table
            if(dropstatus == 0){
                pendingdeltab.erase(del_iter);
                resources_lease_table _reslease_tbl( _self, 0 );
                auto reslease_itr = _reslease_tbl.find( owner );
                if( reslease_itr != _reslease_tbl.end() && reslease_itr->end_block_height > (uint32_t)head_block_number() ) {
                    uint64_t bytes = _gstate.max_ram_size/_gstate.max_resources_number;
                    set_resource_limits(owner, int64_t(bytes*reslease_itr->lease_num), int64_t(reslease_itr->lease_num), int64_t(reslease_itr->lease_num) );
                    print("del_expire_table: delete table only for ",name{owner}, "\n");
                }
                else {
                    print("del_expire_table: delete table and contract for ",name{owner}, "\n");
                    clear_expire_contract( owner );
                }
            }
        }
    }

    void resource::clear_expire_contract( account_name owner ) {
        vector<permission_level> pem = { { owner, N(active) },
                                        { futureio_account_name,     N(active) } };
        {
            // clear contract
            futureio::transaction trx;
            trx.actions.emplace_back(pem, futureio_account_name, NEX(setcode), std::make_tuple(owner, 0, 0, bytes()) );   //clear contract account code
            trx.actions.emplace_back(pem, futureio_account_name, NEX(setabi), std::make_tuple(owner, bytes()) );   //clear contract account abi
            trx.delay_sec = 0;
            uint128_t trxid = now() + owner + N(clrcontract);
            cancel_deferred(trxid);
            trx.send( trxid, _self, true );
            print("checkresexpire  clear contract account name: ",name{owner}, " trxid:",trxid, "\n");
        }
        {
            //recycle resource
            futureio::transaction recyclerestrans;
            recyclerestrans.actions.emplace_back( permission_level{ _self, N(active) }, _self,
                                                NEX(recycleresource), std::make_tuple(owner) );
            recyclerestrans.delay_sec = 10;
            uint128_t trxid = now() + owner + N(recycleres);
            cancel_deferred(trxid);
            recyclerestrans.send( trxid, _self, true );
            print("checkresexpire  recycle resource account name: ",name{owner}, " trxid:",trxid, "\n");
        }
    }

    void resource::del_expired_res_order(account_name owner, uint64_t period_id) {
        if(0 == period_id) {
            return;
        }
        ressaletable resorders(_self, period_id);
        auto ite_order = resorders.find(owner);
        if(ite_order != resorders.end()) {
            print("delete expired resource order, owner: ", name{owner}, " period: ", period_id, "\n");
            resorders.erase(ite_order);
        }
    }

}/// namespace futureiores

