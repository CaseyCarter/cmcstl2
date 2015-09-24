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
#ifndef STL2_DETAIL_ALGORITHM_PUSH_HEAP_HPP
#define STL2_DETAIL_ALGORITHM_PUSH_HEAP_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// push_heap [push.heap]
//
STL2_OPEN_NAMESPACE {
  template <RandomAccessIterator I, Sentinel<I> S, class Comp = less<>,
            class Proj = identity>
    requires Sortable<I, Comp, Proj>()
  I push_heap(I first, S last, Comp comp_ = Comp{}, Proj proj_ = Proj{}) {
    auto&& comp = __stl2::as_function(comp_);
    auto&& proj = __stl2::as_function(proj_);
    std::terminate(); // FIXME: NYI
  }

  template <RandomAccessRange Rng, class Comp = less<>, class Proj = identity>
    requires Sortable<IteratorType<Rng>, Comp, Proj>()
  safe_iterator_t<Rng>
  push_heap(Rng&& rng, Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
    return __stl2::push_heap(__stl2::begin(rng), __stl2::end(rng),
                             __stl2::forward<Comp>(comp),
                             __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
