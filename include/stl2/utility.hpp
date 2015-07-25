#ifndef STL2_UTILITY // -*- mode: c++ -*-
#define STL2_UTILITY

#include <cassert>
#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

namespace stl2 { inline namespace v1 {

////////////////////
// exchange and swap
//
template <Movable T, class U = T>
  requires Assignable<T&, U>()
constexpr T exchange(T& t, U&& u)
  noexcept(std::is_nothrow_move_constructible<T>::value &&
           std::is_nothrow_assignable<T&, U>::value) {
  T tmp(stl2::move(t));
  t = stl2::forward<U>(u);
  return tmp;
}

/*
 * http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-active.html#2152
 * http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-closed.html#2171
 */

constexpr void swap(Movable& a, Movable& b)
  noexcept(noexcept(b = exchange(a, stl2::move(b)))) {
  b = exchange(a, stl2::move(b));
}

template <class T, class U, std::size_t N>
  requires requires (T &x, U &y) { detail::__try_swap(x, y); }
constexpr void swap(T (&t)[N], U (&u)[N])
  noexcept(noexcept(detail::__try_swap(*t, *u))) {
  for (std::size_t i = 0; i < N; ++i)
    swap(t[i], u[i]);
}

template<class T, class U>
constexpr bool is_nothrow_swappable_v = false;

Swappable{T, U}
constexpr bool is_nothrow_swappable_v =
  noexcept(swap(declval<T>(), declval<U>()));

template<class T, class U>
using is_nothrow_swappable_t =
  meta::bool_<is_nothrow_swappable_v<T, U>>;

template<class T, class U>
struct is_nothrow_swappable :
  is_nothrow_swappable_t<T, U> {};

////////////////////
// General Utilities
//
struct identity {
  template <class T>
  constexpr T&& operator()(T&& t) const noexcept {
    return stl2::forward<T>(t);
  }
};

template <class T = void>
  requires Same<T, void>() || EqualityComparable<T>()
struct equal_to {
  constexpr bool operator()(const T& a, const T& b) const {
    return a == b;
  }

  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template <>
struct equal_to<void> {
  EqualityComparable{T, U}
  constexpr auto operator()(const T& t, const U& u) const ->
    decltype(t == u) {
    return {t == u};
  }

  using is_transparent = std::true_type;
};

}} // namespace stl2::v1

#endif
