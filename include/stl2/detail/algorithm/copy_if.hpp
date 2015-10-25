// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_COPY_IF_HPP
#define STL2_DETAIL_ALGORITHM_COPY_IF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// copy_if [alg.copy]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O, class Proj = identity,
            IndirectCallablePredicate<projected<I, Proj>> Pred>
    requires IndirectlyCopyable<I, O>()
  tagged_pair<tag::in(I), tag::out(O)>
  copy_if(I first, S last, O result, Pred&& pred_, Proj&& proj_ = Proj{}) {
    auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));

    for (; first != last; ++first) {
      auto&& v = *first;
      if (pred(proj(v))) {
        *result = __stl2::forward<decltype(v)>(v);
        ++result;
      }
    }

    return {first, result};
  }

  template<InputRange Rng, WeaklyIncrementable O, class Proj = identity,
           IndirectCallablePredicate<projected<iterator_t<Rng>, Proj>> Pred>
    requires IndirectlyCopyable<iterator_t<Rng>, O>()
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
  copy_if(Rng&& rng, O result, Pred&& pred, Proj&& proj = Proj{}) {
    return __stl2::copy_if(__stl2::begin(rng), __stl2::end(rng),
      __stl2::move(result), __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
