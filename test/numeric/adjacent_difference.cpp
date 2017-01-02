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
#include "../detail/test_interface.hpp"
#include "../simple_test.hpp"
#include <deque>
#include <list>
#include <numeric>
#include <stl2/detail/concepts/container.hpp>
#include <stl2/detail/fwd.hpp>
#include <unordered_map>
#include <vector>

namespace ranges = __stl2;

int main()
{
   auto stdcall =
   [](ranges::InputIterator a, ranges::Sentinel<decltype(a)> b, ranges::WeaklyIncrementable c) {
      std::adjacent_difference(ranges::move(a), ranges::move(b), ranges::move(c));
   };
   auto stl2_call =
   [](ranges::InputIterator a, ranges::Sentinel<decltype(a)> b, ranges::WeaklyIncrementable c) {
      return ranges::adjacent_difference(ranges::move(a), ranges::move(b), ranges::move(c));
   };
   auto range_call =
   [](ranges::InputRange&& a, ranges::WeaklyIncrementable b) {
      return ranges::adjacent_difference(ranges::forward<decltype(a)>(a), ranges::move(b));
   };
   
   CHECK_empty<std::vector<int>>(stdcall, stl2_call, range_call);
   CHECK_empty<std::deque<double>>(stdcall, stl2_call, range_call);
   CHECK_empty<std::list<cmcstl2::Uint128>>(stdcall, stl2_call, range_call);

   CHECK_single_element<std::vector<int>>(stdcall, stl2_call, range_call);
   CHECK_single_element<std::deque<double>>(stdcall, stl2_call, range_call);
   CHECK_single_element<std::list<cmcstl2::Uint128>>(stdcall, stl2_call, range_call);

   CHECK_many<std::vector>(stdcall, stl2_call, range_call,
                           0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
   CHECK_many<std::deque>(stdcall, stl2_call, range_call,
                          0.0, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9);
   CHECK_many<std::list>(stdcall, stl2_call, range_call,
                         cmcstl2::Uint128{},
                         cmcstl2::Uint128{0xdeadbeef, 0xf00d5a1e},
                         cmcstl2::Uint128{0, 1},
                         cmcstl2::Uint128{0xfeedfeedfeedfeed, 0xbeefbeefbeefbeef});
   CHECK_projection();
   CHECK_insert_iterator();
   return test_result();
}

void CHECK_projection() noexcept
{
   const auto v = std::vector<int>{100, 200, 400, 600};
   auto s = std::vector<int>(v.size());
   std::adjacent_difference(v.begin(), v.end(), s.begin());

   auto u = std::vector<std::pair<int, int>>{};
   for (const auto& i : v)
      u.push_back({i, 0});

   auto r = std::vector<int>(u.size());
   auto result = ranges::adjacent_difference(u,
                                             r.begin(),
                                             ranges::minus<>{},
                                             [](const auto i) {
                                                return i.first;
                                             });
   CHECK(r == s);
   CHECK(result.in() == u.end());
   CHECK(result.out() == r.end());
}

void CHECK_insert_iterator() noexcept
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
