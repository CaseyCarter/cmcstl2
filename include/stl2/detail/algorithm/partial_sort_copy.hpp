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
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// partial_sort_copy [partial.sort.copy]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I1, Sentinel<I1> S1,
            RandomAccessIterator I2, Sentinel<I2> S2,
            class Comp = less<>, class Proj = identity>
    requires IndirectlyCopyable<I1, I2>() && Sortable<I2, Comp, Proj>()
  I2 partial_sort_copy(I1 first, S1 last, I2 result_first, S2 result_last,
                       Comp comp = Comp{}, Proj proj = Proj{}) {
    std::terminate(); // FIXME: NYI
  }

  template <InputRange Rng1, RandomAccessRange Rng2,
            class Comp = less<>, class Proj = identity>
    requires IndirectlyCopyable<IteratorType<Rng1>, IteratorType<Rng2>>() &&
      Sortable<IteratorType<Rng2>, Comp, Proj>()
  safe_iterator_t<Rng2>
  partial_sort_copy(Rng1&& rng, Rng2&& result_rng, Comp&& comp = Comp{},
                    Proj&& proj = Proj{}) {
    return __stl2::partial_sort_copy(
      __stl2::begin(rng), __stl2::end(rng),
      __stl2::begin(result_rng), __stl2::end(result_rng),
      __stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
