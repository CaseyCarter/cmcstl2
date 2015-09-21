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
#ifndef STL2_DETAIL_ALGORITHM_REVERSE_HPP
#define STL2_DETAIL_ALGORITHM_REVERSE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// reverse [alg.reverse]
//
STL2_OPEN_NAMESPACE {
  template <BidirectionalIterator I>
    requires Permutable<I>()
  I reverse(I first, I last) {
    auto m = last;
    while (first != m && first != --m) {
      __stl2::iter_swap2(first, m);
      ++first;
    }
    return last;
  }

  template <RandomAccessIterator I>
    requires Permutable<I>()
  I reverse(I first, I last) {
    if (first != last) {
      auto m = last;
      while (first < --m) {
        __stl2::iter_swap2(first, m);
        ++first;
      }
    }
    return last;
  }

  template <BidirectionalIterator I, Sentinel<I> S>
    requires Permutable<I>()
  I reverse(I first, S last) {
    auto bound = __stl2::next(first, __stl2::move(last));
    return __stl2::reverse(__stl2::move(first), __stl2::move(bound));
  }

  template <BidirectionalRange Rng>
    requires Permutable<IteratorType<Rng>>()
  safe_iterator_t<Rng> reverse(Rng&& rng) {
    return __stl2::reverse(__stl2::begin(rng), __stl2::end(rng));
  }
} STL2_CLOSE_NAMESPACE

#endif
