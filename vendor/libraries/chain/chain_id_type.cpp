/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */

#include <futureio/chain/chain_id_type.hpp>
#include <futureio/chain/exceptions.hpp>

namespace futureio { namespace chain {

   void chain_id_type::reflector_verify()const {
      FUTURE_ASSERT( *reinterpret_cast<const fc::sha256*>(this) != fc::sha256(), chain_id_type_exception, "chain_id_type cannot be zero" );
   }

} }  // namespace futureio::chain

namespace fc {

   void to_variant(const futureio::chain::chain_id_type& cid, fc::variant& v) {
      to_variant( static_cast<const fc::sha256&>(cid), v);
   }

   void from_variant(const fc::variant& v, futureio::chain::chain_id_type& cid) {
      from_variant( v, static_cast<fc::sha256&>(cid) );
   }

} // fc
