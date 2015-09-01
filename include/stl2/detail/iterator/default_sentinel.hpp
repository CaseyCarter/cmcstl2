#ifndef STL2_DETAIL_ITERATOR_DEFAULT_SENTINEL_HPP
#define STL2_DETAIL_ITERATOR_DEFAULT_SENTINEL_HPP

#include <cstddef>
#include <stl2/detail/fwd.hpp>

namespace stl2 {
  inline namespace v1 {
    struct default_sentinel {};

    constexpr bool
    operator==(default_sentinel, default_sentinel) noexcept {
      return true;
    }
    constexpr bool
    operator!=(default_sentinel, default_sentinel) noexcept {
      return false;
    }

    constexpr bool
    operator<(default_sentinel, default_sentinel) noexcept {
      return false;
    }
    constexpr bool
    operator>(default_sentinel, default_sentinel) noexcept {
      return false;
    }
    constexpr bool
    operator<=(default_sentinel, default_sentinel) noexcept {
      return true;
    }
    constexpr bool
    operator>=(default_sentinel, default_sentinel) noexcept {
      return true;
    }
    constexpr std::ptrdiff_t
    operator-(default_sentinel, default_sentinel) noexcept {
      return 0;
    }
  }
}

#endif
