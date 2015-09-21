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
#ifndef STL2_DETAIL_ALGORITHM_UNIQUE_HPP
#define STL2_DETAIL_ALGORITHM_UNIQUE_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/adjacent_find.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// unique [alg.unique]
//
STL2_OPEN_NAMESPACE {
  template <ForwardIterator I, Sentinel<I> S, class Proj = identity,
            IndirectCallableRelation<Projected<I, Proj>> R = equal_to<>>
    requires Permutable<I>()
  I unique(I first, S last, R comp_ = R{}, Proj proj_ = Proj{}) {
    auto&& comp = __stl2::as_function(comp_);
    auto&& proj = __stl2::as_function(proj_);
    first = __stl2::adjacent_find(__stl2::move(first), last, comp, proj);
    if (first != last) {
      for (auto m = __stl2::next(first, 2); m != last; ++m) {
        if (!comp(proj(*first), proj(*m))) {
          ++first;
          *first = __stl2::iter_move(m);
        }
      }
      ++first;
    }
    return first;
  }

  template<ForwardRange Rng, class Proj = identity,
           IndirectCallableRelation<Projected<IteratorType<Rng>, Proj>> R = equal_to<>>
    requires Permutable<IteratorType<Rng>>()
  safe_iterator_t<Rng> unique(Rng&& rng, R&& comp = R{}, Proj&& proj = Proj{}) {
    return __stl2::unique(__stl2::begin(rng), __stl2::end(rng),
      __stl2::forward<R>(comp), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
