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
#ifndef STL2_DETAIL_ALGORITHM_COPY_N_HPP
#define STL2_DETAIL_ALGORITHM_COPY_N_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// copy_n [alg.copy]
//
STL2_OPEN_NAMESPACE {
  template <WeakInputIterator I, WeaklyIncrementable O>
    requires IndirectlyCopyable<I, O>()
  tagged_pair<tag::in(I), tag::out(O)>
  copy_n(I first, DifferenceType<I> n, O result) {
    for(; n > 0; ++first, ++result, --n) {
      *result = *first;
    }
    return {first, result};
  }
} STL2_CLOSE_NAMESPACE

#endif
