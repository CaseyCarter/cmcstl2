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
#ifndef STL2_DETAIL_ALGORITHM_ROTATE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_ROTATE_COPY_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// rotate_copy [alg.rotate]
//
STL2_OPEN_NAMESPACE {
  template <ForwardIterator I, Sentinel<I> S, WeaklyIncrementable O>
    requires IndirectlyCopyable<I, O>()
  tagged_pair<tag::in(I), tag::out(O)>
  rotate_copy(I first, I middle, S last, O out) {
    auto res = __stl2::copy(middle, __stl2::move(last), __stl2::move(out));
    res.out() = __stl2::copy(__stl2::move(first), __stl2::move(middle),
                             __stl2::move(res.out())).out();
    return res;
  }

  template <ForwardRange Rng, WeaklyIncrementable O>
    requires IndirectlyCopyable<IteratorType<Rng>, O>()
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
  rotate_copy(Rng&& rng, IteratorType<Rng> middle, O result) {
    return __stl2::rotate_copy(__stl2::begin(rng), __stl2::move(middle),
                               __stl2::end(rng), __stl2::move(result));
  }
} STL2_CLOSE_NAMESPACE

#endif
