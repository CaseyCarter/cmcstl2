// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
#ifndef STL2_DETAIL_ALGORITHM_PARTITION_POINT_HPP
#define STL2_DETAIL_ALGORITHM_PARTITION_POINT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// partition_point [alg.partitions]
//
STL2_OPEN_NAMESPACE {
  namespace ext {
    template <ForwardIterator I, class Proj = identity,
              IndirectCallablePredicate<projected<I, Proj>> Pred>
    I partition_point_n(I first, difference_type_t<I> n,
                        Pred&& pred_, Proj&& proj_ = Proj{}) {
      auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
      auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));

      STL2_ASSUME(0 <= n);
      while (n != 0) {
        auto const half = n / 2;
        auto middle = __stl2::next(__stl2::ext::uncounted(first), half);
        if (pred(proj(*middle))) {
          first = __stl2::ext::recounted(first, __stl2::move(++middle), half + 1);
          n -= half + 1;
        } else {
          n = half;
        }
      }
      return first;
    }
  }

  template <ForwardIterator I, Sentinel<I> S, class Proj = identity,
            IndirectCallablePredicate<projected<I, Proj>> Pred>
  I partition_point(I first, S last, Pred&& pred_, Proj&& proj_ = Proj{}) {
    auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));

    // Probe exponentially for either end-of-range or an iterator
    // that is past the partition point (i.e., does not satisfy pred).
    auto n = difference_type_t<I>{1};
    while (true) {
      auto m = first;
      auto d = __stl2::advance(m, n, last);
      if (m == last || !pred(proj(*m))) {
        n -= d;
        return __stl2::ext::partition_point_n(__stl2::move(first), n,
                                              __stl2::move(pred),
                                              __stl2::move(proj));
      }
      first = __stl2::move(m);
      n *= 2;
    }
  }

  template <ForwardIterator I, Sentinel<I> S, class Proj = identity,
            IndirectCallablePredicate<projected<I, Proj>> Pred>
    requires SizedIteratorRange<I, S>()
  I partition_point(I first, S last, Pred&& pred, Proj&& proj = Proj{}) {
    auto n = __stl2::distance(first, last);
    return __stl2::ext::partition_point_n(__stl2::move(first), n,
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }

  template <ForwardRange Rng, class Proj = identity,
            IndirectCallablePredicate<projected<iterator_t<Rng>, Proj>> Pred>
  safe_iterator_t<Rng>
  partition_point(Rng&& rng, Pred&& pred, Proj&& proj = Proj{}) {
    return __stl2::partition_point(__stl2::begin(rng), __stl2::end(rng),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }

  template <ForwardRange Rng, class Proj = identity,
            IndirectCallablePredicate<projected<iterator_t<Rng>, Proj>> Pred>
    requires SizedRange<Rng>()
  safe_iterator_t<Rng>
  partition_point(Rng&& rng, Pred&& pred, Proj&& proj = Proj{}) {
    return __stl2::ext::partition_point_n(__stl2::begin(rng), __stl2::distance(rng),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
