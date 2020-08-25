/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#include <futureiolib/futureio.hpp>

#include "test_api.hpp"

void test_types::types_size() {

   futureio_assert( sizeof(int64_t) == 8, "int64_t size != 8");
   futureio_assert( sizeof(uint64_t) ==  8, "uint64_t size != 8");
   futureio_assert( sizeof(uint32_t) ==  4, "uint32_t size != 4");
   futureio_assert( sizeof(int32_t) ==  4, "int32_t size != 4");
   futureio_assert( sizeof(uint128_t) == 16, "uint128_t size != 16");
   futureio_assert( sizeof(int128_t) == 16, "int128_t size != 16");
   futureio_assert( sizeof(uint8_t) ==  1, "uint8_t size != 1");

   futureio_assert( sizeof(account_name) ==  8, "account_name size !=  8");
   futureio_assert( sizeof(table_name) ==  8, "table_name size !=  8");
   futureio_assert( sizeof(time) ==  4, "time size !=  4");
   futureio_assert( sizeof(futureio::key256) == 32, "key256 size != 32" );
}

void test_types::char_to_symbol() {

   futureio_assert( futureio::char_to_symbol('1') ==  1, "futureio::char_to_symbol('1') !=  1");
   futureio_assert( futureio::char_to_symbol('2') ==  2, "futureio::char_to_symbol('2') !=  2");
   futureio_assert( futureio::char_to_symbol('3') ==  3, "futureio::char_to_symbol('3') !=  3");
   futureio_assert( futureio::char_to_symbol('4') ==  4, "futureio::char_to_symbol('4') !=  4");
   futureio_assert( futureio::char_to_symbol('5') ==  5, "futureio::char_to_symbol('5') !=  5");
   futureio_assert( futureio::char_to_symbol('a') ==  6, "futureio::char_to_symbol('a') !=  6");
   futureio_assert( futureio::char_to_symbol('b') ==  7, "futureio::char_to_symbol('b') !=  7");
   futureio_assert( futureio::char_to_symbol('c') ==  8, "futureio::char_to_symbol('c') !=  8");
   futureio_assert( futureio::char_to_symbol('d') ==  9, "futureio::char_to_symbol('d') !=  9");
   futureio_assert( futureio::char_to_symbol('e') == 10, "futureio::char_to_symbol('e') != 10");
   futureio_assert( futureio::char_to_symbol('f') == 11, "futureio::char_to_symbol('f') != 11");
   futureio_assert( futureio::char_to_symbol('g') == 12, "futureio::char_to_symbol('g') != 12");
   futureio_assert( futureio::char_to_symbol('h') == 13, "futureio::char_to_symbol('h') != 13");
   futureio_assert( futureio::char_to_symbol('i') == 14, "futureio::char_to_symbol('i') != 14");
   futureio_assert( futureio::char_to_symbol('j') == 15, "futureio::char_to_symbol('j') != 15");
   futureio_assert( futureio::char_to_symbol('k') == 16, "futureio::char_to_symbol('k') != 16");
   futureio_assert( futureio::char_to_symbol('l') == 17, "futureio::char_to_symbol('l') != 17");
   futureio_assert( futureio::char_to_symbol('m') == 18, "futureio::char_to_symbol('m') != 18");
   futureio_assert( futureio::char_to_symbol('n') == 19, "futureio::char_to_symbol('n') != 19");
   futureio_assert( futureio::char_to_symbol('o') == 20, "futureio::char_to_symbol('o') != 20");
   futureio_assert( futureio::char_to_symbol('p') == 21, "futureio::char_to_symbol('p') != 21");
   futureio_assert( futureio::char_to_symbol('q') == 22, "futureio::char_to_symbol('q') != 22");
   futureio_assert( futureio::char_to_symbol('r') == 23, "futureio::char_to_symbol('r') != 23");
   futureio_assert( futureio::char_to_symbol('s') == 24, "futureio::char_to_symbol('s') != 24");
   futureio_assert( futureio::char_to_symbol('t') == 25, "futureio::char_to_symbol('t') != 25");
   futureio_assert( futureio::char_to_symbol('u') == 26, "futureio::char_to_symbol('u') != 26");
   futureio_assert( futureio::char_to_symbol('v') == 27, "futureio::char_to_symbol('v') != 27");
   futureio_assert( futureio::char_to_symbol('w') == 28, "futureio::char_to_symbol('w') != 28");
   futureio_assert( futureio::char_to_symbol('x') == 29, "futureio::char_to_symbol('x') != 29");
   futureio_assert( futureio::char_to_symbol('y') == 30, "futureio::char_to_symbol('y') != 30");
   futureio_assert( futureio::char_to_symbol('z') == 31, "futureio::char_to_symbol('z') != 31");

   for(unsigned char i = 0; i<255; i++) {
      if((i >= 'a' && i <= 'z') || (i >= '1' || i <= '5')) continue;
      futureio_assert( futureio::char_to_symbol((char)i) == 0, "futureio::char_to_symbol() != 0");
   }
}

