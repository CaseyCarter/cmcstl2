#ifndef STL2_DETAIL_CONSTRUCT_DESTRUCT_HPP
#define STL2_DETAIL_CONSTRUCT_DESTRUCT_HPP

#include <memory>
#include <new>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>

namespace stl2 { inline namespace v1 { namespace detail {
struct destruct_fn {
  Destructible{T}
  void operator()(T& t) const noexcept {
    t.~T();
  }

  template <Destructible T, std::size_t N>
  void operator()(T (&a)[N]) const noexcept {
    std::size_t i = N;
    while (i > 0) {
      a[--i].~T();
    }
  }
};
namespace {
  constexpr auto& destruct = detail::static_const<destruct_fn>::value;
}

struct construct_fn {
  Constructible{T, ...Args}
  void operator()(T& t, Args&&...args) const
    noexcept(is_nothrow_constructible<T, Args...>::value) {
    ::new(static_cast<void*>(std::addressof(t)))
      T{std::forward<Args>(args)...};
  }
};
namespace {
  constexpr auto& construct = detail::static_const<construct_fn>::value;
}
}}} // namespace stl2::v1::detail

#endif
