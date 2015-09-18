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
#ifndef STL2_DETAIL_ALGORITHM_MOVE_HPP
#define STL2_DETAIL_ALGORITHM_MOVE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>

///////////////////////////////////////////////////////////////////////////
// move [alg.move]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O>
    requires IndirectlyMovable<I, O>()
  tagged_pair<tag::in(I), tag::out(O)>
  move(I first, S last, O result) {
    for (; first != last; ++first, ++result) {
      *result = __stl2::iter_move(first);
    }
    return {first, result};
  }

  template<InputRange Rng, WeaklyIncrementable O>
    requires IndirectlyMovable<IteratorType<Rng>, O>()
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
  move(Rng&& rng, O result) {
    return __stl2::move(__stl2::begin(rng), __stl2::end(rng), __stl2::move(result));
  }
} STL2_CLOSE_NAMESPACE

#endif
