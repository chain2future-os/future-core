/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#pragma once
#include <fc/reflect/reflect.hpp>
#include <futureio/chain/types.hpp>
#include <futureio/chain/database_utils.hpp>
#include <chainbase/chainbase.hpp>

namespace futureio { namespace chain {
    class whiteblacklist_object : public chainbase::object<whiteblacklist_object_type, whiteblacklist_object> {
        OBJECT_CTOR(whiteblacklist_object,(actor_whitelist)(actor_blacklist)(contract_whitelist)(contract_blacklist)(action_blacklist)(key_blacklist))

        id_type                                id;
        shared_set<account_name>               actor_whitelist;
        shared_set<account_name>               actor_blacklist;
        shared_set<account_name>               contract_whitelist;
        shared_set<account_name>               contract_blacklist;
        shared_set<pair<account_name, action_name>>  action_blacklist;
        shared_set<public_key_type>            key_blacklist;
    };

    using whiteblacklist_index = chainbase::shared_multi_index_container<
        whiteblacklist_object,
        indexed_by<
            ordered_unique<tag<by_id>,member<whiteblacklist_object,whiteblacklist_object::id_type,&whiteblacklist_object::id>>
            >
        >;
}}

CHAINBASE_SET_INDEX_TYPE(futureio::chain::whiteblacklist_object, futureio::chain::whiteblacklist_index)

FC_REFLECT(futureio::chain::whiteblacklist_object,(actor_whitelist)(actor_blacklist)(contract_whitelist)(contract_blacklist)(action_blacklist)(key_blacklist))
