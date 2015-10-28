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
#ifndef STL2_DETAIL_ALGORITHM_MERGE_MOVE_HPP
#define STL2_DETAIL_ALGORITHM_MERGE_MOVE_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/tuple.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// merge_move [alg.merge]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I1, Sentinel<I1> S1,
            InputIterator I2, Sentinel<I2> S2,
            class O, class Comp = less<>,
            class Proj1 = identity, class Proj2 = identity>
    requires MoveMergeable<I1, I2, O, Comp, Proj1, Proj2>()
  tagged_tuple<tag::in1(I1), tag::in2(I2), tag::out(O)>
  merge_move(I1 first1, S1 last1, I2 first2, S2 last2, O result,
             Comp&& comp_ = Comp{}, Proj1&& proj1_ = Proj1{},
             Proj2&& proj2_ = Proj2{}) {
    auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
    auto proj1 = ext::make_callable_wrapper(__stl2::forward<Proj1>(proj1_));
    auto proj2 = ext::make_callable_wrapper(__stl2::forward<Proj2>(proj2_));
    while (true) {
      if (first1 == last1) {
        __stl2::tie(first2, result) = __stl2::move(
          __stl2::move(first2), __stl2::move(last2), __stl2::move(result));
        break;
      }
      if (first2 == last2) {
        __stl2::tie(first1, result) = __stl2::move(
          __stl2::move(first1), __stl2::move(last1), __stl2::move(result));
        break;
      }
      if (comp(proj1(*first1), proj2(*first2))) {
        *result = __stl2::iter_move(first1);
        ++first1;
      } else {
        *result = __stl2::iter_move(first2);
        ++first2;
      }
      ++result;
    }
    return {__stl2::move(first1), __stl2::move(first2), __stl2::move(result)};
  }

  template <InputRange Rng1, InputRange Rng2,
            class O, class Comp = less<>,
            class Proj1 = identity, class Proj2 = identity>
    requires MoveMergeable<iterator_t<Rng1>, iterator_t<Rng2>,
                          O, Comp, Proj1, Proj2>()
  tagged_tuple<tag::in1(safe_iterator_t<Rng1>), tag::in2(safe_iterator_t<Rng2>),
               tag::out(O)>
  merge_move(Rng1&& rng1, Rng2&& rng2, O result, Comp&& comp = Comp{},
             Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    return __stl2::merge_move(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::move(result), __stl2::forward<Comp>(comp),
      __stl2::forward<Proj1>(proj1), __stl2::forward<Proj2>(proj2));
  }
} STL2_CLOSE_NAMESPACE

#endif
