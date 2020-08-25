/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */

#include <futureio/utilities/tempdir.hpp>

#include <cstdlib>

namespace futureio { namespace utilities {

fc::path temp_directory_path()
{
   const char* future_tempdir = getenv("FUTURE_TEMPDIR");
   if( future_tempdir != nullptr )
      return fc::path( future_tempdir );
   return fc::temp_directory_path() / "future-tmp";
}

} } // futureio::utilities
