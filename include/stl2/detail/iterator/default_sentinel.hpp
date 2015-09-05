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
