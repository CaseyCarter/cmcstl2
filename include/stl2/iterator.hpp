// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_ITERATOR_HPP
#define STL2_ITERATOR_HPP

#include <initializer_list>

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/common_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>
#include <stl2/detail/iterator/insert_iterators.hpp>
#include <stl2/detail/iterator/istream_iterator.hpp>
#include <stl2/detail/iterator/istreambuf_iterator.hpp>
#include <stl2/detail/iterator/ostreambuf_iterator.hpp>
#include <stl2/detail/iterator/move_iterator.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/iterator/ostream_iterator.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/primitives.hpp>
#include <stl2/detail/range/range.hpp>

STL2_OPEN_NAMESPACE {
  using std::iterator;

  // Extension: unreachable is WeaklyTotallyOrdered with *any* type.
  class unreachable {};

  constexpr bool operator==(const auto&, unreachable) noexcept {
    return false;
  }
  constexpr bool operator==(unreachable, const auto&) noexcept {
    return false;
  }
  constexpr bool operator==(unreachable, unreachable) noexcept {
    return true;
  }

  constexpr bool operator!=(const auto&, unreachable) noexcept {
    return true;
  }
  constexpr bool operator!=(unreachable, const auto&) noexcept {
    return true;
  }
  constexpr bool operator!=(unreachable, unreachable) noexcept {
    return false;
  }

  constexpr bool operator<(const auto&, unreachable) noexcept {
    return true;
  }
  constexpr bool operator<(unreachable, const auto&) noexcept {
    return false;
  }
  constexpr bool operator<(unreachable, unreachable) noexcept {
    return false;
  }

  constexpr bool operator>(const auto&, unreachable) noexcept {
    return false;
  }
  constexpr bool operator>(unreachable, const auto&) noexcept {
    return true;
  }
  constexpr bool operator>(unreachable, unreachable) noexcept {
    return false;
  }

  constexpr bool operator<=(const auto&, unreachable) noexcept {
    return true;
  }
  constexpr bool operator<=(unreachable, const auto&) noexcept {
    return false;
  }
  constexpr bool operator<=(unreachable, unreachable) noexcept {
    return true;
  }

  constexpr bool operator>=(const auto&, unreachable) noexcept {
    return false;
  }
  constexpr bool operator>=(unreachable, const auto&) noexcept {
    return true;
  }
  constexpr bool operator>=(unreachable, unreachable) noexcept {
    return true;
  }
} STL2_CLOSE_NAMESPACE

#endif
