/**
 * @file action_test.cpp
 * @copyright defined in future/LICENSE.txt
 */
#include <futureiolib/action.hpp>
#include <futureiolib/transaction.hpp>
#include <futureiolib/chain.h>
#include <futureiolib/db.h>
#include <futureiolib/crypto.h>
#include <futureiolib/privileged.h>
#include <futureiolib/futureio.hpp>
#include <futureiolib/datastream.hpp>
#include <futureiolib/print.hpp>
#include <futureiolib/compiler_builtins.h>
#include "test_api.hpp"

void test_action::read_action_normal() {

   char buffer[100];
   uint32_t total = 0;

   futureio_assert(action_data_size() == sizeof(dummy_action), "action_size() == sizeof(dummy_action)");

   total = read_action_data(buffer, 30);
   futureio_assert(total == sizeof(dummy_action) , "read_action(30)" );

   total = read_action_data(buffer, 100);
   futureio_assert(total == sizeof(dummy_action) , "read_action(100)" );

   total = read_action_data(buffer, 5);
   futureio_assert(total == 5 , "read_action(5)" );

   total = read_action_data(buffer, sizeof(dummy_action) );
   futureio_assert(total == sizeof(dummy_action), "read_action(sizeof(dummy_action))" );

   dummy_action *dummy13 = reinterpret_cast<dummy_action *>(buffer);

   futureio_assert(dummy13->a == DUMMY_ACTION_DEFAULT_A, "dummy13->a == DUMMY_ACTION_DEFAULT_A");
   futureio_assert(dummy13->b == DUMMY_ACTION_DEFAULT_B, "dummy13->b == DUMMY_ACTION_DEFAULT_B");
   futureio_assert(dummy13->c == DUMMY_ACTION_DEFAULT_C, "dummy13->c == DUMMY_ACTION_DEFAULT_C");
}

void test_action::test_dummy_action() {
   char buffer[100];
   int total = 0;

   // get_action
   total = get_action( 1, 0, buffer, 0 );
   total = get_action( 1, 0, buffer, static_cast<size_t>(total) );
   futureio_assert( total > 0, "get_action failed" );
   futureio::action act = futureio::get_action( 1, 0 );
   futureio_assert( act.authorization.back().actor == N(testapi), "incorrect permission actor" );
   futureio_assert( act.authorization.back().permission == N(active), "incorrect permission name" );
   futureio_assert( futureio::pack_size(act) == static_cast<size_t>(total), "pack_size does not match get_action size" );
   futureio_assert( act.account == N(testapi), "expected testapi account" );

   dummy_action dum13 = act.data_as<dummy_action>();

   if ( dum13.b == 200 ) {
      // attempt to access context free only api
      get_context_free_data( 0, nullptr, 0 );
      futureio_assert(false, "get_context_free_data() not allowed in non-context free action");
   } else {
      futureio_assert(dum13.a == DUMMY_ACTION_DEFAULT_A, "dum13.a == DUMMY_ACTION_DEFAULT_A");
      futureio_assert(dum13.b == DUMMY_ACTION_DEFAULT_B, "dum13.b == DUMMY_ACTION_DEFAULT_B");
      futureio_assert(dum13.c == DUMMY_ACTION_DEFAULT_C, "dum13.c == DUMMY_ACTION_DEFAULT_C");
   }
}

void test_action::read_action_to_0() {
   read_action_data((void *)0, action_data_size());
}

void test_action::read_action_to_64k() {
   read_action_data( (void *)((1<<16)-2), action_data_size());
}

