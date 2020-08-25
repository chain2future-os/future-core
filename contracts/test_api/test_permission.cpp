/**
 * @file action_test.cpp
 * @copyright defined in future/LICENSE.txt
 */
#include <futureiolib/permission.h>
#include <futureiolib/db.h>

#include <futureiolib/futureio.hpp>
#include <futureiolib/print.hpp>
#include <futureiolib/compiler_builtins.h>
#include <futureiolib/serialize.hpp>
#include <futureiolib/action.hpp>

#include "test_api.hpp"

#include <limits>

struct check_auth_msg {
   account_name         account;
   permission_name      permission;
   std::vector<public_key>   pubkeys;

   FUTURELIB_SERIALIZE( check_auth_msg, (account)(permission)(pubkeys)  )
};

void test_permission::check_authorization(uint64_t receiver, uint64_t code, action_name action) {
   (void)code;
   (void)action;
   using namespace futureio;

   auto self = receiver;
   auto params = unpack_action_data<check_auth_msg>();
   auto packed_pubkeys = pack(params.pubkeys);
   int64_t res64 = ::check_permission_authorization( params.account,
                                                     params.permission,
                                                     packed_pubkeys.data(), packed_pubkeys.size(),
                                                     (const char*)0,        0,
                                                     static_cast<uint64_t>(std::numeric_limits<int64_t>::max())
                                                   );
   /*
   uint64_t res64 = (uint64_t)::check_authorization( params.account, params.permission,
        (char*)params.pubkeys.data(), params.pubkeys.size()*sizeof(public_key) );
   */

   auto itr = db_lowerbound_i64(self, self, self, 1);
   if(itr == -1) {
      db_store_i64(self, self, self, 1, &res64, sizeof(int64_t));
   } else {
      db_update_i64(itr, self, &res64, sizeof(int64_t));
   }
}

struct test_permission_last_used_msg {
   account_name     account;
   permission_name  permission;
   int64_t          last_used_time;

   FUTURELIB_SERIALIZE( test_permission_last_used_msg, (account)(permission)(last_used_time) )
};

void test_permission::test_permission_last_used(uint64_t /* receiver */, uint64_t code, action_name action) {
   (void)code;
   (void)action;
   using namespace futureio;

   auto params = unpack_action_data<test_permission_last_used_msg>();

   futureio_assert( get_permission_last_used(params.account, params.permission) == params.last_used_time, "unexpected last used permission time" );
}

void test_permission::test_account_creation_time(uint64_t /* receiver */, uint64_t code, action_name action) {
   (void)code;
   (void)action;
   using namespace futureio;

   auto params = unpack_action_data<test_permission_last_used_msg>();

   futureio_assert( get_account_creation_time(params.account) == params.last_used_time, "unexpected account creation time" );
}
