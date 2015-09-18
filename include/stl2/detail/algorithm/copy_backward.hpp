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
#ifndef STL2_DETAIL_ALGORITHM_COPY_BACKWARD_HPP
#define STL2_DETAIL_ALGORITHM_COPY_BACKWARD_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/range/bound.hpp>

///////////////////////////////////////////////////////////////////////////
// copy_backward [alg.copy]
//
STL2_OPEN_NAMESPACE {
  template <BidirectionalIterator I1, BidirectionalIterator I2>
    requires IndirectlyCopyable<I1, I2>()
  I2 __copy_backward(I1 first, I1 last, I2 out) {
    while (last != first) {
      *--out = *--last;
    }
    return out;
  }

  template <BidirectionalIterator I1, BidirectionalIterator I2>
    requires IndirectlyCopyable<I1, I2>()
  tagged_pair<tag::in(I1), tag::out(I2)>
  copy_backward(I1 first, I1 last, I2 out) {
    auto result = __stl2::__copy_backward(
      __stl2::move(first), last, __stl2::move(out));
    return {__stl2::move(last), __stl2::move(result)};
  }

  template <BidirectionalIterator I1, Sentinel<I1> S1, BidirectionalIterator I2>
    requires IndirectlyCopyable<I1, I2>()
  tagged_pair<tag::in(I1), tag::out(I2)>
  copy_backward(I1 first, S1 last, I2 out) {
    auto rng = __stl2::ext::bound_range(__stl2::move(first), __stl2::move(last));
    auto result = __stl2::__copy_backward(
      __stl2::begin(rng), __stl2::end(rng), __stl2::move(out));
    return {__stl2::ext::uncounted(__stl2::end(rng)), __stl2::move(result)};
  }

  template<BidirectionalRange Rng, BidirectionalIterator I>
    requires IndirectlyCopyable<IteratorType<Rng>, I>()
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(I)>
  copy_backward(Rng&& rng, I result) {
    return __stl2::copy_backward(
      __stl2::begin(rng), __stl2::end(rng), __stl2::move(result));
  }

  template<BidirectionalRange Rng, BidirectionalIterator I>
    requires IndirectlyCopyable<IteratorType<Rng>, I>() &&
      SizedRange<Rng>() && !BoundedRange<Rng>()
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(I)>
  copy_backward(Rng&& rng, I out) {
    auto n = __stl2::size(rng);
    auto first = __stl2::begin(rng);
    auto last = __stl2::next(first, n);
    using CI = counted_iterator<IteratorType<Rng>>;
    auto result = __stl2::__copy_backward(
      CI{__stl2::move(first), n}, CI{last, 0}, __stl2::move(out));
    return {__stl2::move(last), __stl2::move(result)};
  }
} STL2_CLOSE_NAMESPACE

#endif
