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
#include <array>
#include "Book.hpp"
#include <cassert>
#include <cstdint>
#include <stl2/algorithm.hpp>
#include <stl2/concepts.hpp>
#include <stl2/detail/memory/uninitialized_fill.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <typeinfo>

#include "raii.hpp"

#include <experimental/ranges/algorithm>
#include <iostream>
namespace ranges = __stl2;

constexpr auto test_size{1 << 10};

template <typename T>
void uninitialized_fill_test(const T& x)
{
   const auto independent = raii<T>{test_size};
   auto test = [&independent, &x](const auto& p){
      assert(ranges::find_if(independent, [&x](const auto& i){ return i != x; }) == independent.end());
      assert(ranges::find_if(independent.begin(), p, [&x](const auto& i){ return i != x; }) == p);
      ranges::destroy(independent);
   };

   test(ranges::uninitialized_fill(independent.begin(), independent.end(), x));
   test(ranges::uninitialized_fill(independent.cbegin(), independent.cend(), x));
   test(ranges::uninitialized_fill(independent, x));
   test(ranges::uninitialized_fill_n(independent.begin(), independent.size(), x));
   test(ranges::uninitialized_fill_n(independent.cbegin(), independent.size(), x));
}

int main()
{
   uninitialized_fill_test(0);
   uninitialized_fill_test(0.0);
   uninitialized_fill_test('a');
   uninitialized_fill_test(std::vector<int>{});
   uninitialized_fill_test(std::vector<int>(1 << 20, 0));
   uninitialized_fill_test(Book{});
}