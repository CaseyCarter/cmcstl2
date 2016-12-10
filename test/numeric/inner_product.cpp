// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2016
//  Copyright Casey Carter 2016
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/numeric/inner_product.hpp>
#include "../detail/int128.hpp"
#include "../simple_test.hpp"
#include <deque>
#include <list>
#include <numeric>
#include <stl2/detail/concepts/number.hpp>
#include <string>
#include <vector>

namespace ranges = __stl2;

template <typename U, typename V>
void CHECK_algorithm(const U& u, const V& v)
{
   auto s = std::inner_product(u.begin(), u.end(), v.begin(), 0);

   CHECK(s == ranges::inner_product(u.begin(), u.end(), v.begin()));
   CHECK(s == ranges::inner_product(u, v.begin()));
}

int main()
{
   CHECK_algorithm(std::vector<int>{}, std::vector<int>{});
   CHECK_algorithm(std::deque<int>{}, std::vector<double>{});
   CHECK_algorithm(std::vector<double>{}, std::list<int>{});

   CHECK_algorithm(std::vector<int>{}, std::vector<int>{1});
   CHECK_algorithm(std::vector<int>{}, std::vector<double>{1});
   CHECK_algorithm(std::vector<double>{}, std::vector<int>{1});

   CHECK_algorithm(std::vector<int>{1, 2, 3}, std::vector<int>{});

   CHECK_algorithm(std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                   std::vector<int>{10, 20, 30, 40, 50, 60, 70, 80, 90});

   return test_result();
}