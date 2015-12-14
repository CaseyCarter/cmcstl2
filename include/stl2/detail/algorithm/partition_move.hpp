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
#include <stl2/detail/algorithm/partition_copy.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// partition_move [alg.partitions]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O1,
            WeaklyIncrementable O2, class Pred, class Proj = identity>
  requires
    models::IndirectlyMovable<I, O1> &&
    models::IndirectlyMovable<I, O2> &&
    models::IndirectCallablePredicate<
      __f<Pred>, projected<I, __f<Proj>>>
  tagged_tuple<tag::in(I), tag::out1(O1), tag::out2(O2)>
  partition_move(I first, S last, O1 out_true, O2 out_false,
                 Pred&& pred_, Proj&& proj_ = Proj{})
  {
#if 1
    auto res = __stl2::partition_copy(
      __stl2::make_move_iterator(first), __stl2::make_move_sentinel(last),
      out_true, out_false, __stl2::forward<Pred>(pred_),
      __stl2::forward<Proj>(proj_));
    return {res.in().base(), res.out1(), res.out2()};
#else
    auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));

    for (; first != last; ++first) {
      if (pred(proj(*first))) {
        *out_true = __stl2::iter_move(first);
        ++out_true;
      } else {
        *out_false = __stl2::iter_move(first);
        ++out_false;
      }
    }
    return {__stl2::move(first),
      __stl2::move(out_true), __stl2::move(out_false)};
#endif
  }

  template <InputRange Rng, class O1, class O2,
            class Pred, class Proj = identity>
  requires
    models::WeaklyIncrementable<__f<O1>> &&
    models::WeaklyIncrementable<__f<O2>> &&
    models::IndirectlyMovable<iterator_t<Rng>, __f<O1>> &&
    models::IndirectlyMovable<iterator_t<Rng>, __f<O2>> &&
    models::IndirectCallablePredicate<
      __f<Pred>, projected<iterator_t<Rng>, __f<Proj>>>
  tagged_tuple<
    tag::in(safe_iterator_t<Rng>),
    tag::out1(__f<O1>),
    tag::out2(__f<O2>)>
  partition_move(Rng&& rng, O1&& out_true, O2&& out_false,
                 Pred&& pred, Proj&& proj = Proj{})
  {
    return __stl2::partition_move(
      __stl2::begin(rng), __stl2::end(rng),
      __stl2::forward<O1>(out_true), __stl2::forward<O2>(out_false),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }

  // Extension
  template <class E, class O1, class O2,
            class Pred, class Proj = identity>
  requires
    models::WeaklyIncrementable<__f<O1>> &&
    models::WeaklyIncrementable<__f<O2>> &&
    models::IndirectlyMovable<const E*, __f<O1>> &&
    models::IndirectlyMovable<const E*, __f<O2>> &&
    models::IndirectCallablePredicate<
      __f<Pred>, projected<const E*, __f<Proj>>>
  tagged_tuple<
    tag::in(dangling<const E*>),
    tag::out1(__f<O1>),
    tag::out2(__f<O2>)>
  partition_move(std::initializer_list<E>&& rng,
                 O1&& out_true, O2&& out_false,
                 Pred&& pred, Proj&& proj = Proj{})
  {
    return __stl2::partition_move(
      __stl2::begin(rng), __stl2::end(rng),
      __stl2::forward<O1>(out_true), __stl2::forward<O2>(out_false),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
