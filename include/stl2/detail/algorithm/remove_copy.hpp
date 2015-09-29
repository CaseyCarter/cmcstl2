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
#ifndef STL2_DETAIL_ALGORITHM_REMOVE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_REMOVE_COPY_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// remove_copy [alg.remove]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O, class T,
            class Proj = identity>
    requires IndirectlyCopyable<I, O>() &&
      IndirectCallableRelation<equal_to<>, Projected<I, Proj>, const T*>()
  tagged_pair<tag::in(I), tag::out(O)>
  remove_copy(I first, S last, O result, const T& value, Proj proj_ = Proj{}) {
    auto&& proj = __stl2::as_function(proj_);

    for (; first != last; ++first) {
      auto&& v = *first;
      if (proj(v) != value) {
        *result = __stl2::forward<decltype(v)>(v);
        ++result;
      }
    }
    return {__stl2::move(first), __stl2::move(result)};
  }

  template <InputRange Rng, WeaklyIncrementable O, class T, class Proj = identity>
    requires IndirectlyCopyable<IteratorType<Rng>, O>() &&
      IndirectCallableRelation<equal_to<>, Projected<IteratorType<Rng>, Proj>, const T*>()
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
  remove_copy(Rng&& rng, O result, const T& value, Proj&& proj = Proj{}) {
    return __stl2::remove_copy(__stl2::begin(rng), __stl2::end(rng),
       __stl2::move(result), value, __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
