#pragma once 
#include <futureiolib/varint.hpp>
#include <futureiolib/serialize.hpp>

namespace futureio {

   /**
   *  @defgroup publickeytype Public Key Type
   *  @ingroup types
   *  @brief Specifies public key type
   *
   *  @{
   */
   
   /**
    * FUTUREIO Public Key
    * @brief FUTUREIO Public Key
    */
   struct public_key {
      /**
       * Type of the public key, could be either K1 or R1
       * @brief Type of the public key
       */
      unsigned_int        type;

      /**
       * Bytes of the public key
       * 
       * @brief Bytes of the public key
       */
      std::array<char,33> data;

      friend bool operator == ( const public_key& a, const public_key& b ) {
        return std::tie(a.type,a.data) == std::tie(b.type,b.data);
      }
      friend bool operator != ( const public_key& a, const public_key& b ) {
        return std::tie(a.type,a.data) != std::tie(b.type,b.data);
      }
      FUTURELIB_SERIALIZE( public_key, (type)(data) )
   };
   
}

/// @} publickeytype
