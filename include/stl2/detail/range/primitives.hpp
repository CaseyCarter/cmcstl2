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
#ifndef STL2_DETAIL_RANGE_PRIMITIVES_HPP
#define STL2_DETAIL_RANGE_PRIMITIVES_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/dangling.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// Range primitives [range.primitives]
//
STL2_OPEN_NAMESPACE {
  // enumerate
  namespace ext {
    Range{R}
    pair<DifferenceType<IteratorType<R>>, safe_iterator_t<R>>
    enumerate(R&& r) {
      return __stl2::ext::enumerate(__stl2::begin(r), __stl2::end(r));
    }

    SizedRange{R}
    pair<DifferenceType<IteratorType<R>>, safe_iterator_t<R>>
    enumerate(R&& r) {
      return {__stl2::size(r), __stl2::next(__stl2::begin(r), __stl2::end(r))};
    }
  }

  // distance
  Range{R}
  DifferenceType<R> distance(R&& r) {
    return __stl2::distance(__stl2::begin(r), __stl2::end(r));
  }

  SizedRange{R}
  DifferenceType<R> distance(R&& r) {
    return __stl2::size(r);
  }
} STL2_CLOSE_NAMESPACE

#endif
