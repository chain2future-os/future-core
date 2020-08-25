/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#pragma once

#ifdef DISABLE_FUTURELIB_SERIALIZE
#include <futureiolib/serialize.hpp>
#undef FUTURELIB_SERIALIZE
#define FUTURELIB_SERIALIZE(...)
#endif

static constexpr unsigned int DJBH(const char* cp)
{
  unsigned int hash = 5381;
  while (*cp)
      hash = 33 * hash ^ (unsigned char) *cp++;
  return hash;
}

static constexpr action_name WASM_TEST_ACTION_EX(const char* cls, const char* method)
{
    uint64_t l = static_cast<unsigned long long>(DJBH(cls)) << 32 | static_cast<unsigned long long>(DJBH(method));
  return action_name(0, l);
}

static constexpr uint64_t WASM_TEST_ACTION(const char* cls, const char* method) {
    return WASM_TEST_ACTION_EX(cls, method).valueL;
}

#pragma pack(push, 1)
struct dummy_action {
   static action_name get_name() {
      return NEX(dummy_action);
   }
   static uint64_t get_account() {
      return N(testapi);
   }

  char a; //1
  uint64_t b; //8
  int32_t  c; //4

  FUTURELIB_SERIALIZE( dummy_action, (a)(b)(c) )
};

struct u128_action {
  unsigned __int128  values[3]; //16*3

  FUTURELIB_SERIALIZE( u128_action, (values) )
};

struct cf_action {
   static action_name get_name() {
      return NEX(cf_action);
   }
   static uint64_t get_account() {
      return N(testapi);
   }

   uint32_t       payload = 100;
   uint32_t       cfd_idx = 0; // context free data index

   FUTURELIB_SERIALIZE( cf_action, (payload)(cfd_idx) )
};

// Deferred Transaction Trigger Action
struct dtt_action {
   static action_name get_name() {
      return WASM_TEST_ACTION_EX("test_transaction", "send_deferred_tx_with_dtt_action");
   }
   static uint64_t get_account() {
      return N(testapi);
   }

   uint64_t       payer = N(testapi);
   uint64_t       deferred_account = N(testapi);
   action_name       deferred_action = WASM_TEST_ACTION_EX("test_transaction", "deferred_print");
   uint64_t       permission_name = N(active);
   uint32_t       delay_sec = 2;

   FUTURELIB_SERIALIZE( dtt_action, (payer)(deferred_account)(deferred_action)(permission_name)(delay_sec) )
};

#pragma pack(pop)

static_assert( sizeof(dummy_action) == 13 , "unexpected packing" );
static_assert( sizeof(u128_action) == 16*3 , "unexpected packing" );

#define DUMMY_ACTION_DEFAULT_A 0x45
#define DUMMY_ACTION_DEFAULT_B 0xab11cd1244556677
#define DUMMY_ACTION_DEFAULT_C 0x7451ae12

struct invalid_access_action {
   uint64_t code;
   uint64_t val;
   uint32_t index;
   bool store;

   FUTURELIB_SERIALIZE( invalid_access_action, (code)(val)(index)(store) )
};