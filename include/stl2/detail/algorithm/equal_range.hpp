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
#ifndef STL2_DETAIL_ALGORITHM_EQUAL_RANGE_HPP
#define STL2_DETAIL_ALGORITHM_EQUAL_RANGE_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/lower_bound.hpp>
#include <stl2/detail/algorithm/upper_bound.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// equal_range [equal.range]
// TODO: optimize this to perform the LB and UB probes simultaneously.
//
STL2_OPEN_NAMESPACE {
  template <ForwardIterator I, Sentinel<I> S, class T, class Proj = identity,
            IndirectCallableStrictWeakOrder<const T*, Projected<I, Proj>> Comp = less<>>
  tagged_pair<tag::begin(I), tag::end(I)>
  equal_range(I first, S last, const T& value,
              Comp&& comp_ = Comp{}, Proj&& proj_ = Proj{}) {
    auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
    auto lb = __stl2::lower_bound(__stl2::move(first), last, value,
                                  __stl2::ref(comp), __stl2::ref(proj));
    auto ub = __stl2::upper_bound(lb, __stl2::move(last), value,
                                  __stl2::ref(comp), __stl2::ref(proj));
    return {__stl2::move(lb), __stl2::move(ub)};
  }

  template <ForwardRange Rng, class T, class Proj = identity,
            IndirectCallableStrictWeakOrder<const T*,
              Projected<IteratorType<Rng>, Proj>> Comp = less<>>
  tagged_pair<tag::begin(safe_iterator_t<Rng>), tag::end(safe_iterator_t<Rng>)>
  equal_range(Rng&& rng, const T& value, Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
    return __stl2::equal_range(__stl2::begin(rng), __stl2::end(rng), value,
      __stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
