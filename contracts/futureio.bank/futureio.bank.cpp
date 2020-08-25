#include "futureio.bank.hpp"
#include <futureio.system/futureio.system.hpp>
#include <futureiolib/system.h>
#include <futureiolib/types.hpp>
namespace futureiobank {
    void bank::checktranstobank( account_name from,
                              const asset&  quantity,
                              const name&  to_chain_name,
                              const name& cur_chain_name ) {
        if( cur_chain_name == N(futureio) ){
            futureiosystem::chains_table   _chains(N(futureio),N(futureio));
            auto const iter = std::find_if( _chains.begin(), _chains.end(), [&](auto& c){
                return c.chain_name == to_chain_name;
            });
            futureio_assert( iter != _chains.end(), " The chain to synchronize transfer transactions does not exist");
            futureio_assert( cur_chain_name != iter->chain_name, "Cannot transfer to the same chain");
            recordchainbalance( to_chain_name, quantity );//if master chain,futio.bank record the money transferred in
        }else{
            futureio_assert( N(futureio) == to_chain_name, "Subchain funds transfer must be transferred to the main chain");
            checkchainbalance( to_chain_name, quantity ); //if side chain,futio.bank check the money transferred in
        }

        futureio_assert( quantity >= asset(10), "The amount of funds transferred into the account is too small");

        uint64_t block_height = (uint64_t)head_block_number() + 1;
        bulletin_info  bullinfo(from, quantity);
        bulletinbank bullbank( _self, to_chain_name );
        auto it_bank = bullbank.find(block_height);
        if(it_bank == bullbank.end()){
            bullbank.emplace( [&]( auto& b ){
                b.block_height = block_height;
                b.bulletin_infos.emplace_back(bullinfo);
            });
        }else{
            bullbank.modify(it_bank, [&]( auto& b ) {
                b.bulletin_infos.emplace_back(bullinfo);
            });            
        }
    }

    void bank::checktransfrombank( asset quantity, name chain_name ){
        if( chain_name == N(master) || chain_name == N(futureio) ) {
            //If synchronize from the master chain to the side chain, record the amount of transfer that futio.bank will issue in the side chain
            recordchainbalance( name{N(futureio)}, quantity );
        } else {
            //If the side chain is synchronized to the master chain, check the amount previously transferred to the side chain on the master chain must be enough to transfer the amount in the master chain
            checkchainbalance( chain_name, quantity );
        }
    }
    void bank::transfer( account_name from,
                    account_name to,
                    asset        quantity,
                    string       memo ){
        futureiosystem::global_state_singleton   _global(N(futureio),N(futureio));
        futureio_assert( _global.exists(), "global table not found");
        futureiosystem::futureio_global_state _gstate = _global.get();
        if( from == N(futureio) ){
            futureio_assert( !_gstate.is_master_chain(), " futureio is not allowed to transfer to futio.bank in master chain" );
            if( memo != std::string( futureiosystem::bank_issue_memo ) ) {
                futureio_assert( false, " futureio is not allowed to transfer to futio.bank in side chain" );
            } else  //sidechain futio.bank issue
                return;
        }
        name  chain_name = name{string_to_name(memo.c_str())};
        if( to == N(futio.bank) ){
            checktranstobank( from, quantity, chain_name, _gstate.chain_name );
        }else if( from == N(futio.bank) ){
            checktransfrombank( quantity, chain_name );
        } else{
            futureio_assert( false, " Incorrect transfer action" );
        }

        print(name{from}," transfer to ", name{to}, "  ", quantity," to_chain:",memo);
        uint64_t block_height = (uint64_t)head_block_number() + 1;
        futureiosystem::lwc_singleton   _lwcsingleton(N(futureio),N(futureio));
        futureio_assert( _lwcsingleton.exists(), "lwc_singleton lwc table not found");
        futureiosystem::lwc_parameters _lwc = _lwcsingleton.get();
        if(block_height > _lwc.save_blocks_num){
            uint32_t loopsize = 0;
            bulletinbank bullbank( _self, chain_name );
            for(auto bulliter = bullbank.begin(); bulliter != bullbank.end(); ){
                if(bulliter->block_height < (block_height - _lwc.save_blocks_num))
                    bulliter = bullbank.erase(bulliter);
                else
                    break;
                loopsize++;
                if(loopsize > 1000)
                    break;
            }
        }
    }

    void bank::recordchainbalance( const name& chain_name, const asset& quantity ) {
        chainbalance  chainbalan(_self, _self);
        auto it_chain = chainbalan.find( chain_name );
        if(it_chain == chainbalan.end()){
            chainbalan.emplace( [&]( auto& b ){
                b.chain_name = chain_name;
                b.balance = quantity;
            });
        }else{
            chainbalan.modify(it_chain, [&]( auto& b ) {
                b.balance += quantity;
            });
        }
    }

    void bank::checkchainbalance( const name& chain_name, const asset& quantity ) {
        chainbalance  chainbalan(_self, _self);
        auto it_chain = chainbalan.find( chain_name );
        futureio_assert( it_chain != chainbalan.end(), " chainbalance chain_name no found" );
        futureio_assert( it_chain->balance >= quantity, " Insufficient funds transferred in" );
        chainbalan.modify(it_chain, [&]( auto& b ) {
            b.balance -= quantity;
        });
    }

}/// namespace futureiobank
