#ifndef STL2_ITERATOR_HPP
#define STL2_ITERATOR_HPP

#include <cassert>
#include <initializer_list>
#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/common_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>
#include <stl2/detail/iterator/insert_iterators.hpp>
#include <stl2/detail/iterator/istream_iterator.hpp>
#include <stl2/detail/iterator/move_iterator.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/primitives.hpp>

namespace stl2 { inline namespace v1 {
using std::iterator;

template <CopyConstructible T>
class dangling {
public:
  constexpr dangling()
    noexcept(is_nothrow_default_constructible<T>::value)
    requires DefaultConstructible<T>()
    : value{}
  { }
  constexpr dangling(T t)
    noexcept(is_nothrow_move_constructible<T>::value)
    : value(stl2::move(t))
  { }
  constexpr T get_unsafe() const&
    noexcept(is_nothrow_copy_constructible<T>::value) {
    return value;
  }
  constexpr T get_unsafe() &&
    noexcept(is_nothrow_move_constructible<T>::value) {
    return stl2::move(value);
  }
private:
  T value;
};

template <Range R>
using safe_iterator_t =
  meta::if_<std::is_lvalue_reference<R>, IteratorType<R>, dangling<IteratorType<R>>>;

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
}} // namespace stl2::v1

#endif
