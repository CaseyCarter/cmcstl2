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
#include "Book.hpp"
#include <cassert>
#include <cstdint>
#include <deque>
#include <iostream>
#include <list>
#include <stl2/algorithm.hpp>
#include <stl2/concepts.hpp>
#include <stl2/detail/memory/uninitialized_value_construct.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <string>
#include <typeinfo>

#include "raii.hpp"

#include <experimental/ranges/algorithm>
#include <iostream>
namespace ranges = __stl2;

template <typename T>
void uninitialized_value_construct_test()
{
   auto independent = raii<T>{1 << 20};
   auto test = [&independent](const auto& p) {
      auto t = T();
      for (const auto& i : independent)
         assert(i == t);
      for (auto i = independent.begin(); i != p; ++i)
         assert(*i == t);
      ranges::destroy(independent);
   };

   test(ranges::uninitialized_value_construct(independent.begin(), independent.end()));
   test(ranges::uninitialized_value_construct(independent.cbegin(), independent.cend()));
   test(ranges::uninitialized_value_construct(independent));
   test(ranges::uninitialized_value_construct_n(independent.begin(), independent.size()));
   test(ranges::uninitialized_value_construct_n(independent.cbegin(), independent.size()));
}

int main()
{
   using namespace std;
   uninitialized_value_construct_test<char>();
   uninitialized_value_construct_test<short>();
   uninitialized_value_construct_test<int>();
   uninitialized_value_construct_test<float>();
   uninitialized_value_construct_test<long>();
   uninitialized_value_construct_test<double>();
   uninitialized_value_construct_test<long long>();
   uninitialized_value_construct_test<vector<char>>();
   uninitialized_value_construct_test<string>();
   uninitialized_value_construct_test<deque<double>>();
   uninitialized_value_construct_test<list<vector<deque<double>>>>();
   uninitialized_value_construct_test<unique_ptr<string>>();
}