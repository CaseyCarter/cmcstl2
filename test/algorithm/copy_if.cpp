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
#include <stl2/detail/algorithm/copy_if.hpp>
#include <algorithm>
#include "../simple_test.hpp"

namespace stl2 = __stl2;

int main() {
  static const int source[] = {5,4,3,2,1,0};
  static constexpr std::ptrdiff_t n = sizeof(source)/sizeof(source[0]);

  static const int evens[] = {4,2,0};
  static_assert(sizeof(evens) / sizeof(evens[0]) == n / 2);
  auto is_even = [](int i){
    return i % 2 == 0;
  };


  {
    int target[n];
    std::fill_n(target, n, -1);

    static const int evens[] = {4,2,0};
    auto is_even = [](int i){
      return i % 2 == 0;
    };

    auto res = stl2::copy_if(source, source + n, target, is_even);
    CHECK(res.in() == source + n);
    CHECK(res.out() == target + n / 2);

    CHECK(std::equal(target, target + n / 2, evens));
    CHECK(std::count(target + n / 2, target + n, -1) == n / 2);
  }

  {
    int target[n];
    std::fill_n(target, n, -1);

    auto res = stl2::copy_if(source, target, is_even);
    CHECK(res.in() == source + n);
    CHECK(res.out() == target + n / 2);

    CHECK(std::equal(target, target + n / 2, evens));
    CHECK(std::count(target + n / 2, target + n, -1) == n / 2);
  }

  {
    int target[n];
    std::fill_n(target, n, -1);

    auto res = stl2::copy_if(std::move(source), target, is_even);
    CHECK(res.in().get_unsafe() == source + n);
    CHECK(res.out() == target + n / 2);

    CHECK(std::equal(target, target + n / 2, evens));
    CHECK(std::count(target + n / 2, target + n, -1) == n / 2);
  }

  {
    struct S { int value; };
    S source[n];
    for (auto i = n; i-- > 0;) {
      source[i].value = i;
    }
    S target[n];
    for (auto i = n; i-- > 0;) {
      target[i].value = -1;
    }

    auto res = stl2::copy_if(source, target, is_even, &S::value);
    CHECK(res.in() == source + n);
    CHECK(res.out() == target + n / 2);

    for (auto i = n / 2; i-- > 0;) {
      CHECK(target[i].value == source[2 * i].value);
    }

    CHECK(std::count_if(target + n / 2, target + n, [](const S& s){
          return s.value == -1; }) == n / 2);
  }

  return test_result();
}
