// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/algorithm/all_of.hpp>

#include "../simple_test.hpp"

int main() {
  static const int even_ints[] = {8, 12, 0, 14};
  CHECK(__stl2::all_of(even_ints, [](auto&& i){ return i % 2 == 0; }));
  return ::test_result();
}
