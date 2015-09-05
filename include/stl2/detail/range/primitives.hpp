#ifndef STL2_DETAIL_RANGE_PRIMITIVES_HPP
#define STL2_DETAIL_RANGE_PRIMITIVES_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// Range primitives [range.primitives]
//
STL2_OPEN_NAMESPACE {
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
