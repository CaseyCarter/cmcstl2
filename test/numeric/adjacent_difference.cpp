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
#include <stl2/detail/numeric/adjacent_difference.hpp>
#include "../detail/int128.hpp"
#include "../simple_test.hpp"
#include <deque>
#include <list>
#include <numeric>
#include <stl2/detail/fwd.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace ranges = __stl2;

template <typename T>
void CHECK_algorithm(const T& v)
{
   auto s = T(v.size());
   std::adjacent_difference(v.begin(), v.end(), s.begin());

   auto r = T(v.size());
   auto result = ranges::adjacent_difference(v.begin(), v.end(), r.begin());
   CHECK(s == r);
   CHECK(result.in() == v.end());
   CHECK(result.out() == r.end());

   r = T(v.size());
   result = ranges::adjacent_difference(v, r.begin());
   CHECK(s == r);
   CHECK(result.in() == v.end());
   CHECK(result.out() == r.end());
}

void CHECK_irregular()
{
   const auto v = std::vector<int>{100, 200, 300, 400, 5600};
   auto s = std::vector<int>{};
   std::adjacent_difference(v.begin(), v.end(), std::back_inserter(s));

   auto r = std::vector<int>{};
   ranges::adjacent_difference(v.begin(), v.end(), ranges::back_inserter(r));
   CHECK(r == s);

   r.clear();
   CHECK(r != s);
   ranges::adjacent_difference(v, ranges::back_inserter(r));
   CHECK(r == s);
}

template <typename Proj>
void CHECK_projection(const std::unordered_map<int, int>& h, Proj proj)
{
   // TODO identify a suitable data structure and projection to test
}

int main()
{
   CHECK_algorithm(std::vector<int>{});
   CHECK_algorithm(std::deque<double>{});
   CHECK_algorithm(std::list<cmcstl2::Uint128>{});

   CHECK_algorithm(std::vector<int>{1});
   CHECK_algorithm(std::deque<double>{1.0});
   CHECK_algorithm(std::list<cmcstl2::Uint128>{{0xdeadbeef, 0xf00d5a1e}});

   CHECK_algorithm(std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
   CHECK_algorithm(std::vector<double>{0, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9});
   CHECK_algorithm(std::list<cmcstl2::Uint128>{{}, {0xdeadbeef, 0xf00d5a1e}, {0, 1},
                                                   {0xfeedfeedfeedfeed, 0xbeefbeefbeefbeef}});
   CHECK_irregular();
   return test_result();
}