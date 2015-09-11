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
#ifndef STL2_DETAIL_ALGORITHM_FOR_EACH_HPP
#define STL2_DETAIL_ALGORITHM_FOR_EACH_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// for_each [alg.for_each]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, class Proj = identity,
            IndirectCallable<Projected<I, Proj>> Fun>
  tagged_pair<tag::in(I), tag::fun(Fun)>
  for_each(I first, S last, Fun fun_, Proj proj_ = Proj{}) {
    auto &&fun = __stl2::as_function(fun_);
    auto &&proj = __stl2::as_function(proj_);
    for (; first != last; ++first) {
      (void)fun(proj(*first));
    }
    return {__stl2::move(first), __stl2::move(fun_)};
  }

  template <InputRange Rng, class Proj = identity,
            IndirectCallable<Projected<IteratorType<Rng>, Proj>> Fun>
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::fun(decay_t<Fun>)>
  for_each(Rng&& rng, Fun&& f, Proj&& proj = Proj{}) {
    return __stl2::for_each(__stl2::begin(rng), __stl2::end(rng),
      __stl2::forward<Fun>(f), __stl2::forward<Proj>(proj));
  }

  template <class E, class Proj = identity,
            IndirectCallable<Projected<const E*, Proj>> Fun>
  tagged_pair<tag::in(dangling<const E*>), tag::fun(decay_t<Fun>)>
  for_each(std::initializer_list<E>&& il, Fun&& f, Proj&& proj = Proj{}) {
    return __stl2::for_each(il.begin(), il.end(),
      __stl2::forward<Fun>(f), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
