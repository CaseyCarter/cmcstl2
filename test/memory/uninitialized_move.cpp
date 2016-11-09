// cmcstl2 - A concept-enabled C++ standard library
//
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
#include <stl2/detail/memory/uninitialized_move.hpp>
#include <array>
#include "Book.hpp"
#include <cassert>
#include <cstdint>
#include <stl2/algorithm.hpp>
#include <stl2/concepts.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include "raw_buffer.hpp"

namespace ranges = __stl2;

template <typename T>
void assert_empty(const Array<T>& a)
{
   for (const auto& i : a)
      assert(i.empty());
}

template <typename T>
requires ranges::is_fundamental<T>::value
void assert_empty(const Array<T>&)
{
}

template <ranges::Copyable T>
void uninitialized_move_test(const Array<T>& control)
{
   auto independent = make_buffer<T>(control.size());
   auto to_move = control;
   auto test = [&control, &to_move, &independent](const auto& p) {
      assert_empty(to_move);
      assert(p.in() == to_move.end());

      assert(ranges::equal(control, independent, std::equal_to<T>{}));
      assert(independent.end() == p.out());
      ranges::destroy(independent);
   };

   test(ranges::uninitialized_move(to_move.begin(), to_move.end(), independent.begin()));

   to_move = control;      // note to_move.begin(), not to_move.cbegin()
   test(ranges::uninitialized_move(to_move.begin(), to_move.end(), independent.cbegin()));

   to_move = control;
   test(ranges::uninitialized_move(to_move, independent.begin()));

   to_move = control;
   test(ranges::uninitialized_move(to_move, independent.cbegin()));

   to_move = control;
   test(ranges::uninitialized_move_n(to_move.begin(), to_move.size(), independent.begin()));

   to_move = control;        // note to_move.begin(), not to_move.cbegin()
   test(ranges::uninitialized_move_n(to_move.begin(), to_move.size(), independent.cbegin()));
}

using Move_only_t = Array<std::unique_ptr<std::string>>;
void uninitialized_move_test(Move_only_t first, const ranges::value_type_t<Move_only_t>& t)
{
   auto test = [&t](const auto& s, const auto& d, const auto& p) {
      assert(static_cast<decltype(s.size())>(ranges::count_if(s.begin(), s.end(), [](const auto& i){ return !i; })) == s.size());
      assert(ranges::count_if(d.begin(), d.end(), [&t](const auto& i){ return !i; }) == 0);
      assert(p.in() == s.end());
      assert(p.out() == d.end());
   };

   auto second = make_buffer<Move_only_t::value_type>(first.size());
   test(first, second, ranges::uninitialized_move(first.begin(), first.end(), second.begin()));
   test(second, first, ranges::uninitialized_move(second.begin(), second.end(), first.cbegin()));
   test(first, second, ranges::uninitialized_move(first, second.begin()));
   test(second, first, ranges::uninitialized_move(second, first.cbegin()));
   test(first, second, ranges::uninitialized_move_n(first.begin(), first.size(), second.cbegin()));
   test(second, first, ranges::uninitialized_move_n(second.begin(), second.size(), first.begin()));
}

/**
 *    Testing framework:
 *       - test an array of fundamentals
 *       - test an array of standard containers
 *       - test an array of non-standard structures
 *
 *       - initial array: using the default constructor
 *       - second array:  using a non-default constructor
 */
int main()
{
   using Test_type_one = Array<int>;
   using Test_type_two = Array<std::vector<double>>;
   uninitialized_move_test(Test_type_one{});
   uninitialized_move_test(Test_type_two{});
   uninitialized_move_test(Array<Book>{});

   uninitialized_move_test(Test_type_one{0, 1, 2, 3, 4, 5, 6, 7});
   uninitialized_move_test(Test_type_two{{{0.0, 0.1, 0.2},
                                          {1.0, 1.1, 1.2, 1.3, 1.4},
                                          {2.0, 2.1, 2.2, 2.3},
                                          {3.01, 3.20, 3.33, 3.4},
                                          {4.101, 4.102, 4.201, 4.202, 4.311},
                                          {5.},
                                          {6.1, 3.02, 6.3, 6.4, 6.5, 6.6, 6.7, 6.8, 6.9},
                                          std::vector<double>(1 << 26, 7.0)}});

   uninitialized_move_test(Move_only_t{std::make_unique<std::string>("0"),
                                       std::make_unique<std::string>("0"),
                                       std::make_unique<std::string>("0"),
                                       std::make_unique<std::string>("0"),
                                       std::make_unique<std::string>("0"),
                                       std::make_unique<std::string>("0"),
                                       std::make_unique<std::string>("0"),
                                       std::make_unique<std::string>("0")},
                           std::make_unique<std::string>("0"));
}
