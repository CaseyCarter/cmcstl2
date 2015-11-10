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
#ifndef STL2_DETAIL_ALGORITHM_REVERSE_MOVE_HPP
#define STL2_DETAIL_ALGORITHM_REVERSE_MOVE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// reverse_move [Extension]
//
STL2_OPEN_NAMESPACE {
  template <BidirectionalIterator I, Sentinel<I> S, WeaklyIncrementable O>
  requires
    models::IndirectlyMovable<I, O>
  tagged_pair<tag::in(I), tag::out(O)>
  reverse_move(I first, S last, O result)
  {
    auto bound = __stl2::next(first, __stl2::move(last));
    for (auto m = bound; m != first; ++result) {
      *result = __stl2::iter_move(--m);
    }
    return {__stl2::move(bound), __stl2::move(result)};
  }

  template <BidirectionalRange Rng, class O>
  requires
    models::WeaklyIncrementable<__f<O>> &&
    models::IndirectlyMovable<iterator_t<Rng>, __f<O>>
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
  reverse_move(Rng&& rng, O&& result)
  {
    return __stl2::reverse_move(__stl2::begin(rng), __stl2::end(rng),
                                __stl2::forward<O>(result));
  }

  // Extension
  template <class E, class O>
  requires
    models::WeaklyIncrementable<__f<O>> &&
    models::IndirectlyMovable<const E*, __f<O>>
  tagged_pair<tag::in(dangling<const E*>), tag::out(__f<O>)>
  reverse_move(std::initializer_list<E>&& rng, O&& result)
  {
    return __stl2::reverse_move(__stl2::begin(rng), __stl2::end(rng),
                                __stl2::forward<O>(result));
  }
} STL2_CLOSE_NAMESPACE

#endif
