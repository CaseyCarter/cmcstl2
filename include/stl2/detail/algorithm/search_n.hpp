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
//
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
#ifndef STL2_DETAIL_ALGORITHM_SEARCH_N_HPP
#define STL2_DETAIL_ALGORITHM_SEARCH_N_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>

///////////////////////////////////////////////////////////////////////////
// search_n [alg.search]
//
STL2_OPEN_NAMESPACE {
  template <ForwardIterator I, Sentinel<I> S, class T, class Pred, class Proj>
    requires models::IndirectlyComparable<I, const T*, Pred, Proj>
  I __unsized_search_n(I first, S last, DifferenceType<I> count,
                       const T& value, Pred& pred_, Proj& proj_) {
    if (count <= 0) {
      return first;
    }

    auto&& pred = __stl2::as_function(pred_);
    auto&& proj = __stl2::as_function(proj_);

    for (; first != last; ++first) {
      if (pred(proj(*first), value)) {
        auto saved = first;
        DifferenceType<I> n = 0;
        do {
          if (++n == count) {
            return saved;
          }
          if (++first == last) {
            return first;
          }
        } while(pred(proj(*first), value));
      }
    }

    return first;
  }

  template <ForwardIterator I, Sentinel<I> S, class T,
            class Pred = equal_to<>, class Proj = identity>
    requires models::IndirectlyComparable<I, const T*, Pred, Proj>
  I search_n(I first, S last, DifferenceType<I> count,
             const T& value, Pred pred = Pred{}, Proj proj = Proj{}) {
    return __stl2::__unsized_search_n(__stl2::move(first), __stl2::move(last),
                                      count, value, pred, proj);
  }

  template <ForwardIterator I, Sentinel<I> S, class T, class Pred, class Proj>
    requires models::IndirectlyComparable<I, const T*, Pred, Proj>
  I __sized_search_n(I first_, S last, DifferenceType<I> d_, DifferenceType<I> count,
                     const T& value, Pred& pred_, Proj& proj_) {
    if (count <= 0) {
      return first_;
    }

    auto&& pred = __stl2::as_function(pred_);
    auto&& proj = __stl2::as_function(proj_);

    auto d = d_;
    auto first = __stl2::ext::uncounted(first_);

    for (; d >= count; ++first, --d) {
      if (pred(proj(*first), value)) {
        // *first matches val, now match elements after here
        auto saved = first;
        DifferenceType<I> n = 0;
        do {
          if (++n == count) {
            // Pattern exhausted, saved is the answer (works for 1 element pattern)
            return __stl2::ext::recounted(first_, __stl2::move(saved), d_ - d);
          }
        } while (pred(proj(*++first), value));
        d -= n;
      }
    }

    return __stl2::next(__stl2::ext::recounted(first_, __stl2::move(first), d_ - d),
                        __stl2::move(last));
  }

  template <ForwardIterator I, Sentinel<I> S, class T,
            class Pred = equal_to<>, class Proj = identity>
    requires models::IndirectlyComparable<I, const T*, Pred, Proj> &&
      SizedIteratorRange<I, S>()
  I search_n(I first, S last, DifferenceType<I> count,
             const T& value, Pred pred = Pred{}, Proj proj = Proj{}) {
    return __stl2::__sized_search_n(__stl2::move(first), __stl2::move(last),
                                    __stl2::distance(first, last),
                                    count, value, pred, proj);
  }

  template <ForwardRange Rng, class T, class Pred = equal_to<>, class Proj = identity>
    requires models::IndirectlyComparable<IteratorType<Rng>, const T*, Pred, Proj>
  safe_iterator_t<Rng> search_n(Rng&& rng, DifferenceType<IteratorType<Rng>> count,
                                const T& value, Pred&& pred = Pred{}, Proj&& proj = Proj{}) {
    return __stl2::__unsized_search_n(__stl2::begin(rng), __stl2::end(rng),
                                      count, value, pred, proj);
  }

  template <ForwardRange Rng, class T, class Pred = equal_to<>, class Proj = identity>
    requires SizedRange<Rng>() &&
      models::IndirectlyComparable<IteratorType<Rng>, const T*, Pred, Proj>
  safe_iterator_t<Rng> search_n(Rng&& rng, DifferenceType<IteratorType<Rng>> count,
                                const T& value, Pred&& pred = Pred{}, Proj&& proj = Proj{}) {
    return __stl2::__sized_search_n(__stl2::begin(rng), __stl2::end(rng),
                                    __stl2::distance(rng), count, value, pred, proj);
  }
} STL2_CLOSE_NAMESPACE

#endif
