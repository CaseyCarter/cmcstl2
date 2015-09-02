#ifndef STL2_DETAIL_SWAP_HPP
#define STL2_DETAIL_SWAP_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

namespace stl2 { inline namespace v1 {
// exchange
template <Movable T, class U = T>
  requires Assignable<T&, U>()
constexpr T exchange(T& t, U&& u)
  noexcept(is_nothrow_move_constructible<T>::value &&
           is_nothrow_assignable<T&, U>::value) {
  T tmp(stl2::move(t));
  t = stl2::forward<U>(u);
  return tmp;
}

// swap
namespace __swap {
// http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-active.html#2152
// http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-closed.html#2171

// Extension: less specialized than overloads on different Movable types.
template <Movable T, Movable U>
  requires Same<T, U>()
constexpr void swap(T& a, U& b)
  noexcept(noexcept(b = stl2::exchange(a, stl2::move(b)))) {
  b = stl2::exchange(a, stl2::move(b));
}

struct __try_swap_fn {
  template <class T, class U>
    requires requires (T& x, T& y) { swap(x, y); }
  void operator()(T& t, U& u) const
    noexcept(noexcept(swap(t,u)));

  template <class T, class U, std::size_t N, typename This = __try_swap_fn>
    requires requires (T& x, U& y) { This{}(x, y); }
  void operator()(T (&t)[N], U (&u)[N]) const
    noexcept(noexcept(This{}(*t, *u)));
};
// Workaround GCC PR66957 by declaring this unnamed namespace inline.
inline namespace {
  constexpr auto& __try_swap = detail::static_const<__try_swap_fn>::value;
}

// 20150715: Conforming extension: can swap T(&)[N] with U(&)[N]
// if T& and U& are Swappable.
template <class T, class U, std::size_t N>
  requires requires (T &x, U &y) { __try_swap(x, y); }
constexpr void swap(T (&t)[N], U (&u)[N])
  noexcept(noexcept(__try_swap(*t, *u)));

struct fn {
  template <class T, class U>
    requires requires (T&& a, U&& b) { swap((T&&)a, (U&&)b); }
  constexpr void operator()(T&& a, U&& b) const
    noexcept(noexcept(swap(stl2::forward<T>(a), stl2::forward<U>(b)))) {
    swap(stl2::forward<T>(a), stl2::forward<U>(b));
  }
};
} // namespace __swap
// Workaround GCC PR66957 by declaring this unnamed namespace inline.
inline namespace {
  // 20150805: Not to spec: swap is a N4381-style function object customization point.
  constexpr auto& swap = detail::static_const<__swap::fn>::value;
}

namespace __swap {
template <class T, class U, std::size_t N>
  requires requires (T &x, U &y) { __try_swap(x, y); }
constexpr void swap(T (&t)[N], U (&u)[N])
  noexcept(noexcept(__try_swap(*t, *u))) {
  for (std::size_t i = 0; i < N; ++i) {
    stl2::swap(t[i], u[i]);
  }
}
}

namespace detail {
template <class T, class U>
concept bool Swappable_ =
  requires (T&& t, U&&u) {
    stl2::swap((T&&)t, (U&&)u);
  };
}

template <class T>
concept bool Swappable() {
  return detail::Swappable_<T, T>;
}

template <class T, class U>
concept bool Swappable() {
  return Swappable<T>() &&
    Swappable<U>() &&
    CommonReference<const T&, const U&>() &&
    detail::Swappable_<T, U> &&
    detail::Swappable_<U, T>;
}

template <class T, class U>
constexpr bool is_nothrow_swappable_v = false;

Swappable{T, U}
constexpr bool is_nothrow_swappable_v<T, U> =
  noexcept(stl2::swap(stl2::declval<T>(), stl2::declval<U>())) &&
  noexcept(stl2::swap(stl2::declval<U>(), stl2::declval<T>()));

template <class T, class U>
using is_nothrow_swappable_t =
  meta::bool_<is_nothrow_swappable_v<T, U>>;

template <class T, class U>
struct is_nothrow_swappable :
  is_nothrow_swappable_t<T, U> {};

namespace ext { namespace models {
template <class>
constexpr bool swappable() { return false; }
Swappable{T}
constexpr bool swappable() { return true; }
template <class, class>
constexpr bool swappable() { return false; }
Swappable{T, U}
constexpr bool swappable() { return true; }
}}}} // namespace stl2::v1::ext::models

#endif
