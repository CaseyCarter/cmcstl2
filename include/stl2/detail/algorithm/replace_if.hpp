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
#ifndef STL2_DETAIL_ALGORITHM_REPLACE_IF_HPP
#define STL2_DETAIL_ALGORITHM_REPLACE_IF_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// replace_if [alg.replace]
//
STL2_OPEN_NAMESPACE {
  template<ForwardIterator I, Sentinel<I> S, Semiregular T, class Proj = identity,
           IndirectCallablePredicate<Projected<I, Proj>> Pred>
    requires Writable<I, T>()
  I replace_if(I first, S last, Pred pred_, const T& new_value, Proj proj_ = Proj{}) {
    auto&& pred = __stl2::as_function(pred_);
    auto&& proj = __stl2::as_function(proj_);
    for (; first != last; ++first) {
      if (pred(proj(*first))) {
        *first = new_value;
      }
    }
    return first;
  }

  template<ForwardRange Rng, Semiregular T, class Proj = identity,
           IndirectCallablePredicate<Projected<IteratorType<Rng>, Proj>> Pred>
    requires Writable<IteratorType<Rng>, T>()
  safe_iterator_t<Rng>
  replace_if(Rng&& rng, Pred&& pred, const T& new_value, Proj&& proj = Proj{}) {
    return __stl2::replace_if(__stl2::begin(rng), __stl2::end(rng),
      __stl2::forward<Pred>(pred), new_value, __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
