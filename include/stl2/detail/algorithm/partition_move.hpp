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
#ifndef STL2_DETAIL_ALGORITHM_PARTITION_MOVE_HPP
#define STL2_DETAIL_ALGORITHM_PARTITION_MOVE_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/tuple.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// partition_copy [alg.partitions]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O1,
            WeaklyIncrementable O2, class Proj = identity,
            IndirectCallablePredicate<Projected<I, Proj>> Pred>
    requires IndirectlyMovable<I, O1>() && IndirectlyMovable<I, O2>()
  tagged_tuple<tag::in(I), tag::out1(O1), tag::out2(O2)>
  partition_move(I first, S last, O1 out_true, O2 out_false, Pred pred_,
                 Proj proj_ = Proj{}) {
    auto&& pred = __stl2::as_function(pred_);
    auto&& proj = __stl2::as_function(proj_);

    for (; first != last; ++first) {
      if (pred(proj(*first))) {
        *out_true = __stl2::iter_move(first);
        ++out_true;
      } else {
        *out_false = __stl2::iter_move(first);
        ++out_false;
      }
    }
    return {__stl2::move(first), __stl2::move(out_true), __stl2::move(out_false)};
  }

  template <InputRange Rng, WeaklyIncrementable O1, WeaklyIncrementable O2,
            class Proj = identity,
            IndirectCallablePredicate<Projected<IteratorType<Rng>, Proj>> Pred>
    requires IndirectlyMovable<IteratorType<Rng>, O1>() &&
      IndirectlyMovable<IteratorType<Rng>, O2>()
  tagged_tuple<tag::in(safe_iterator_t<Rng>), tag::out1(O1), tag::out2(O2)>
  partition_move(Rng&& rng, O1 out_true, O2 out_false,
                 Pred&& pred, Proj&& proj = Proj{}) {
    return __stl2::partition_move(
      __stl2::begin(rng), __stl2::end(rng),
      __stl2::move(out_true), __stl2::move(out_false),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
