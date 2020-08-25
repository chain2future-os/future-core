#pragma once

#include <futureio/chain/authority.hpp>
#include <futureio/chain/chain_config.hpp>
#include <futureio/chain/config.hpp>
#include <futureio/chain/types.hpp>

namespace futureio { namespace chain {

using action_name    = futureio::chain::action_name;

struct newaccount {
   account_name                     creator;
   account_name                     name;
   authority                        owner;
   authority                        active;
   bool                             updateable;

   static account_name get_account() {
      return account_name(config::system_account_name);
   }

   static action_name get_name() {
      return NEX(newaccount);
   }
};

struct setcode {
   account_name                     account;
   uint8_t                          vmtype = 0;
   uint8_t                          vmversion = 0;
   bytes                            code;

   static account_name get_account() {
      return account_name(config::system_account_name);
   }

   static action_name get_name() {
      return NEX(setcode);
   }
};

struct setabi {
   account_name                     account;
   bytes                            abi;

   static account_name get_account() {
      return account_name(config::system_account_name);
   }

   static action_name get_name() {
      return NEX(setabi);
   }
};


struct updateauth {
   account_name                      account;
   permission_name                   permission;
   permission_name                   parent;
   authority                         auth;

   static account_name get_account() {
      return account_name(config::system_account_name);
   }

   static action_name get_name() {
      return NEX(updateauth);
   }
};

struct deleteauth {
   deleteauth() = default;
   deleteauth(const account_name& account, const permission_name& permission)
   :account(account), permission(permission)
   {}

   account_name                      account;
   permission_name                   permission;

   static account_name get_account() {
      return account_name(config::system_account_name);
   }

   static action_name get_name() {
      return NEX(deleteauth);
   }
};

struct linkauth {
   linkauth() = default;
   linkauth(const account_name& account, const account_name& code, const action_name& type, const permission_name& requirement)
   :account(account), code(code), type(type), requirement(requirement)
   {}

   account_name                      account;
   account_name                      code;
   action_name                       type;
   permission_name                   requirement;

   static account_name get_account() {
      return account_name(config::system_account_name);
   }

   static action_name get_name() {
      return NEX(linkauth);
   }
};

struct unlinkauth {
   unlinkauth() = default;
   unlinkauth(const account_name& account, const account_name& code, const action_name& type)
   :account(account), code(code), type(type)
   {}

   account_name                      account;
   account_name                      code;
   action_name                       type;

   static account_name get_account() {
      return account_name(config::system_account_name);
   }

   static action_name get_name() {
      return NEX(unlinkauth);
   }
};

struct canceldelay {
   permission_level      canceling_auth;
   transaction_id_type   trx_id;

   static account_name get_account() {
      return account_name(config::system_account_name);
   }

   static action_name get_name() {
      return NEX(canceldelay);
   }
};

struct onerror {
   uint128_t      sender_id;
   bytes          sent_trx;

   onerror( uint128_t sid, const char* data, size_t len )
   :sender_id(sid),sent_trx(data,data+len){}

   static account_name get_account() {
      return account_name(config::system_account_name);
   }

   static action_name get_name() {
      return NEX(onerror);
   }
};

struct delaccount {
   account_name    account;

   static account_name get_account() {
      return account_name(config::system_account_name);
   }

   static action_name get_name() {
      return NEX(delaccount);
   }
};

struct action_actor_type {
   account_name   actor;
   action_name    action;
};

struct white_black_type {
   std::vector<account_name>           actor_black;
   std::vector<account_name>           actor_white;
   std::vector<account_name>           contract_black;
   std::vector<account_name>           contract_white;
   std::vector<action_actor_type>      action_black;
   std::vector<public_key_type>        key_black;
};

struct addwhiteblack {
   white_black_type  add_white_black;
   static account_name get_account() {
      return account_name(config::system_account_name);
   }

   static action_name get_name() {
      return NEX(addwhiteblack);
   }
};

struct rmwhiteblack {
   white_black_type  rm_white_black;
   static account_name get_account() {
      return account_name(config::system_account_name);
   }

   static action_name get_name() {
      return NEX(rmwhiteblack);
   }
};


} } /// namespace futureio::chain

FC_REFLECT( futureio::chain::newaccount                       , (creator)(name)(owner)(active)(updateable) )
FC_REFLECT( futureio::chain::setcode                          , (account)(vmtype)(vmversion)(code) )
FC_REFLECT( futureio::chain::setabi                           , (account)(abi) )
FC_REFLECT( futureio::chain::updateauth                       , (account)(permission)(parent)(auth) )
FC_REFLECT( futureio::chain::deleteauth                       , (account)(permission) )
FC_REFLECT( futureio::chain::linkauth                         , (account)(code)(type)(requirement) )
FC_REFLECT( futureio::chain::unlinkauth                       , (account)(code)(type) )
FC_REFLECT( futureio::chain::canceldelay                      , (canceling_auth)(trx_id) )
FC_REFLECT( futureio::chain::onerror                          , (sender_id)(sent_trx) )
FC_REFLECT( futureio::chain::delaccount                       , (account) )
FC_REFLECT( futureio::chain::action_actor_type                , (actor)(action) )
FC_REFLECT( futureio::chain::white_black_type                 , (actor_black)(actor_white)(contract_black)(contract_white)(action_black)(key_black) )
FC_REFLECT( futureio::chain::addwhiteblack                    , (add_white_black) )
FC_REFLECT( futureio::chain::rmwhiteblack                     , (rm_white_black) )
