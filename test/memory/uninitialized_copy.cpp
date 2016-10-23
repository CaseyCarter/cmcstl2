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
#include <iostream>
#include <stl2/algorithm.hpp>
#include <stl2/concepts.hpp>
#include <uninitialized_copy.hpp>

#include "raii.hpp"

#include <experimental/ranges/algorithm>

template <typename T>
void uninitialised_copy(const std::array<T, 8>& data)
{
   auto c = raii<T>{data.size()};
   control::uninitialized_copy(data.cbegin(), data.cend(), c.begin());

   auto i = raii<T>{data.size()};
   independent::uninitialized_copy(data.cbegin(), data.cend(), i.begin());

   assert(ranges::equal(c, i, std::equal_to<T>{}));

   independent::destroy(i.begin(), i.end());

   independent::uninitialized_copy(data, i.begin());
   assert(ranges::equal(c, i, std::equal_to<T>{}));

   //independent::destroy(c); // since std::[experimental::]destroy doesn't exist in gcc 6.2
   //independent::destroy(i);
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
void thorough_test()
{
   uninitialised_copy(std::array<int, 8>{});
//   uninitialised_copy(std::array<std::vector<double>, 8>{});
//   uninitialised_copy(std::array<Book, 8>{});
}

int main()
{
   thorough_test();
}
