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
#ifndef STL2_DETAIL_ALGORITHM_UNIQUE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_UNIQUE_COPY_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// unique_copy [alg.unique]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O,
            class Proj = identity,
            IndirectCallableRelation<Projected<I, Proj>> R = equal_to<>>
    requires IndirectlyCopyable<I, O>() && Copyable<ValueType<I>>()
  tagged_pair<tag::in(I), tag::out(O)>
  __unique_copy_no_fwd(I first, S last, O result, R comp_ = R{}, Proj proj_ = Proj{}) {
    if (first != last) {
      auto&& comp = __stl2::as_function(comp_);
      auto&& proj = __stl2::as_function(proj_);
      ValueType<I> saved = *first;
      *result = saved;
      ++result;
      while (++first != last) {
        if (!comp(proj(*first), proj(saved))) {
          saved = *first;
          *result = saved;
          ++result;
        }
      }
    }
    return {__stl2::move(first), __stl2::move(result)};
  }

  template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O,
            class Proj = identity,
            IndirectCallableRelation<Projected<I, Proj>> R = equal_to<>>
    requires IndirectlyCopyable<I, O>() && Copyable<ValueType<I>>()
  tagged_pair<tag::in(I), tag::out(O)>
  unique_copy(I first, S last, O result, R&& comp = R{}, Proj&& proj = Proj{}) {
    return __stl2::__unique_copy_no_fwd(__stl2::move(first), __stl2::move(last),
      __stl2::move(result), __stl2::forward<R>(comp), __stl2::forward<Proj>(proj));
  }

  template <InputIterator I, Sentinel<I> S, ForwardIterator O,
            class Proj = identity,
            IndirectCallableRelation<Projected<I, Proj>> R = equal_to<>>
    requires IndirectlyCopyable<I, O>()
  tagged_pair<tag::in(I), tag::out(O)>
  __unique_copy_res_fwd(I first, S last, O result, R comp_ = R{}, Proj proj_ = Proj{}) {
    if (first != last) {
      auto&& comp = __stl2::as_function(comp_);
      auto&& proj = __stl2::as_function(proj_);
      *result = *first;
      auto m = result;
      while (++first != last) {
        if (!comp(proj(*first), proj(*m))) {
          *++result = *first;
          m = result;
        }
      }
      ++result;
    }
    return {__stl2::move(first), __stl2::move(result)};
  }

  template <InputIterator I, Sentinel<I> S, ForwardIterator O,
            class Proj = identity,
            IndirectCallableRelation<Projected<I, Proj>> R = equal_to<>>
    requires IndirectlyCopyable<I, O>()
  tagged_pair<tag::in(I), tag::out(O)>
  unique_copy(I first, S last, O result, R&& comp = R{}, Proj&& proj = Proj{}) {
    return __stl2::__unique_copy_res_fwd(__stl2::move(first), __stl2::move(last),
      __stl2::move(result), __stl2::forward<R>(comp), __stl2::forward<Proj>(proj));
  }

  template <ForwardIterator I, Sentinel<I> S, WeaklyIncrementable O,
            class Proj = identity,
            IndirectCallableRelation<Projected<I, Proj>> R = equal_to<>>
    requires IndirectlyCopyable<I, O>()
  tagged_pair<tag::in(I), tag::out(O)>
  __unique_copy_src_fwd(I first, S last, O result, R comp_ = R{}, Proj proj_ = Proj{}) {
    if (first != last) {
      auto&& comp = __stl2::as_function(comp_);
      auto&& proj = __stl2::as_function(proj_);
      *result = *first;
      ++result;
      auto m = first;
      while (++first != last) {
        if (!comp(proj(*first), proj(*m))) {
          *result = *first;
          ++result;
          m = first;
        }
      }
    }
    return {__stl2::move(first), __stl2::move(result)};
  }

  template <ForwardIterator I, Sentinel<I> S, WeaklyIncrementable O,
            class Proj = identity,
            IndirectCallableRelation<Projected<I, Proj>> R = equal_to<>>
    requires IndirectlyCopyable<I, O>()
  tagged_pair<tag::in(I), tag::out(O)>
  unique_copy(I first, S last, O result, R&& comp = R{}, Proj&& proj = Proj{}) {
    return __stl2::__unique_copy_src_fwd(__stl2::move(first), __stl2::move(last),
       __stl2::move(result), __stl2::forward<R>(comp), __stl2::forward<Proj>(proj));
  }

  template <ForwardIterator I, Sentinel<I> S, ForwardIterator O,
            class Proj = identity,
            IndirectCallableRelation<Projected<I, Proj>> R = equal_to<>>
    requires IndirectlyCopyable<I, O>()
  tagged_pair<tag::in(I), tag::out(O)>
  unique_copy(I first, S last, O result, R&& comp = R{}, Proj&& proj = Proj{}) {
    return __stl2::__unique_copy_src_fwd(__stl2::move(first), __stl2::move(last),
       __stl2::move(result), __stl2::forward<R>(comp), __stl2::forward<Proj>(proj));
  }

  template <InputRange Rng, WeaklyIncrementable O, class Proj = identity,
            IndirectCallableRelation<Projected<IteratorType<Rng>, Proj>> R = equal_to<>>
    requires IndirectlyCopyable<IteratorType<Rng>, O>() &&
      (ForwardIterator<IteratorType<Rng>>() || ForwardIterator<O>() ||
       Copyable<ValueType<IteratorType<Rng>>>())
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
  unique_copy(Rng&& rng, O result, R&& comp = R{}, Proj&& proj = Proj{}) {
    return __stl2::unique_copy(__stl2::begin(rng), __stl2::end(rng),
       __stl2::move(result), __stl2::forward<R>(comp), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
