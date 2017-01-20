// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2013 -- 2017 (range-v3/include/numeric/iota.hpp)
//  Copyright Casey Carter 2015 -- 2017
//  Copyright Christopher Di Bella 2016 -- 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/numeric/iota.hpp>
#include <forward_list>
#include <list>
#include <stl2/detail/algorithm/equal.hpp>
#include "../simple_test.hpp"
#include <vector>

namespace ranges = __stl2;

template <typename T>
void test() noexcept
{
   const T a{5, 6, 7, 8, 9};
   {
      T b{1, 2, 3, 4, 5};
      ranges::iota(ranges::begin(b), ranges::end(b), 5);
      CHECK(ranges::equal(a, b));
   }
   {
      T rng{1, 2, 3, 4, 5};
      ranges::iota(rng, 5);
      CHECK(ranges::equal(a, rng));
   }
}

int main()
{
   test<std::forward_list<int>>();
   test<std::list<int>>();
   test<std::vector<int>>();
   test<int[]>();

   return ::test_result();
}
