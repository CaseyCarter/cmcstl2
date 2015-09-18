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
#ifndef STL2_DETAIL_RANGE_BOUND_HPP
#define STL2_DETAIL_RANGE_BOUND_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/range/range.hpp>

STL2_OPEN_NAMESPACE {
  namespace ext {
    // If there is a cycle in [i, s), then distance(i, i + (s - i))
    // may be less than (s - i). We need to preserve the number of
    // elements in the range while finding the bound.
    template <ForwardIterator I, Sentinel<I> S>
    range<counted_iterator<I>, counted_iterator<I>>
    bound_range(I first, S last) {
      auto n = DifferenceType<I>{0};
      auto i = first;
      while (i != last) {
        ++i;
        ++n;
      }
      return {{__stl2::move(first), n}, {__stl2::move(i), 0}};
    }


    template <ForwardIterator I, Sentinel<I> S>
      requires SizedIteratorRange<I, S>()
    range<counted_iterator<I>, counted_iterator<I>>
    bound_range(I first, S last) {
      auto n = DifferenceType<I>{last - first};
      return {{first, n}, {__stl2::next(first, n), 0}};
    }

    Iterator{I}
    range<I, I> bound_range(I i, I s) {
      return {__stl2::move(i), __stl2::move(s)};
    }
  }
} STL2_CLOSE_NAMESPACE

#endif
