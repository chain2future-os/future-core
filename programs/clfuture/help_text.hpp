/**
 *  @file
 *  @copyright defined in future/LICENSE.txt
 */
#pragma once
#include <fc/exception/exception.hpp>

namespace futureio { namespace client { namespace help {
   bool print_recognized_errors(const fc::exception& e, const bool verbose_errors);
   bool print_help_text(const fc::exception& e);
}}}
