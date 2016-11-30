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
#include <stl2/detail/memory/uninitialized_copy.hpp>
#include <stl2/detail/algorithm/equal.hpp>
#include <stl2/concepts.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <cstdint>
#include <vector>
#include "../simple_test.hpp"
#include "Book.hpp"
#include "raw_buffer.hpp"

namespace ranges = __stl2;

template <typename T>
requires
   ranges::CopyConstructible<T>() &&
   ranges::EqualityComparable<T>()
void uninitialized_copy_test(const Array<T>& control)
{
   auto independent = make_buffer<T>(control.size());
   auto test = [&control, &independent](const auto p) {
      CHECK(p.in() == control.end());
      CHECK(p.out() == independent.end());
      CHECK(ranges::equal(control.begin(), p.in(), independent.begin(), p.out()));
      ranges::destroy(independent.begin(), p.out());
   };

   test(ranges::uninitialized_copy(control.begin(), control.end(), independent.begin()));
   test(ranges::uninitialized_copy(control.cbegin(), control.cend(), independent.cbegin()));
   test(ranges::uninitialized_copy(control, independent.begin()));
   test(ranges::uninitialized_copy(control, independent.cbegin()));
   test(ranges::uninitialized_copy_n(control.begin(), control.size(), independent.begin()));
   test(ranges::uninitialized_copy_n(control.cbegin(), control.size(), independent.cbegin()));
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

   uninitialized_copy_test(Test_type_one{});
   uninitialized_copy_test(Test_type_two{});
   uninitialized_copy_test(Array<Book>{});

   uninitialized_copy_test(Test_type_one{0, 1, 2, 3, 4, 5, 6, 7});
   uninitialized_copy_test(Test_type_two{{{0.0, 0.1, 0.2},
                                          {1.0, 1.1, 1.2, 1.3, 1.4},
                                          {2.0, 2.1, 2.2, 2.3},
                                          {3.01, 3.20, 3.33, 3.4},
                                          {4.101, 4.102, 4.201, 4.202, 4.311},
                                          {5.},
                                          {6.1, 3.02, 6.3, 6.4, 6.5, 6.6, 6.7, 6.8, 6.9},
                                          std::vector<double>(1 << 12, 7.0)}});

   return ::test_result();
}
