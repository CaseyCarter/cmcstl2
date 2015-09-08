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
#ifndef STL2_DETAIL_ITERATOR_DEFAULT_SENTINEL_HPP
#define STL2_DETAIL_ITERATOR_DEFAULT_SENTINEL_HPP

#include <cstddef>
#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
  struct default_sentinel {};

  constexpr bool operator==(default_sentinel, default_sentinel) noexcept {
    return true;
  }
  constexpr bool operator!=(default_sentinel, default_sentinel) noexcept {
    return false;
  }

  constexpr bool operator<(default_sentinel, default_sentinel) noexcept {
    return false;
  }
  constexpr bool operator>(default_sentinel, default_sentinel) noexcept {
    return false;
  }
  constexpr bool operator<=(default_sentinel, default_sentinel) noexcept {
    return true;
  }
  constexpr bool operator>=(default_sentinel, default_sentinel) noexcept {
    return true;
  }
  constexpr std::ptrdiff_t
  operator-(default_sentinel, default_sentinel) noexcept {
    return 0;
  }
} STL2_CLOSE_NAMESPACE

#endif
