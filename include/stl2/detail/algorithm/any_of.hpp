// Adapted (stolen) from Range v3 library
//
//  Copyright Andrew Sutton 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_ANY_OF_HPP
#define STL2_DETAIL_ALGORITHM_ANY_OF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// any_of [alg.any_of]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, class F, class P = identity>
    requires IndirectCallablePredicate<F, Projected<I, P>>()
  bool any_of(I first, S last, F pred, P proj = P{}) {
    auto &&ipred = __stl2::as_function(pred);
    auto &&iproj = __stl2::as_function(proj);
    for (; first != last; ++first) {
      if (ipred(iproj(*first))) {
        return true;
      }
    }
    return false;
  }

  template <InputRange R, class F, class P = identity>
    requires IndirectCallablePredicate<F, Projected<R, P>>()
  bool any_of(R&& rng, F&& pred, P&& proj = P{}) {
    return __stl2::any_of(__stl2::begin(rng), __stl2::end(rng),
                          __stl2::forward<F>(pred), __stl2::forward<P>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
