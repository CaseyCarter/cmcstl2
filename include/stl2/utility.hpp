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


namespace detail { namespace __swap {
// http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-active.html#2152
// http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-closed.html#2171

constexpr void swap(Movable& a, Movable& b)
  noexcept(noexcept(b = stl2::exchange(a, stl2::move(b)))) {
  b = stl2::exchange(a, stl2::move(b));
}

template <class T, class U, std::size_t N>
  requires requires (T &x, U &y) { __try_swap(x, y); }
constexpr void swap(T (&t)[N], U (&u)[N])
  noexcept(noexcept(__try_swap(*t, *u))) {
  for (std::size_t i = 0; i < N; ++i) {
    stl2::swap(t[i], u[i]);
  }
}

template <class T, class U>
  requires requires (T&& a, U&& b) { swap((T&&)a, (U&&)b); }
constexpr void fn::operator()(T&& a, U&& b) const
  noexcept(noexcept(swap(stl2::forward<T>(a), stl2::forward<U>(b)))) {
  swap(stl2::forward<T>(a), stl2::forward<U>(b));
}
}}

template<class T, class U>
constexpr bool is_nothrow_swappable_v = false;

Swappable{T, U}
constexpr bool is_nothrow_swappable_v<T, U> =
  noexcept(swap(declval<T>(), declval<U>()));

template<class T, class U>
using is_nothrow_swappable_t =
  meta::bool_<is_nothrow_swappable_v<T, U>>;

template<class T, class U>
struct is_nothrow_swappable :
  is_nothrow_swappable_t<T, U> {};

}} // namespace stl2::v1

#endif
