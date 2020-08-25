#pragma once
#include <futureiolib/futureio.hpp>
#include <futureiolib/asset.hpp>
#include <futureiolib/multi_index.hpp>

namespace futureio {
   using std::string;
   using std::array;

   /**
    *  This contract enables the creation, issuance, and transfering of many different tokens.
    */
   class currency {
      public:
         currency( account_name contract )
         :_contract(contract)
         { }

         struct create {
            account_name           issuer;
            asset                  maximum_supply;
           // array<char,32>         issuer_agreement_hash;
            uint8_t                issuer_can_freeze     = true;
            uint8_t                issuer_can_recall     = true;
            uint8_t                issuer_can_whitelist  = true;

            /*(issuer_agreement_hash)*/
            FUTURELIB_SERIALIZE( create, (issuer)(maximum_supply)(issuer_can_freeze)(issuer_can_recall)(issuer_can_whitelist) )
         };

         struct transfer
         {
            account_name from;
            account_name to;
            asset        quantity;
            string       memo;

            FUTURELIB_SERIALIZE( transfer, (from)(to)(quantity)(memo) )
         };

         struct issue {
            account_name to;
            asset        quantity;
            string       memo;

            FUTURELIB_SERIALIZE( issue, (to)(quantity)(memo) )
         };

         struct fee_schedule {
            uint64_t primary_key()const { return 0; }

            array<extended_asset,7> fee_per_length;
            FUTURELIB_SERIALIZE( fee_schedule, (fee_per_length) )
         };

         struct account {
            asset    balance;
            bool     frozen    = false;
            bool     whitelist = true;

            uint64_t primary_key()const { return balance.symbol.name(); }

            FUTURELIB_SERIALIZE( account, (balance)(frozen)(whitelist) )
         };

         struct currency_stats {
            asset          supply;
            asset          max_supply;
            account_name   issuer;
            bool           can_freeze         = true;
            bool           can_recall         = true;
            bool           can_whitelist      = true;
            bool           is_frozen          = false;
            bool           enforce_whitelist  = false;

            uint64_t primary_key()const { return supply.symbol.name(); }

            FUTURELIB_SERIALIZE( currency_stats, (supply)(max_supply)(issuer)(can_freeze)(can_recall)(can_whitelist)(is_frozen)(enforce_whitelist) )
         };

         typedef futureio::multi_index<N(accounts), account> accounts;
         typedef futureio::multi_index<N(stat), currency_stats> stats;


         asset get_balance( account_name owner, symbol_name symbol )const {
            accounts t( _contract, owner );
            return t.get(symbol).balance;
         }

         asset get_supply( symbol_name symbol )const {
            accounts t( _contract, symbol );
            return t.get(symbol).balance;
         }

         static void inline_transfer( account_name from, account_name to, extended_asset amount, string memo = string(), permission_name perm = N(active) ) {
            action act( permission_level( from, perm ), amount.contract, NEX(transfer), transfer{from,to,amount,memo} );
            act.send();
         }

         void inline_transfer( account_name from, account_name to, asset amount, string memo = string(), permission_name perm = N(active) ) {
            action act( permission_level( from, perm ), _contract, NEX(transfer), transfer{from,to,amount,memo} );
            act.send();
         }


         bool apply( account_name contract, action_name act ) {
            if( contract != _contract )
               return false;

            bool handled = true;
            if (act == NEX(issue)) {
                print( "issue\n");
                on( unpack_action_data<issue>() );
            } else if (act == NEX(transfer)) {
                print( "transfer\n");
                on( unpack_action_data<transfer>() );
            } else if (act == NEX(create)) {
                print( "create\n");
                on( unpack_action_data<create>() );
            } else {
                handled = false;
            }

            return handled;
         }

