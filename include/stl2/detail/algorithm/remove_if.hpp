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
#ifndef STL2_DETAIL_ALGORITHM_REMOVE_HPP
#define STL2_DETAIL_ALGORITHM_REMOVE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// remove [alg.remove]
//
STL2_OPEN_NAMESPACE {
  template <ForwardIterator I, Sentinel<I> S, class Proj = identity,
            IndirectCallablePredicate<Projected<I, Proj>> Pred>
    requires Permutable<I>()
  I remove_if(I first, S last, Pred&& pred_, Proj&& proj_ = Proj{}) {
    auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
    first = __stl2::find_if(__stl2::move(first), last, pred, proj);
    if (first != last) {
      for (auto m = __stl2::next(first); m != last; ++m) {
        if (!pred(proj(*m))) {
          *first = __stl2::iter_move(m);
          ++first;
        }
      }
    }
    return first;
  }

  template <ForwardRange Rng, class Proj = identity,
            IndirectCallablePredicate<Projected<IteratorType<Rng>, Proj>> Pred>
    requires Permutable<IteratorType<Rng>>()
  safe_iterator_t<Rng>
  remove_if(Rng&& rng, Pred&& pred, Proj&& proj = Proj{}) {
    return __stl2::remove_if(
      __stl2::begin(rng), __stl2::end(rng),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
