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
#ifndef STL2_DETAIL_ALGORITHM_REVERSE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_REVERSE_COPY_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// reverse_copy [alg.reverse]
//
STL2_OPEN_NAMESPACE {
  template <BidirectionalIterator I, Sentinel<I> S, WeaklyIncrementable O>
    requires IndirectlyCopyable<I, O>()
  tagged_pair<tag::in(I), tag::out(O)> reverse_copy(I first, S last, O result) {
    auto bound = __stl2::next(first, __stl2::move(last));
    for (auto m = bound; m != first; ++result) {
      *result = *--m;
    }
    return {__stl2::move(bound), __stl2::move(result)};
  }

  template <BidirectionalRange Rng, WeaklyIncrementable O>
    requires IndirectlyCopyable<IteratorType<Rng>, O>()
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
  reverse_copy(Rng&& rng, O result) {
    return __stl2::reverse_copy(__stl2::begin(rng), __stl2::end(rng),
                                __stl2::move(result));
  }
} STL2_CLOSE_NAMESPACE

#endif
