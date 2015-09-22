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
#ifndef STL2_DETAIL_ALGORITHM_STABLE_PARTITION_HPP
#define STL2_DETAIL_ALGORITHM_STABLE_PARTITION_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// stable_partition [alg.partitions]
//
STL2_OPEN_NAMESPACE {
  template <BidirectionalIterator I, Sentinel<I> S, class Proj = identity,
            IndirectCallablePredicate<Projected<I, Proj>> Pred>
    requires Permutable<I>()
  I stable_partition(I first, S last, Pred pred_, Proj proj_ = Proj{}) {
    std::terminate(); // FIXME: NYI
  }

  template <BidirectionalRange Rng, class Proj = identity,
            IndirectCallablePredicate<Projected<IteratorType<Rng>, Proj>> Pred>
    requires Permutable<IteratorType<Rng>>()
  safe_iterator_t<Rng>
  stable_partition(Rng&& rng, Pred&& pred, Proj&& proj = Proj{}) {
    return __stl2::stable_partition(__stl2::begin(rng), __stl2::end(rng),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
