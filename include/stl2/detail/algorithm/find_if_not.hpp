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
#ifndef STL2_DETAIL_ALGORITHM_FIND_IF_NOT_HPP
#define STL2_DETAIL_ALGORITHM_FIND_IF_NOT_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// find_if_not [alg.find]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, class Proj = identity,
            IndirectCallablePredicate<projected<I, Proj>> Pred>
  I find_if_not(I first, S last, Pred&& pred_, Proj&& proj_ = Proj{}) {
    auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
    for (; first != last && pred(proj(*first)); ++first) {
      ;
    }
    return first;
  }

  template <InputRange Rng, class Proj = identity,
            IndirectCallablePredicate<projected<iterator_t<Rng>, Proj>> Pred>
  safe_iterator_t<Rng>
  find_if_not(Rng&& rng, Pred&& pred, Proj&& proj = Proj{}) {
    return __stl2::find_if_not(__stl2::begin(rng), __stl2::end(rng),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }

  // Extension
  template <class E, class Proj = identity,
            IndirectCallablePredicate<projected<const E*, Proj>> Pred>
  dangling<const E*>
  find_if_not(std::initializer_list<E>&& il, Pred&& pred, Proj&& proj = Proj{}) {
    return __stl2::find_if_not(il.begin(), il.end(),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
