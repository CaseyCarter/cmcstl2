// cmcstl2 - A concept-enabled C++ standard library
//
// Algorithm adapted (stolen) from Range v3 library
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
#ifndef STL2_DETAIL_ALGORITHM_ALL_OF_HPP
#define STL2_DETAIL_ALGORITHM_ALL_OF_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// all_of [alg.all_of]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, class Proj = identity,
            IndirectCallablePredicate<Projected<I, Proj>> Pred>
  bool all_of(I first, S last, Pred pred_, Proj proj_ = Proj{}) {
    auto &&pred = __stl2::as_function(pred_);
    auto &&proj = __stl2::as_function(proj_);
    for (; first != last; ++first) {
      if (!pred(proj(*first))) {
        return false;
      }
    }
    return true;
  }

  template <InputRange R, class Proj = identity,
            IndirectCallablePredicate<Projected<IteratorType<R>, Proj>> Pred>
  bool all_of(R&& rng, Pred&& pred, Proj&& proj = Proj{}) {
    return __stl2::all_of(__stl2::begin(rng), __stl2::end(rng),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }

  // Extension
  template <class E, class Proj = identity,
            IndirectCallablePredicate<Projected<const E*, Proj>> Pred>
  bool all_of(std::initializer_list<E> il, Pred&& pred, Proj&& proj = Proj{}) {
    return __stl2::all_of(il.begin(), il.end(),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
