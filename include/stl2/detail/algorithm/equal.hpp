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
#ifndef STL2_DETAIL_ALGORITHM_EQUAL_HPP
#define STL2_DETAIL_ALGORITHM_EQUAL_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// equal [alg.equal]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I1, Sentinel<I1> S1, WeakInputIterator I2,
            class Pred, class Proj1, class Proj2>
    requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>()
  bool __equal_unsized(I1 first1, S1 last1, I2 first2, Pred pred_,
                       Proj1 proj1_, Proj2 proj2_) {
    auto&& pred = __stl2::as_function(pred_);
    auto&& proj1 = __stl2::as_function(proj1_);
    auto&& proj2 = __stl2::as_function(proj2_);

    for (; first1 != last1; ++first1, ++first2) {
      if (!pred(proj1(*first1), proj2(*first2))) {
        return false;
      }
    }
    return true;
  }

  template <InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
            class Pred, class Proj1, class Proj2>
    requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>()
  bool __equal_unsized(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred_,
                       Proj1 proj1_, Proj2 proj2_) {
    auto&& pred = __stl2::as_function(pred_);
    auto&& proj1 = __stl2::as_function(proj1_);
    auto&& proj2 = __stl2::as_function(proj2_);

    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
      if (!pred(proj1(*first1), proj2(*first2))) {
        return false;
      }
    }
    return first1 == last1 && first2 == last2;
  }

  template <InputIterator I1, Sentinel<I1> S1, WeakInputIterator I2,
            class Pred = equal_to<>, class Proj1 = identity, class Proj2 = identity>
    requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>()
  bool equal(I1 first1, S1 last1, I2 first2, Pred&& pred = Pred{},
             Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    return __stl2::__equal_unsized(
      first1, last1, first2,
      __stl2::forward<Pred>(pred), __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  template <InputRange Rng1, WeakInputIterator I2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
    requires IndirectlyComparable<IteratorType<Rng1>, I2, Pred, Proj1, Proj2>()
  bool equal(Rng1&& rng1, I2 first2, Pred&& pred = Pred{},
             Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    return __stl2::__equal_unsized(
      __stl2::begin(rng1), __stl2::end(rng1),
      first2, __stl2::forward<Pred>(pred),
      __stl2::forward<Proj1>(proj1), __stl2::forward<Proj2>(proj2));
  }

  template <InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
            class Pred = equal_to<>, class Proj1 = identity, class Proj2 = identity>
    requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>()
  bool equal(I1 first1, S1 last1, I2 first2, S2 last2, Pred&& pred = Pred{},
             Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    return __stl2::__equal_unsized(
      first1, last1, first2, last2,
      __stl2::forward<Pred>(pred), __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  template <InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
            class Pred = equal_to<>, class Proj1 = identity, class Proj2 = identity>
    requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>() &&
      SizedIteratorRange<I1, S1>() && SizedIteratorRange<I2, S2>()
  bool equal(I1 first1, S1 last1, I2 first2, S2 last2, Pred&& pred = Pred{},
             Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    using C = CommonType<DifferenceType<I1>, DifferenceType<I2>>;
    return C(last1 - first1) == C(last2 - first2) &&
      __stl2::__equal_unsized(first1, last1, first2, last2,
        __stl2::forward<Pred>(pred), __stl2::forward<Proj1>(proj1),
        __stl2::forward<Proj2>(proj2));
  }

  template <InputRange Rng1, InputRange Rng2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
    requires IndirectlyComparable<IteratorType<Rng1>, IteratorType<Rng2>, Pred, Proj1, Proj2>()
  bool equal(Rng1&& rng1, Rng2&& rng2, Pred&& pred = Pred{},
             Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    return __stl2::__equal_unsized(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::forward<Pred>(pred),
      __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  template <InputRange Rng1, InputRange Rng2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
    requires IndirectlyComparable<IteratorType<Rng1>,
                                  IteratorType<Rng2>, Pred, Proj1, Proj2>() &&
      SizedRange<Rng1>() && SizedRange<Rng2>()
  bool equal(Rng1&& rng1, Rng2&& rng2, Pred&& pred = Pred{},
             Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    using C = CommonType<DifferenceType<Rng1>, DifferenceType<Rng2>>;
    return C(__stl2::size(rng1)) == C(__stl2::size(rng2)) &&
      __stl2::__equal_unsized(
        __stl2::begin(rng1), __stl2::end(rng1),
        __stl2::begin(rng2), __stl2::end(rng2),
        __stl2::forward<Pred>(pred),
        __stl2::forward<Proj1>(proj1),
        __stl2::forward<Proj2>(proj2));
  }
} STL2_CLOSE_NAMESPACE

#endif
