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
#ifndef STL2_DETAIL_ALGORITHM_UNIQUE_HPP
#define STL2_DETAIL_ALGORITHM_UNIQUE_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/adjacent_find.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// unique [alg.unique]
//
STL2_OPEN_NAMESPACE {
  template <ForwardIterator I, Sentinel<I> S,
            class R = equal_to<>, class Proj = identity>
  requires
    models::Permutable<I> &&
    models::IndirectCallableRelation<__f<R>, projected<I, __f<Proj>>>
  I unique(I first, S last, R&& comp_ = R{}, Proj&& proj_ = Proj{})
  {
    auto comp = ext::make_callable_wrapper(__stl2::forward<R>(comp_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
    first = __stl2::adjacent_find(
      __stl2::move(first), last, __stl2::ref(comp), __stl2::ref(proj));
    if (first != last) {
      for (auto m = __stl2::next(first, 2); m != last; ++m) {
        if (!comp(proj(*first), proj(*m))) {
          ++first;
          *first = __stl2::iter_move(m);
        }
      }
      ++first;
    }
    return first;
  }

  template <ForwardRange Rng, class R = equal_to<>,
            class Proj = identity>
  requires
    models::Permutable<iterator_t<Rng>> &&
    models::IndirectCallableRelation<
      __f<R>, projected<iterator_t<Rng>, __f<Proj>>>
  safe_iterator_t<Rng>
  unique(Rng&& rng, R&& comp = R{}, Proj&& proj = Proj{})
  {
    return __stl2::unique(__stl2::begin(rng), __stl2::end(rng),
      __stl2::forward<R>(comp), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
