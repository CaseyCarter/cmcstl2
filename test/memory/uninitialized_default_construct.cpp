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
#include <stl2/detail/memory/uninitialized_default_construct.hpp>
#include "Book.hpp"
#include <cassert>
#include <cstdint>
#include <deque>
#include <iostream>
#include <list>
#include <stl2/algorithm.hpp>
#include <stl2/concepts.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <string>
#include "raw_buffer.hpp"

namespace ranges = __stl2;

template <typename T>
void test(const raw_buffer<T>& independent, const auto& p)
{
      T t;
      assert(ranges::find_if(independent, [&t](const auto& i){ return i != t; }) == independent.cend());
      assert(p == independent.end());
      ranges::destroy(independent);
}

template <typename T>
void uninitialized_default_construct_test()
{
   auto independent = make_buffer<T>(1 << 20);

   test(independent, ranges::uninitialized_default_construct(independent.begin(), independent.end()));
   test(independent, ranges::uninitialized_default_construct(independent.cbegin(), independent.cend()));
   test(independent, ranges::uninitialized_default_construct(independent));
   test(independent, ranges::uninitialized_default_construct_n(independent.begin(), independent.size()));
   test(independent, ranges::uninitialized_default_construct_n(independent.cbegin(), independent.size()));
}

template <typename T>
requires ranges::is_fundamental<T>::value
void uninitialized_default_construct_test()
{
   std::cerr << "Warning, this test may fail, as default construction does not initialise "
                "fundamental objects.\n";
   auto independent = make_buffer<T>(1 << 20);

   test(independent, ranges::uninitialized_default_construct(independent.begin(), independent.end()));
   test(independent, ranges::uninitialized_default_construct(independent));
   test(independent, ranges::uninitialized_default_construct_n(independent.begin(), independent.size()));
}

int main()
{
   uninitialized_default_construct_test<char>();
   uninitialized_default_construct_test<short>();
   //uninitialized_default_construct_test<int>();
   //uninitialized_default_construct_test<float>();
   //uninitialized_default_construct_test<long>();
   //uninitialized_default_construct_test<double>();
   //uninitialized_default_construct_test<long long>();
   uninitialized_default_construct_test<std::vector<char>>();
   uninitialized_default_construct_test<std::string>();
   uninitialized_default_construct_test<std::deque<double>>();
   uninitialized_default_construct_test<std::list<std::vector<std::deque<double>>>>();
   uninitialized_default_construct_test<std::unique_ptr<std::string>>();
}