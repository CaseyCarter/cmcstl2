//  Copyright Eric Niebler 2015
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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/tuple.hpp>

namespace stl2 { inline namespace v1 {

// 20150803: Not to spec: the Destructible requirements are implicit.
template< InputIterator I, Sentinel<I> S, WeaklyIncrementable O, Destructible F,
          Destructible Proj = identity >
  requires Writable<O, IndirectCallableResultType<F, Projected<I, Proj>>>()
tagged_pair<tag::in(I), tag::out(O)>
transform(I first, S last, O result, F op_, Proj proj_ = Proj{}) {
  auto&& op = as_function(op_);
  auto&& proj = as_function(proj_);
  for(; first != last; ++first, ++result)
    *result = op(proj(*first));
  return {first, result};
}

// 20150801: Not to spec: the MoveConstructible requirements are implicit.
template< InputRange R, WeaklyIncrementable O, MoveConstructible F,
          MoveConstructible Proj = identity>
  requires Writable<O, IndirectCallableResultType<F, Projected<IteratorType<R>, Proj>>>()
tagged_pair<tag::in(safe_iterator_t<R>), tag::out(O)>
transform(R&& r, O result, F op_, Proj proj_ = Proj{}) {
  return stl2::transform(stl2::begin(r), stl2::end(r), result,
    stl2::move(op_), stl2::move(proj_));
}

template< InputIterator I1, Sentinel<I1> S1, WeakInputIterator I2, WeaklyIncrementable O,
          Destructible F, Destructible Proj1 = identity, Destructible Proj2 = identity >
  requires Writable<O, IndirectCallableResultType<F, Projected<I1, Proj1>, Projected<I2, Proj2>>>()
tagged_tuple<tag::in1(I1), tag::in2(I2), tag::out(O)>
  transform(I1 first1, S1 last1, I2 first2, O result,
            F op_, Proj1 proj1_ = Proj1{}, Proj2 proj2_ = Proj2{}) {
  auto&& op = as_function(op_);
  auto&& proj1 = as_function(proj1_);
  auto&& proj2 = as_function(proj2_);
  for(; first1 != last1; ++first1, ++first2, ++result)
    *result = op(proj1(*first1), proj2(*first2));
  return make_tagged_tuple<tag::in1, tag::in2, tag::out>(first1, first2, result);
}

template< InputRange Rng, WeakInputIterator I, WeaklyIncrementable O, MoveConstructible F,
          MoveConstructible Proj1 = identity, MoveConstructible Proj2 = identity >
  requires Writable<O, IndirectCallableResultType<F,
    Projected<IteratorType<Rng>, Proj1>, Projected<I, Proj2>>>()
tagged_tuple<tag::in1(safe_iterator_t<Rng>), tag::in2(I), tag::out(O)>
  transform(Rng&& r1, I first2, O result,
            F op_, Proj1 proj1_ = Proj1{}, Proj2 proj2_ = Proj2{}) {
  return stl2::transform(stl2::begin(r1), stl2::end(r1), first2, result,
    stl2::move(op_), stl2::move(proj1_), stl2::move(proj2_));
}

template< InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
          WeaklyIncrementable O, MoveConstructible F, MoveConstructible Proj1 = identity,
          MoveConstructible Proj2 = identity >
  requires Writable<O, IndirectCallableResultType<F, Projected<I1, Proj1>, Projected<I2, Proj2>>>()
tagged_tuple<tag::in1(I1), tag::in2(I2), tag::out(O)>
  transform(I1 first1, S1 last1, I2 first2, S2 last2, O result,
            F op_, Proj1 proj1_ = Proj1{}, Proj2 proj2_ = Proj2{}) {
  auto&& op = as_function(op_);
  auto&& proj1 = as_function(proj1_);
  auto&& proj2 = as_function(proj2_);
  for(; first1 != last1 && first2 != last2; ++first1, ++first2, ++result)
    *result = op(proj1(*first1), proj2(*first2));
  return make_tagged_tuple<tag::in1, tag::in2, tag::out>(first1, first2, result);
}

template<InputRange Rng1, InputRange Rng2,
  WeaklyIncrementable O, MoveConstructible F, MoveConstructible Proj1 = identity,
  MoveConstructible Proj2 = identity>
  requires Writable<O, IndirectCallableResultType<F, Projected<IteratorType<Rng1>, Proj1>,
    Projected<IteratorType<Rng2>, Proj2>>>()
tagged_tuple<tag::in1(safe_iterator_t<Rng1>), tag::in2(safe_iterator_t<Rng2>), tag::out(O)>
  transform(Rng1&& r1, Rng2&& r2, O result,
            F op_, Proj1 proj1_ = Proj1{}, Proj2 proj2_ = Proj2{}) {
  return stl2::transform(stl2::begin(r1), stl2::end(r1), stl2::begin(r2), stl2::end(r2),
    result, stl2::move(op_), stl2::move(proj1_), stl2::move(proj2_));
}

}}

#endif
