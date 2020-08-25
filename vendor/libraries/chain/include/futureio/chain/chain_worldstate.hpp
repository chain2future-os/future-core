/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#pragma once

#include <futureio/chain/exceptions.hpp>

namespace futureio { namespace chain {

struct chain_worldstate_header {
   /**
    * Version history
    *   1: initial version
    */

   static constexpr uint32_t minimum_compatible_version = 1;
   static constexpr uint32_t current_version = 1;

   uint32_t version = current_version;

   void validate() const {
      auto min = minimum_compatible_version;
      auto max = current_version;
      FUTURE_ASSERT(version >= min && version <= max,
              worldstate_validation_exception,
              "Unsupported version of chain worldstate: ${version}. Supported version must be between ${min} and ${max} inclusive.",
              ("version",version)("min",min)("max",max));
   }
};

} }

FC_REFLECT(futureio::chain::chain_worldstate_header,(version))
