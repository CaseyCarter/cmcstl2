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
#ifndef STL2_DETAIL_ALGORITHM_GENERATE_N_HPP
#define STL2_DETAIL_ALGORITHM_GENERATE_N_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/function.hpp>

///////////////////////////////////////////////////////////////////////////
// generate_n [alg.generate]
//
STL2_OPEN_NAMESPACE {
  template <Function F, WeakOutputIterator<result_of_t<F&()>> O>
  O generate_n(O first, difference_type_t<O> n, F gen)
  {
    for (; n > 0; ++first, --n) {
      *first = gen();
    }
    return first;
  }
} STL2_CLOSE_NAMESPACE

#endif
