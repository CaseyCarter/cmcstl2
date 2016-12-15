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
#include <stl2/detail/numeric/accumulate.hpp>
#include "../detail/int128.hpp"
#include "../simple_test.hpp"
#include <deque>
#include <list>
#include <numeric>
#include <stl2/detail/concepts/number.hpp>
#include <string>
#include <vector>

namespace ranges = __stl2;

template <typename T>
void CHECK_empty()
{
   const auto v = std::vector<int>{};
   CHECK(ranges::accumulate(v) == std::accumulate(v.begin(), v.end(), 0));
}

template <typename T>
void CHECK_one()
{
   const auto v = std::deque<T>(1);
   CHECK(ranges::accumulate(v) == std::accumulate(v.begin(), v.end(), T{}));
}

template <typename T, typename U = T>
   requires ranges::models::RegularStrictNumber<U, T>
void CHECK_many(const U i = U{})
{
   auto v = std::vector<T>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   auto a = std::accumulate(v.begin(), v.end(), i);

   CHECK(ranges::accumulate(v.begin(), v.end(), i) == a);
   CHECK(ranges::accumulate(v, i) == a);

   auto foo = [](const auto a, const auto b) { return a < b ? a - b : a + b; };
   a = std::accumulate(v.begin(), v.end(), i, foo);
   CHECK(ranges::accumulate(v.begin(), v.end(), i, foo) == a);
   CHECK(ranges::accumulate(v, i, foo) == a);
}

void CHECK_many(const std::string& i)
{
   using namespace std::string_literals;
   auto l = std::list<std::string>{"hello"s, "how are you"s, "foo"s, "bar"s, "this"s, "that"s,
                                   "tit"s, "for"s, "tat"s};
   CHECK(ranges::accumulate(l) == std::accumulate(l.cbegin(), l.cend(), i));
}

int main()
{
   CHECK_empty<int>();
   CHECK_one<int>();
   CHECK_many<int>();
   CHECK_many<int, double>();

   CHECK_empty<double>();
   CHECK_one<double>();
   CHECK_many<double>();
   CHECK_many<double, int>();

   using namespace std::string_literals;
   CHECK_empty<std::string>();
   CHECK_one<std::string>();
   CHECK_many(""s);

   CHECK_empty<cmcstl2::Uint128>();
   CHECK_one<cmcstl2::Uint128>();
   CHECK_many<cmcstl2::Uint128>(cmcstl2::Uint128{0xdeadbeef, 0xf00d4b0b});
   CHECK_many<std::uint64_t, cmcstl2::Uint128>();

   return test_result();
}