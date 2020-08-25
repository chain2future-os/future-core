/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#pragma once

#include <string>
#include <fc/crypto/elliptic.hpp>
#include <fc/optional.hpp>

namespace futureio { namespace utilities {

#ifndef FUTURE_TRX_SUPPORT_GM
std::string                        key_to_wif(const fc::ecc::private_key& key);
fc::optional<fc::ecc::private_key> wif_to_key( const std::string& wif_key );
#endif

} } // end namespace futureio::utilities
