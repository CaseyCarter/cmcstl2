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
#include <stl2/functional.hpp>
#include <algorithm>
#include <iterator>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

namespace stl2 = __stl2;

constexpr struct {
  constexpr bool operator()(stl2::Integral i) const {
    return i % 2 != 0;
  }
} is_odd{};

struct A {
  int i;
  constexpr bool is_odd() const { return ::is_odd(i); }
  constexpr operator int() const { return i; }
};

int main() {
  {
    int some_ints[] = {0,1,2,3,4,5,6,7};
    int result[sizeof(some_ints)/sizeof(some_ints[0])];
    auto e = std::copy_if(std::begin(some_ints), std::end(some_ints), result, stl2::not_fn(is_odd));
    ::check_equal(stl2::ext::make_range(result, e), {0,2,4,6});
  }

  {
    A some_As[] = {0,1,2,3,4,5,6,7};
    A result[sizeof(some_As)/sizeof(some_As[0])];
    auto e = std::copy_if(std::begin(some_As), std::end(some_As), result, stl2::not_fn(&A::is_odd));
    ::check_equal(stl2::ext::make_range(result, e), {0,2,4,6});
  }

  return ::test_result();
}
