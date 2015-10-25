// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2013-2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_COPY_BACKWARD_HPP
#define STL2_DETAIL_ALGORITHM_COPY_BACKWARD_HPP

#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// copy_backward [alg.copy]
//
STL2_OPEN_NAMESPACE {
  template <BidirectionalIterator I1, Sentinel<I1> S1, BidirectionalIterator I2>
    requires IndirectlyCopyable<I1, I2>()
  tagged_pair<tag::in(I1), tag::out(I2)>
  copy_backward(I1 first, S1 sent, I2 out) {
    auto last = __stl2::next(first, __stl2::move(sent));
    auto i = last;
    while (i != first) {
      *--out = *--i;
    }
    return {__stl2::move(last), __stl2::move(out)};
  }

  template<BidirectionalRange Rng, BidirectionalIterator I>
    requires IndirectlyCopyable<iterator_t<Rng>, I>()
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(I)>
  copy_backward(Rng&& rng, I result) {
    return __stl2::copy_backward(
      __stl2::begin(rng), __stl2::end(rng), __stl2::move(result));
  }
} STL2_CLOSE_NAMESPACE

#endif
