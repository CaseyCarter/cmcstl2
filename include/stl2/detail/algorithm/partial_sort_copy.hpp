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
#ifndef STL2_DETAIL_ALGORITHM_PARTIAL_SORT_COPY_HPP
#define STL2_DETAIL_ALGORITHM_PARTIAL_SORT_COPY_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/tuple.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/heap_sift.hpp>
#include <stl2/detail/algorithm/make_heap.hpp>
#include <stl2/detail/algorithm/sort_heap.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// partial_sort_copy [partial.sort.copy]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I1, Sentinel<I1> S1,
            RandomAccessIterator I2, Sentinel<I2> S2,
            class Proj1 = identity, class Proj2 = identity,
            IndirectCallableStrictWeakOrder<
              projected<I1, Proj1>, projected<I2, Proj2>> Comp = less<>>
    requires IndirectlyCopyable<I1, I2>() && Sortable<I2, Comp, Proj2>()
  I2 partial_sort_copy(I1 first, S1 last, I2 result_first, S2 result_last,
                       Comp&& comp_ = Comp{}, Proj1&& proj1_ = Proj1{},
                       Proj2&& proj2_ = Proj2{})
  {
    auto r = result_first;
    if(r != result_last) {
      auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
      auto proj1 = ext::make_callable_wrapper(__stl2::forward<Proj1>(proj1_));
      auto proj2 = ext::make_callable_wrapper(__stl2::forward<Proj2>(proj2_));

      for(; first != last && r != result_last; ++first, ++r) {
        *r = *first;
      }
      __stl2::make_heap(result_first, r, __stl2::ref(comp), __stl2::ref(proj2));
      const auto len = difference_type_t<I2>(r - result_first);
      for(; first != last; ++first) {
        auto&& x = *first;
        if(comp(proj1(x), proj2(*result_first))) {
          *result_first = (decltype(x) &&) x;
          detail::sift_down_n(result_first, len, result_first, __stl2::ref(comp), __stl2::ref(proj2));
        }
      }
      __stl2::sort_heap(result_first, r, __stl2::ref(comp), __stl2::ref(proj2));
    }
    return r;
  }

  template <InputRange Rng1, RandomAccessRange Rng2,
            class Proj1 = identity, class Proj2 = identity,
            IndirectCallableStrictWeakOrder<
              projected<iterator_t<Rng1>, Proj1>,
              projected<iterator_t<Rng2>, Proj2>> Comp = less<>>
    requires IndirectlyCopyable<iterator_t<Rng1>, iterator_t<Rng2>>() &&
      Sortable<iterator_t<Rng2>, Comp, Proj2>()
  safe_iterator_t<Rng2>
  partial_sort_copy(Rng1&& rng, Rng2&& result_rng, Comp&& comp = Comp{},
                    Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    return __stl2::partial_sort_copy(
      __stl2::begin(rng), __stl2::end(rng),
      __stl2::begin(result_rng), __stl2::end(result_rng),
      __stl2::forward<Comp>(comp),
      __stl2::forward<Proj1>(proj1), __stl2::forward<Proj2>(proj2));
  }
} STL2_CLOSE_NAMESPACE

#endif