void test_types::string_to_name() {

   futureio_assert( futureio::string_to_name("a") == N(a) , "futureio::string_to_name(a)" );
   futureio_assert( futureio::string_to_name("ba") == N(ba) , "futureio::string_to_name(ba)" );
   futureio_assert( futureio::string_to_name("cba") == N(cba) , "futureio::string_to_name(cba)" );
   futureio_assert( futureio::string_to_name("dcba") == N(dcba) , "futureio::string_to_name(dcba)" );
   futureio_assert( futureio::string_to_name("edcba") == N(edcba) , "futureio::string_to_name(edcba)" );
   futureio_assert( futureio::string_to_name("fedcba") == N(fedcba) , "futureio::string_to_name(fedcba)" );
   futureio_assert( futureio::string_to_name("gfedcba") == N(gfedcba) , "futureio::string_to_name(gfedcba)" );
   futureio_assert( futureio::string_to_name("hgfedcba") == N(hgfedcba) , "futureio::string_to_name(hgfedcba)" );
   futureio_assert( futureio::string_to_name("ihgfedcba") == N(ihgfedcba) , "futureio::string_to_name(ihgfedcba)" );
   futureio_assert( futureio::string_to_name("jihgfedcba") == N(jihgfedcba) , "futureio::string_to_name(jihgfedcba)" );
   futureio_assert( futureio::string_to_name("kjihgfedcba") == N(kjihgfedcba) , "futureio::string_to_name(kjihgfedcba)" );
   futureio_assert( futureio::string_to_name("lkjihgfedcba") == N(lkjihgfedcba) , "futureio::string_to_name(lkjihgfedcba)" );
   futureio_assert( futureio::string_to_name("mlkjihgfedcba") == N(mlkjihgfedcba) , "futureio::string_to_name(mlkjihgfedcba)" );
   futureio_assert( futureio::string_to_name("mlkjihgfedcba1") == N(mlkjihgfedcba2) , "futureio::string_to_name(mlkjihgfedcba2)" );
   futureio_assert( futureio::string_to_name("mlkjihgfedcba55") == N(mlkjihgfedcba14) , "futureio::string_to_name(mlkjihgfedcba14)" );

   futureio_assert( futureio::string_to_name("azAA34") == N(azBB34) , "futureio::string_to_name N(azBB34)" );
   futureio_assert( futureio::string_to_name("AZaz12Bc34") == N(AZaz12Bc34) , "futureio::string_to_name AZaz12Bc34" );
   futureio_assert( futureio::string_to_name("AAAAAAAAAAAAAAA") == futureio::string_to_name("BBBBBBBBBBBBBDDDDDFFFGG") , "futureio::string_to_name BBBBBBBBBBBBBDDDDDFFFGG" );
}

void test_types::name_class() {

   futureio_assert( futureio::name{futureio::string_to_name("azAA34")}.value == N(azAA34), "futureio::name != N(azAA34)" );
   futureio_assert( futureio::name{futureio::string_to_name("AABBCC")}.value == 0, "futureio::name != N(0)" );
   futureio_assert( futureio::name{futureio::string_to_name("AA11")}.value == N(AA11), "futureio::name != N(AA11)" );
   futureio_assert( futureio::name{futureio::string_to_name("11AA")}.value == N(11), "futureio::name != N(11)" );
   futureio_assert( futureio::name{futureio::string_to_name("22BBCCXXAA")}.value == N(22), "futureio::name != N(22)" );
   futureio_assert( futureio::name{futureio::string_to_name("AAAbbcccdd")} == futureio::name{futureio::string_to_name("AAAbbcccdd")}, "futureio::name == futureio::name" );

   uint64_t tmp = futureio::name{futureio::string_to_name("11bbcccdd")};
   futureio_assert(N(11bbcccdd) == tmp, "N(11bbcccdd) == tmp");
}
