#ifndef STL2_UTILITY_HPP
#define STL2_UTILITY_HPP

#include <stl2/detail/config.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/concept/foundational.hpp>

namespace stl2 { inline namespace v1 {

////////////////////
// exchange and swap
//
template <MoveConstructible T, AssignableTo<T> U = T>
constexpr T exchange(T& t, U&& u)
  noexcept(std::is_nothrow_move_constructible<T>::value &&
           std::is_nothrow_assignable<T&, U>::value) {
  auto tmp = move(t);
  t = forward<U>(u);
  return tmp;
}

/*
 * http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-active.html#2152
 * http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-closed.html#2171
 */

constexpr void swap(Movable& a, Movable& b)
  noexcept(noexcept(exchange(a, move(b)))) {
  exchange(b, exchange(a, move(b)));
}


////////////////////
// General Utilities
//
struct identity {
  template <class T>
  constexpr T&& operator()(T&& t) const noexcept {
    return forward<T>(t);
  }
};

template <class T = void>
  requires Same<T, void> || WeaklyEqualityComparable<T>
struct equal_to {
  constexpr auto operator()(const T& a, const T& b) const {
    return a == b;
  }
};

template <>
struct equal_to<void> {
  template <class T, WeaklyEqualityComparable<T> U>
  constexpr auto operator()(const T& t, const U& u) const {
    return t == u;
  }

  using is_transparent = std::true_type;
};

}} // namespace stl2::v1

#endif // STL2_UTILITY_HPP
