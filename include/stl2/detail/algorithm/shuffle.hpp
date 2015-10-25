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
#ifndef STL2_DETAIL_ALGORITHM_SHUFFLE_HPP
#define STL2_DETAIL_ALGORITHM_SHUFFLE_HPP

#include <stl2/iterator.hpp>
#include <stl2/random.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/core.hpp>

///////////////////////////////////////////////////////////////////////////
// copy [alg.copy]
//
STL2_OPEN_NAMESPACE {
  template <RandomAccessIterator I, Sentinel<I> S, class Gen>
    requires Permutable<I>() &&
      ConvertibleTo<result_of_t<Gen&()>, difference_type_t<I>>() &&
      UniformRandomNumberGenerator<remove_reference_t<Gen>>()
  I shuffle(I first, S last_, Gen&& g) {
    I last = __stl2::next(first, last_), orig = last;
    difference_type_t<I> d = last - first;
    if (d > 1) {
      using param_t = typename uniform_int_distribution<difference_type_t<I>>::param_type;
      uniform_int_distribution<difference_type_t<I>> uid;
      for (--last, --d; first < last; ++first, --d) {
        auto i = uid(g, param_t{0, d});
        if (i != 0) {
          __stl2::iter_swap(first, first + i);
        }
      }
    }
    return orig;
  }

  template <RandomAccessRange Rng, class Gen>
    requires Permutable<iterator_t<Rng>>() &&
      ConvertibleTo<result_of_t<Gen&()>, difference_type_t<iterator_t<Rng>>>() &&
      UniformRandomNumberGenerator<remove_reference_t<Gen>>()
  safe_iterator_t<Rng> shuffle(Rng&& rng, Gen&& g) {
    return  __stl2::shuffle(__stl2::begin(rng), __stl2::end(rng),
                            __stl2::forward<Gen>(g));
  }
} STL2_CLOSE_NAMESPACE

#endif
