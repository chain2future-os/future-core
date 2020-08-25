#include <futureiolib/futureio.hpp>
using namespace futureio;

class hello : public futureio::contract {
  public:
      using contract::contract;

      /// @abi action
      void hi( account_name user ) {
         //print( "Hello, ", name{user} );
         (void)user;
         set_result_str("Hi-Successed.");
      }

      void TestALongNameFunction( account_name user ) {
         print( "TestALongNameFunction, ", name{user} );

         set_result_str("Hi-Successed.");
      }
};

FUTUREIO_ABI( hello, (hi)(TestALongNameFunction) )
