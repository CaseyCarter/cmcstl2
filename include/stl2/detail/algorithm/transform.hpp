// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_TRANSFORM_HPP
#define STL2_DETAIL_ALGORITHM_TRANSFORM_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/tuple.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// transform [alg.transform]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O,
            class F, class Proj = identity>
    requires Writable<O, indirect_callable_result_t<F, projected<I, Proj>>>()
  tagged_pair<tag::in(I), tag::out(O)>
    transform(I first, S last, O result, F&& op_, Proj&& proj_ = Proj{}) {
      auto op = ext::make_callable_wrapper(__stl2::forward<F>(op_));
      auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
      for (; first != last; ++first, ++result) {
        *result = op(proj(*first));
      }
      return {first, result};
    }

  template <InputRange R, WeaklyIncrementable O, class F, class Proj = identity>
    requires Writable<O, indirect_callable_result_t<F, projected<iterator_t<R>, Proj>>>()
  tagged_pair<tag::in(safe_iterator_t<R>), tag::out(O)>
    transform(R&& r, O result, F&& op_, Proj&& proj_ = Proj{}) {
      return __stl2::transform(__stl2::begin(r), __stl2::end(r), result,
                               __stl2::forward<F>(op_), __stl2::forward<Proj>(proj_));
    }

  template <InputIterator I1, Sentinel<I1> S1, WeakInputIterator I2, WeaklyIncrementable O,
            class F, class Proj1 = identity, class Proj2 = identity>
  [[deprecated]] auto
  transform(I1 first1, S1 last1, I2 first2, O result,
            F&& op_, Proj1&& proj1_ = Proj1{}, Proj2&& proj2_ = Proj2{})
    requires Writable<O, indirect_callable_result_t<F, projected<I1, Proj1>,
                                                    projected<I2, Proj2>>>()
  {
    auto op = ext::make_callable_wrapper(__stl2::forward<F>(op_));
    auto proj1 = ext::make_callable_wrapper(__stl2::forward<Proj1>(proj1_));
    auto proj2 = ext::make_callable_wrapper(__stl2::forward<Proj2>(proj2_));
    for (; first1 != last1; ++first1, ++first2, ++result) {
      *result = op(proj1(*first1), proj2(*first2));
    }
    return __stl2::make_tagged_tuple<tag::in1, tag::in2, tag::out>(first1, first2, result);
  }

  template <InputRange Rng, WeakInputIterator I, WeaklyIncrementable O, class F,
            class Proj1 = identity, class Proj2 = identity>
  [[deprecated]] tagged_tuple<tag::in1(safe_iterator_t<Rng>), tag::in2(I), tag::out(O)>
  transform(Rng&& r1, I first2, O result, F&& op_,
            Proj1&& proj1_ = Proj1{}, Proj2&& proj2_ = Proj2{})
    requires Writable<O, indirect_callable_result_t<F,
      projected<iterator_t<Rng>, Proj1>, projected<I, Proj2>>>()
  {
    return __stl2::transform(
      __stl2::begin(r1), __stl2::end(r1), first2, result,
      __stl2::forward<F>(op_), __stl2::forward<Proj1>(proj1_),
      __stl2::forward<Proj2>(proj2_));
  }

  template <InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
            WeaklyIncrementable O, class F, class Proj1 = identity, class Proj2 = identity>
    requires Writable<O, indirect_callable_result_t<F, projected<I1, Proj1>, projected<I2, Proj2>>>()
  auto transform(I1 first1, S1 last1, I2 first2, S2 last2, O result,
                 F&& op_, Proj1&& proj1_ = Proj1{}, Proj2&& proj2_ = Proj2{}) {
    auto op = ext::make_callable_wrapper(__stl2::forward<F>(op_));
    auto proj1 = ext::make_callable_wrapper(__stl2::forward<Proj1>(proj1_));
    auto proj2 = ext::make_callable_wrapper(__stl2::forward<Proj2>(proj2_));
    for (; first1 != last1 && first2 != last2; ++first1, ++first2, ++result) {
      *result = op(proj1(*first1), proj2(*first2));
    }
    return __stl2::make_tagged_tuple<tag::in1, tag::in2, tag::out>(first1, first2, result);
  }

  template <InputRange Rng1, InputRange Rng2, WeaklyIncrementable O, class F,
            class Proj1 = identity, class Proj2 = identity>
    requires Writable<O, indirect_callable_result_t<F, projected<iterator_t<Rng1>, Proj1>,
      projected<iterator_t<Rng2>, Proj2>>>()
  tagged_tuple<tag::in1(safe_iterator_t<Rng1>), tag::in2(safe_iterator_t<Rng2>), tag::out(O)>
    transform(Rng1&& r1, Rng2&& r2, O result,
              F&& op_, Proj1&& proj1_ = Proj1{}, Proj2&& proj2_ = Proj2{}) {
      return __stl2::transform(
        __stl2::begin(r1), __stl2::end(r1), __stl2::begin(r2), __stl2::end(r2),
        result, __stl2::forward<F>(op_), __stl2::forward<Proj1>(proj1_),
        __stl2::forward<Proj2>(proj2_));
    }
} STL2_CLOSE_NAMESPACE

#endif
