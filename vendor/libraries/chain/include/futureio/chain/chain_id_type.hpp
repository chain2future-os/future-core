/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#pragma once

#include <fc/crypto/sha256.hpp>

struct hello;

namespace futureio {

   namespace net_plugin_n {
     class net_plugin_impl;
   }

   namespace kcp_plugin_n {
     class kcp_plugin_impl;
   }

   struct handshake_message;

   namespace chain_apis {
      class read_only;
   }

namespace chain {

   struct chain_id_type : public fc::sha256 {
      using fc::sha256::sha256;

      template<typename T>
      inline friend T& operator<<( T& ds, const chain_id_type& cid ) {
        ds.write( cid.data(), cid.data_size() );
        return ds;
      }

      template<typename T>
      inline friend T& operator>>( T& ds, chain_id_type& cid ) {
        ds.read( cid.data(), cid.data_size() );
        return ds;
      }

      void reflector_verify()const;

      private:
         chain_id_type() = default;

         // Some exceptions are unfortunately necessary:
         template<typename T>
         friend T fc::variant::as()const;

         friend class futureio::chain_apis::read_only;

         friend class futureio::net_plugin_n::net_plugin_impl;
         friend class futureio::kcp_plugin_n::kcp_plugin_impl;
         friend struct futureio::handshake_message;

         friend struct ::hello; // TODO: Rushed hack to support bnet_plugin. Need a better solution.
   };

} }  // namespace futureio::chain

namespace fc {
  class variant;
  void to_variant(const futureio::chain::chain_id_type& cid, fc::variant& v);
  void from_variant(const fc::variant& v, futureio::chain::chain_id_type& cid);
} // fc