void test_action::test_cf_action() {

   futureio::action act = futureio::get_action( 0, 0 );
   cf_action cfa = act.data_as<cf_action>();
   if ( cfa.payload == 100 ) {
      // verify read of get_context_free_data, also verifies system api access
      int size = get_context_free_data( cfa.cfd_idx, nullptr, 0 );
      futureio_assert( size > 0, "size determination failed" );
      futureio::bytes cfd( static_cast<size_t>(size) );
      size = get_context_free_data( cfa.cfd_idx, &cfd[0], static_cast<size_t>(size) );
      futureio_assert(static_cast<size_t>(size) == cfd.size(), "get_context_free_data failed" );
      uint32_t v = futureio::unpack<uint32_t>( &cfd[0], cfd.size() );
      futureio_assert( v == cfa.payload, "invalid value" );

      // verify crypto api access
      checksum256 hash;
      char test[] = "test";
      sha256( test, sizeof(test), &hash );
      assert_sha256( test, sizeof(test), &hash );
      // verify action api access
      action_data_size();
      // verify console api access
      futureio::print("test\n");
      // verify memory api access
      uint32_t i = 42;
      memccpy(&v, &i, sizeof(i), sizeof(i));
      // verify transaction api access
      futureio_assert(transaction_size() > 0, "transaction_size failed");
      // verify softfloat api access
      float f1 = 1.0f, f2 = 2.0f;
      float f3 = f1 + f2;
      futureio_assert( f3 >  2.0f, "Unable to add float.");
      // verify compiler builtin api access
      __int128 ret;
      __divti3(ret, 2, 2, 2, 2);
      // verify context_free_system_api
      futureio_assert( true, "verify futureio_assert can be called" );


   } else if ( cfa.payload == 200 ) {
      // attempt to access non context free api, privileged_api
      is_privileged(act.account);
      futureio_assert( false, "privileged_api should not be allowed" );
   } else if ( cfa.payload == 201 ) {
      // attempt to access non context free api, producer_api
      get_active_producers( nullptr, 0 );
      futureio_assert( false, "producer_api should not be allowed" );
   } else if ( cfa.payload == 202 ) {
      // attempt to access non context free api, db_api
      db_store_i64( N(testapi), N(testapi), N(testapi), 0, "test", 4 );
      futureio_assert( false, "db_api should not be allowed" );
   } else if ( cfa.payload == 203 ) {
      // attempt to access non context free api, db_api
      uint64_t i = 0;
      db_idx64_store( N(testapi), N(testapi), N(testapi), 0, &i );
      futureio_assert( false, "db_api should not be allowed" );
   } else if ( cfa.payload == 204 ) {
      db_find_i64( N(testapi), N(testapi), N(testapi), 1);
      futureio_assert( false, "db_api should not be allowed" );
   } else if ( cfa.payload == 205 ) {
      // attempt to access non context free api, send action
      futureio::action dum_act;
      dum_act.send();
      futureio_assert( false, "action send should not be allowed" );
   } else if ( cfa.payload == 206 ) {
      futureio::require_auth(N(test));
      futureio_assert( false, "authorization_api should not be allowed" );
   } else if ( cfa.payload == 207 ) {
      now();
      futureio_assert( false, "system_api should not be allowed" );
   } else if ( cfa.payload == 208 ) {
      current_time();
      futureio_assert( false, "system_api should not be allowed" );
   } else if ( cfa.payload == 209 ) {
      publication_time();
      futureio_assert( false, "system_api should not be allowed" );
   } else if ( cfa.payload == 210 ) {
      send_inline( (char*)"hello", 6 );
      futureio_assert( false, "transaction_api should not be allowed" );
   } else if ( cfa.payload == 211 ) {
      send_deferred( N(testapi), N(testapi), "hello", 6 );
      futureio_assert( false, "transaction_api should not be allowed" );
   }

}

void test_action::require_notice(uint64_t receiver, uint64_t code, action_name action) {
   (void)code;(void)action;
   if( receiver == N(testapi) ) {
      futureio::require_recipient( N(acc1) );
      futureio::require_recipient( N(acc2) );
      futureio::require_recipient( N(acc1), N(acc2) );
      futureio_assert(false, "Should've failed");
   } else if ( receiver == N(acc1) || receiver == N(acc2) ) {
      return;
   }
   futureio_assert(false, "Should've failed");
}

void test_action::require_auth() {
   prints("require_auth");
   futureio::require_auth( N(acc3) );
   futureio::require_auth( N(acc4) );
}

void test_action::assert_false() {
   futureio_assert(false, "test_action::assert_false");
}

void test_action::assert_true() {
   futureio_assert(true, "test_action::assert_true");
}

void test_action::assert_true_cf() {
   futureio_assert(true, "test_action::assert_true");
}

void test_action::test_abort() {
   abort();
   futureio_assert( false, "should've aborted" );
}

void test_action::test_publication_time() {
   uint64_t pub_time = 0;
   uint32_t total = read_action_data(&pub_time, sizeof(uint64_t));
   futureio_assert( total == sizeof(uint64_t), "total == sizeof(uint64_t)");
   futureio_assert( pub_time == publication_time(), "pub_time == publication_time()" );
}

void test_action::test_current_receiver(uint64_t receiver, uint64_t code, action_name action) {
   (void)code;(void)action;
   account_name cur_rec;
   read_action_data(&cur_rec, sizeof(account_name));

   futureio_assert( receiver == cur_rec, "the current receiver does not match" );
}

void test_action::test_current_time() {
   uint64_t tmp = 0;
   uint32_t total = read_action_data(&tmp, sizeof(uint64_t));
   futureio_assert( total == sizeof(uint64_t), "total == sizeof(uint64_t)");
   futureio_assert( tmp == current_time(), "tmp == current_time()" );
}

void test_action::test_assert_code() {
   uint64_t code = 0;
   uint32_t total = read_action_data(&code, sizeof(uint64_t));
   futureio_assert( total == sizeof(uint64_t), "total == sizeof(uint64_t)");
   futureio_assert_code( false, code );
}
