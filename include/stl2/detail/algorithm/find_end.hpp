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
#ifndef STL2_DETAIL_ALGORITHM_FIND_END_HPP
#define STL2_DETAIL_ALGORITHM_FIND_END_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/optional.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// find_end [alg.find.end]
//
STL2_OPEN_NAMESPACE {
  template <ForwardIterator I1, Sentinel<I1> S1, ForwardIterator I2,
            Sentinel<I2> S2, class Proj = identity,
            IndirectCallableRelation<I2, Projected<I1, Proj>> Pred = equal_to<>>
  I1 find_end(I1 first1, const S1 last1,
              const I2 first2, const S2 last2,
              Pred pred_ = Pred{}, Proj proj_ = Proj{}) {
    if (first2 == last2) {
      return __stl2::next(first1, last1);
    }

    auto &&pred = __stl2::as_function(pred_);
    auto &&proj = __stl2::as_function(proj_);

    optional<I1> res;
    for (; first1 != last1; ++first1) {
      if (pred(proj(*first1), *first2)) {
        auto m1 = first1;
        auto m2 = first2;
        do {
          if (++m2 == last2) {
            res = first1;
            break;
          }
          if (++m1 == last1) {
            return __stl2::move(res).value_or(__stl2::move(m1));
          }
        } while (pred(proj(*m1), *m2));
      }
    }
    return __stl2::move(res).value_or(__stl2::move(first1));
  }

  template <BidirectionalIterator I1, BidirectionalIterator I2, class Proj = identity,
            IndirectCallableRelation<I2, Projected<I1, Proj>> Pred = equal_to<>>
  I1 find_end(I1 first1, I1 last1, I2 first2, I2 last2, Pred pred_, Proj proj_) {
    if (first2 == last2) {
      return last1;  // Everything matches an empty sequence
    }

    auto&& pred = __stl2::as_function(pred_);
    auto&& proj = __stl2::as_function(proj_);

    --last2;
    auto l1 = last1;
    while (l1 != first1) {
      if (pred(proj(*--l1), *last2)) {
        auto m1 = l1;
        auto m2 = last2;
        do {
          if (m2 == first2) {
            return m1;
          }
          if (m1 == first1) {
            return last1;
          }
        } while (pred(proj(*--m1), *--m2));
      }
    }

    return last1;
  }

  template <RandomAccessIterator I1, RandomAccessIterator I2, class Proj = identity,
            IndirectCallableRelation<I2, Projected<I1, Proj>> Pred = equal_to<>>
  I1 find_end(I1 first1, I1 last1, I2 first2, I2 last2, Pred pred_, Proj proj_) {
    // Take advantage of knowing source and pattern lengths.
    // Stop short when source is smaller than pattern
    const auto len2 = last2 - first2;
    if (len2 == 0 || last1 - first1 < len2) {
      return last1;
    }

    auto&& pred = __stl2::as_function(pred_);
    auto&& proj = __stl2::as_function(proj_);
    const auto s = first1 + (len2 - 1);  // End of pattern match can't go before here

    for (auto l1 = last1; l1 != s; --l1) {
      auto m1 = l1;
      auto m2 = last2;
      while (pred(proj(*--m1), *--m2)) {
        if (m2 == first2) {
          return m1;
        }
      }
    }
    return last1;
  }

  template <BidirectionalIterator I1, Sentinel<I1> S1,
            BidirectionalIterator I2, Sentinel<I2> S2, class Proj = identity,
            IndirectCallableRelation<I2, Projected<I1, Proj>> Pred = equal_to<>>
  I1 find_end(I1 first1, S1 s1, I2 first2, S2 s2, Pred&& pred, Proj&& proj = {}) {
    return __stl2::find_end(
      first1, __stl2::next(first1, s1),
      first2, __stl2::next(first2, s2),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }

  template <ForwardRange Rng1, ForwardRange Rng2, class Proj = identity,
            IndirectCallableRelation<IteratorType<Rng2>,
              Projected<IteratorType<Rng1>, Proj>> Pred = equal_to<>>
  safe_iterator_t<Rng1>
  find_end(Rng1&& rng1, Rng2&& rng2, Pred&& pred = Pred{}, Proj&& proj = Proj{}) {
    return __stl2::find_end(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
