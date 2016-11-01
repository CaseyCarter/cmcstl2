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
#include <array>
#include "Book.hpp"
#include <cassert>
#include <cstdint>
#include <stl2/algorithm.hpp>
#include <stl2/concepts.hpp>
#include <stl2/detail/memory/uninitialized_move.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <typeinfo>

#include "raii.hpp"

#include <experimental/ranges/algorithm>
#include <iostream>
namespace ranges = __stl2;

template <typename T>
using Array = std::array<T, 8>;

template <typename T, bool Fundamental = ranges::is_fundamental<T>::value>
struct Assert {
   static void empty(const Array<T>& a)
   {
      for(const auto& i : a)
         assert(i.empty());
   }
};

template <typename T>
struct Assert<T, true>
{
   static void empty(const Array<T>&)
   {
   }
};

template <typename T>
void uninitialized_move_test(const Array<T>& control)
{
   const auto independent = raii<T>{static_cast<std::ptrdiff_t>(control.size())};
   auto to_move = control;
   auto test = [&control, &to_move, &independent](const auto& p){
      Assert<T>::empty(to_move);
      assert(p.in() == to_move.end());

      assert(ranges::equal(control, independent, std::equal_to<T>{}));
      assert(ranges::equal(control.begin(), control.end(), independent.begin(), p.out(), std::equal_to<T>{}));
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
}
