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
#ifndef STL2_DETAIL_ALGORITHM_PARTIAL_SORT_HPP
#define STL2_DETAIL_ALGORITHM_PARTIAL_SORT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/heap_sift.hpp>
#include <stl2/detail/algorithm/make_heap.hpp>
#include <stl2/detail/algorithm/sort_heap.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// partial_sort [partial.sort]
//
STL2_OPEN_NAMESPACE {
  template <RandomAccessIterator I, Sentinel<I> S, class Comp = less<>,
            class Proj = identity>
  requires
    models::Sortable<I, __f<Comp>, __f<Proj>>
  I partial_sort(I first, I middle, S last,
                 Comp&& comp_ = Comp{}, Proj&& proj_ = Proj{}) {
    auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));

    __stl2::make_heap(first, middle, __stl2::ref(comp), __stl2::ref(proj));
    const auto len = __stl2::distance(first, middle);
    I i = middle;
    for(; i != last; ++i) {
      if(comp(proj(*i), proj(*first))) {
        __stl2::iter_swap(i, first);
        detail::sift_down_n(first, len, first, __stl2::ref(comp), __stl2::ref(proj));
      }
    }
    __stl2::sort_heap(first, middle, __stl2::ref(comp), __stl2::ref(proj));
    return i;
  }

  template <RandomAccessRange Rng, class Comp = less<>, class Proj = identity>
  requires
    models::Sortable<iterator_t<Rng>, __f<Comp>, __f<Proj>>
  safe_iterator_t<Rng>
  partial_sort(Rng&& rng, iterator_t<Rng> middle, Comp&& comp = Comp{},
               Proj&& proj = Proj{}) {
    return __stl2::partial_sort(
      __stl2::begin(rng), __stl2::move(middle), __stl2::end(rng),
      __stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
