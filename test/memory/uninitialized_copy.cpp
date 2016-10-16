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
#include <cassert>
#include <cstdint>
#include <experimental/ranges/memory>
#include <iostream>

#include "raii.hpp"

template <typename T>
void uninitialised_copy(const std::array<T, 64>& data)
{
   auto c = raii<T>{data.size()};
   control::uninitialized_copy(data.cbegin(), data.cend(), c.begin());

   auto i = raii<T>{data.size()};
   independent::uninitialized_copy(data.cbegin(), data.cend(), i.begin());

   assert(ranges::equal(
}