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
#ifndef STL2_DETAIL_ALGORITHM_SEARCH_HPP
#define STL2_DETAIL_ALGORITHM_SEARCH_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>

///////////////////////////////////////////////////////////////////////////
// search [alg.search]
//
STL2_OPEN_NAMESPACE {
  template <ForwardIterator I1, Sentinel<I1> S1,
            ForwardIterator I2, Sentinel<I2> S2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
    requires models::IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
  I1 search(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred_ = Pred{},
            Proj1 proj1_ = Proj1{}, Proj2 proj2_ = Proj2{}) {
    if (first2 == last2) {
      return first1;
    }

    auto&& pred = __stl2::as_function(pred_);
    auto&& proj1 = __stl2::as_function(proj1_);
    auto&& proj2 = __stl2::as_function(proj2_);

    for (; first1 != last1; ++first1) {
      if (pred(proj1(*first1), proj2(*first2))) {
        // *first1 matches *first2, now match elements after here
        auto m1 = first1;
        auto m2 = first2;
        do {
          // If pattern exhausted, first1 is the answer (works for 1 element pattern)
          if (++m2 == last2) {
            return first1;
          }
          // Otherwise if source exhausted, pattern not found
          if (++m1 == last1) {
            return m1;
          }
        } while (pred(proj1(*m1), proj2(*m2)));
      }
    }

    return first1;
  }

  template <ForwardIterator I1, Sentinel<I1> S1,
            ForwardIterator I2, Sentinel<I2> S2,
            class Pred, class Proj1, class Proj2>
    requires models::IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
  I1 __sized_search(const I1 first1_, S1 last1, const DifferenceType<I1> d1_,
                    I2 first2, S2 last2, const DifferenceType<I2> d2,
                    Pred pred_, Proj1 proj1_, Proj2 proj2_) {
    if (d2 == 0) {
      return first1_;
    }

    auto&& pred = __stl2::as_function(pred_);
    auto&& proj1 = __stl2::as_function(proj1_);
    auto&& proj2 = __stl2::as_function(proj2_);

    auto d1 = d1_;
    auto first1 = __stl2::ext::uncounted(first1_);
    for(; d1 >= d2; ++first1, --d1) {
      if (pred(proj1(*first1), proj2(*first2))) {
        auto m1 = first1;
        auto m2 = first2;
        do {
          if (++m2 == last2) {
            return __stl2::ext::recounted(first1_, first1, d1_ - d1);
          }
        } while (pred(proj1(*++m1), proj2(*m2)));
      }
    }
    return __stl2::next(__stl2::ext::recounted(first1_, first1, d1_ - d1), last1);
  }

  // Extension
  template <ForwardIterator I1, Sentinel<I1> S1,
            ForwardIterator I2, Sentinel<I2> S2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
    requires SizedIteratorRange<I1, S1>() && SizedIteratorRange<I2, S2>() &&
      models::IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
  I1 search(I1 first1, S1 last1, I2 first2, S2 last2, Pred&& pred = Pred{},
            Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    return __stl2::__sized_search(
      first1, last1, __stl2::distance(first1, last1),
      first2, last2, __stl2::distance(first2, last2),
      __stl2::forward<Pred>(pred),
      __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  template <ForwardRange Rng1, ForwardRange Rng2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
    requires models::IndirectlyComparable<IteratorType<Rng1>, IteratorType<Rng2>, Pred, Proj1, Proj2>
  safe_iterator_t<Rng1> search(Rng1&& rng1, Rng2&& rng2, Pred&& pred = Pred{},
                               Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    return __stl2::search(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::forward<Pred>(pred),
      __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  // Extension
  template <ForwardRange Rng1, ForwardRange Rng2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
    requires SizedRange<Rng1>() && SizedRange<Rng2>() &&
    models::IndirectlyComparable<IteratorType<Rng1>, IteratorType<Rng2>, Pred, Proj1, Proj2>
  safe_iterator_t<Rng1> search(Rng1&& rng1, Rng2&& rng2, Pred&& pred = Pred{},
                               Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    return __stl2::__sized_search(
      __stl2::begin(rng1), __stl2::end(rng1), __stl2::distance(rng1),
      __stl2::begin(rng2), __stl2::end(rng2), __stl2::distance(rng2),
      __stl2::forward<Pred>(pred),
      __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }
} STL2_CLOSE_NAMESPACE

#endif