          /**
           * This is factored out so it can be used as a building block
           */
          void create_currency( const create& c ) {
            auto sym = c.maximum_supply.symbol;
            futureio_assert( sym.is_valid(), "invalid symbol name" );

             stats statstable( _contract, sym.name() );
             auto existing = statstable.find( sym.name() );
             futureio_assert( existing == statstable.end(), "token with symbol already exists" );

             statstable.emplace( [&]( auto& s ) {
                s.supply.symbol = c.maximum_supply.symbol;
                s.max_supply    = c.maximum_supply;
                s.issuer        = c.issuer;
                s.can_freeze    = c.issuer_can_freeze;
                s.can_recall    = c.issuer_can_recall;
                s.can_whitelist = c.issuer_can_whitelist;
             });
          }

          void issue_currency( const issue& i ) {
             auto sym = i.quantity.symbol.name();
             stats statstable( _contract, sym );
             const auto& st = statstable.get( sym );

             statstable.modify( st, [&]( auto& s ) {
                s.supply.amount += i.quantity.amount;
                futureio_assert( s.supply.amount >= 0, "underflow" );
             });

             add_balance( st.issuer, i.quantity, st );
          }


          void on( const create& c ) {
             require_auth( c.issuer );
             create_currency( c );

             /*
             auto fee = get_fee_schedule()[c.maximum_supply.name_length()];
             if( fee.amount > 0 ) {
                inline_transfer( c.issuer, _contract, fee, "symbol registration fee" );
             }
             */
          }

          void on( const issue& i ) {
             auto sym = i.quantity.symbol.name();
             stats statstable( _contract, sym );
             const auto& st = statstable.get( sym );

             require_auth( st.issuer );
             futureio_assert( i.quantity.is_valid(), "invalid quantity" );
             futureio_assert( i.quantity.amount > 0, "must issue positive quantity" );

             statstable.modify( st, [&]( auto& s ) {
                s.supply.amount += i.quantity.amount;
             });

             add_balance( st.issuer, i.quantity, st );

             if( i.to != st.issuer )
             {
                inline_transfer( st.issuer, i.to, i.quantity, i.memo );
             }
          }

          void on( const transfer& t ) {
             require_auth(t.from);
             auto sym = t.quantity.symbol.name();
             stats statstable( _contract, sym );
             const auto& st = statstable.get( sym );

             require_recipient( t.to );

             futureio_assert( t.quantity.is_valid(), "invalid quantity" );
             futureio_assert( t.quantity.amount > 0, "must transfer positive quantity" );
             sub_balance( t.from, t.quantity, st );
             add_balance( t.to, t.quantity, st );
          }


      private:
          void sub_balance( account_name owner, asset value, const currency_stats& st ) {
             accounts from_acnts( _contract, owner );

             const auto& from = from_acnts.get( value.symbol.name() );
             futureio_assert( from.balance.amount >= value.amount, "overdrawn balance" );

             if( has_auth( owner ) ) {
                futureio_assert( !st.can_freeze || !from.frozen, "account is frozen by issuer" );
                futureio_assert( !st.can_freeze || !st.is_frozen, "all transfers are frozen by issuer" );
                futureio_assert( !st.enforce_whitelist || from.whitelist, "account is not white listed" );
             } else if( has_auth( st.issuer ) ) {
                futureio_assert( st.can_recall, "issuer may not recall token" );
             } else {
                futureio_assert( false, "insufficient authority" );
             }

             from_acnts.modify( from, [&]( auto& a ) {
                 a.balance.amount -= value.amount;
             });
          }

          void add_balance( account_name owner, asset value, const currency_stats& st )
          {
             accounts to_acnts( _contract, owner );
             auto to = to_acnts.find( value.symbol.name() );
             if( to == to_acnts.end() ) {
                futureio_assert( !st.enforce_whitelist, "can only transfer to white listed accounts" );
                to_acnts.emplace( [&]( auto& a ){
                  a.balance = value;
                });
             } else {
                futureio_assert( !st.enforce_whitelist || to->whitelist, "receiver requires whitelist by issuer" );
                to_acnts.modify( to, [&]( auto& a ) {
                  a.balance.amount += value.amount;
                });
             }
          }

      private:
         account_name _contract;
   };

}