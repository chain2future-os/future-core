/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#pragma once
#include <fc/uint128.hpp>
#include <fc/array.hpp>

#include <futureio/chain/types.hpp>
#include <futureio/chain/block_timestamp.hpp>
#include <futureio/chain/chain_config.hpp>
#include <futureio/chain/incremental_merkle.hpp>
#include <chainbase/chainbase.hpp>
#include "multi_index_includes.hpp"

namespace futureio { namespace chain {

   /**
    * @class global_property_object
    * @brief Maintains global state information (committee_member list, current fees)
    * @ingroup object
    * @ingroup implementation
    *
    * This is an implementation detail. The values here are set by committee_members to tune the blockchain parameters.
    */
   class global_property_object : public chainbase::object<global_property_object_type, global_property_object>
   {
      OBJECT_CTOR(global_property_object)

      id_type                           id;
      chain_config                      configuration;
   };

   /**
    * @class dynamic_global_property_object
    * @brief Maintains global state information (committee_member list, current fees)
    * @ingroup object
    * @ingroup implementation
    *
    * This is an implementation detail. The values here are calculated during normal chain operations and reflect the
    * current values of global blockchain properties.
    */
   class dynamic_global_property_object : public chainbase::object<dynamic_global_property_object_type, dynamic_global_property_object>
   {
        OBJECT_CTOR(dynamic_global_property_object)

        id_type    id;
        uint64_t   global_action_sequence = 0;
   };

   using global_property_multi_index = chainbase::shared_multi_index_container<
      global_property_object,
      indexed_by<
         ordered_unique<tag<by_id>,
            BOOST_MULTI_INDEX_MEMBER(global_property_object, global_property_object::id_type, id)
         >
      >
   >;

   using dynamic_global_property_multi_index = chainbase::shared_multi_index_container<
      dynamic_global_property_object,
      indexed_by<
         ordered_unique<tag<by_id>,
            BOOST_MULTI_INDEX_MEMBER(dynamic_global_property_object, dynamic_global_property_object::id_type, id)
         >
      >
   >;

}}

CHAINBASE_SET_INDEX_TYPE(futureio::chain::global_property_object, futureio::chain::global_property_multi_index)
CHAINBASE_SET_INDEX_TYPE(futureio::chain::dynamic_global_property_object,
                         futureio::chain::dynamic_global_property_multi_index)

FC_REFLECT(futureio::chain::dynamic_global_property_object,
           (global_action_sequence)
          )

FC_REFLECT(futureio::chain::global_property_object,
          (configuration)
          )
