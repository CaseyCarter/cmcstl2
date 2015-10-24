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
  template <class I, class S, class O, class Proj, class R>
  tagged_pair<tag::in(I), tag::out(O)>
  __unique_copy(false_type, false_type, I first, S last, O result,
                R&& comp_, Proj&& proj_) {
    if (first != last) {
      auto comp = ext::make_callable_wrapper(__stl2::forward<R>(comp_));
      auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
      ValueType<I> saved = *first;
      *result = saved;
      ++result;
      while (++first != last) {
        auto&& v = *first;
        if (!comp(proj(v), proj(saved))) {
          saved = __stl2::forward<decltype(v)>(v);
          *result = saved;
          ++result;
        }
      }
    }
    return {__stl2::move(first), __stl2::move(result)};
  }

  template <class I, class S, class O, class Proj, class R>
  tagged_pair<tag::in(I), tag::out(O)>
  __unique_copy(false_type, true_type, I first, S last, O result,
                R&& comp_, Proj&& proj_) {
    if (first != last) {
      auto comp = ext::make_callable_wrapper(__stl2::forward<R>(comp_));
      auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
      *result = *first;
      auto m = result;
      while (++first != last) {
        auto&& v = *first;
        if (!comp(proj(v), proj(*m))) {
          *++result = __stl2::forward<decltype(v)>(v);
          m = result;
        }
      }
      ++result;
    }
    return {__stl2::move(first), __stl2::move(result)};
  }

  template <class I, class S, class O, class Proj, class R>
  tagged_pair<tag::in(I), tag::out(O)>
  __unique_copy(true_type, auto, I first, S last, O result,
                R&& comp_, Proj&& proj_) {
    if (first != last) {
      auto comp = ext::make_callable_wrapper(__stl2::forward<R>(comp_));
      auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
      *result = *first;
      ++result;
      auto m = first;
      while (++first != last) {
        auto&& v = *first;
        if (!comp(proj(v), proj(*m))) {
          *result = __stl2::forward<decltype(v)>(v);
          ++result;
          m = first;
        }
      }
    }
    return {__stl2::move(first), __stl2::move(result)};
  }

  template <class I, class S, class O, class P, class R>
  constexpr bool __unique_copy_req = false;
  template <class I, class S, class O, class P, class R>
    requires IndirectlyCopyable<I, O>() &&
      (models::ForwardIterator<I> || models::ForwardIterator<O> || Copyable<ValueType<I>>())
  constexpr bool __unique_copy_req<I, S, O, P, R> = true;

  template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O,
            class Proj = identity,
            IndirectCallableRelation<Projected<I, Proj>> R = equal_to<>>
    requires __unique_copy_req<I, S, O, Proj, R>
  tagged_pair<tag::in(I), tag::out(O)>
  unique_copy(I first, S last, O result, R&& comp = R{}, Proj&& proj = Proj{}) {
    return __stl2::__unique_copy(
       meta::bool_<models::ForwardIterator<I>>{},
       meta::bool_<models::ForwardIterator<O>>{},
       __stl2::move(first), __stl2::move(last),
       __stl2::move(result),
       __stl2::forward<R>(comp),
       __stl2::forward<Proj>(proj));
  }

  template <InputRange Rng, WeaklyIncrementable O, class Proj = identity,
            IndirectCallableRelation<Projected<IteratorType<Rng>, Proj>> R = equal_to<>>
    requires __unique_copy_req<IteratorType<Rng>, SentinelType<Rng>, O, Proj, R>
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
  unique_copy(Rng&& rng, O result, R&& comp = R{}, Proj&& proj = Proj{}) {
    return __stl2::__unique_copy(
       meta::bool_<models::ForwardIterator<IteratorType<Rng>>>{},
       meta::bool_<models::ForwardIterator<O>>{},
       __stl2::begin(rng), __stl2::end(rng),
       __stl2::move(result),
       __stl2::forward<R>(comp),
       __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
