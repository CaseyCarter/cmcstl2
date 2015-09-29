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
#ifndef STL2_DETAIL_ALGORITHM_COUNT_IF_HPP
#define STL2_DETAIL_ALGORITHM_COUNT_IF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// count_if [alg.count]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, class Proj = identity,
            IndirectCallablePredicate<Projected<I, Proj>> Pred>
  DifferenceType<I> count_if(I first, S last, Pred pred_, Proj proj_ = Proj{}) {
    auto&& proj = __stl2::as_function(proj_);
    auto&& pred = __stl2::as_function(pred_);
    auto n = DifferenceType<I>(0);
    for (; first != last; ++first) {
      if (pred(proj(*first))) {
        ++n;
      }
    }
    return n;
  }

  template <InputRange Rng, class Proj = identity,
            IndirectCallablePredicate<Projected<IteratorType<Rng>, Proj>> Pred>
  DifferenceType<Rng> count_if(Rng&& rng, Pred&& pred, Proj&& proj = Proj{}) {
    return __stl2::count_if(__stl2::begin(rng), __stl2::end(rng),
                            __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
