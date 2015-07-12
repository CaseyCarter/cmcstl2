#ifndef STL2_CONCEPTS_OBJECT_HPP
#define STL2_CONCEPTS_OBJECT_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/compare.hpp>
#include <stl2/concepts/core.hpp>

#include <type_traits>
#include <utility>

////////////////////////////////////////
// Object Concepts [concepts.lib.object]
//
namespace stl2 { inline namespace v1 {

template <class T>
concept bool Destructible() {
  return std::is_object<T>::value &&
    requires(T& t, T* p) {
      { t.~T() } noexcept;
      &t;
      requires Same<T*, decltype(&t)>();
      delete p;
      delete[] p;
    };
}

template <class T, class...Args>
concept bool Constructible() {
  return Destructible<T>() &&
    requires(Args&&...args) {
      T{ stl2::forward<Args>(args)... };
      new T{ stl2::forward<Args>(args)... };
    };
}

template <class T>
concept bool MoveConstructible() {
  return Constructible<T, T&&>();
}

template <class T>
concept bool CopyConstructible() {
  return MoveConstructible<T>() &&
    Constructible<T, T&>() &&
    Constructible<T, const T&>() &&
    Constructible<T, const T&&>();
}

template <class T>
concept bool Movable() {
  return MoveConstructible<T>() &&
    Assignable<T&, T&&>();
}

template <class T>
concept bool Copyable() {
  return Movable<T>() &&
    CopyConstructible<T>() &&
    Assignable<T&, T&>() &&
    Assignable<T&, const T&>() &&
    Assignable<T&, const T&&>();
}

template <class T>
concept bool Semiregular() {
  return Copyable<T>() &&
  Constructible<T>() &&
  requires (const std::size_t n) {
    new T[n];
    requires Same<T*, decltype(new T[n])>();
  };
}

template <Movable T, class U = T>
  requires Assignable<T&, U>()
constexpr T exchange(T& t, U&& u)
  noexcept(std::is_nothrow_move_constructible<T>::value &&
           std::is_nothrow_assignable<T&, U>::value);

constexpr void swap(Movable& a, Movable& b)
  noexcept(noexcept(b = exchange(a, stl2::move(b))));

namespace detail {
  constexpr struct __try_swap_fn {
    template <class T, class U>
    auto operator()(T &t, U &u) const
      noexcept(noexcept(swap(t,u)))
      -> decltype(swap(t,u));

    template <class T, class U, std::size_t N,
      typename This = __try_swap_fn>
    auto operator()(T (&t)[N], U (&u)[N]) const
      noexcept(noexcept(This{}(*t, *u)))
      -> decltype(This{}(*t, *u));
  } __try_swap{};
}

template <class T, class U, std::size_t N>
  requires requires (T &t, U &u) { detail::__try_swap(t, u); }
constexpr void swap(T (&t)[N], U (&u)[N])
  noexcept(noexcept(detail::__try_swap(*t, *u)));

template <class T, class U = T>
concept bool Swappable() {
  return requires (T&& t, U&& u) {
    swap(stl2::forward<T>(t), stl2::forward<U>(u));
    swap(stl2::forward<U>(u), stl2::forward<T>(t));
  };
}

template <class T>
concept bool Regular() {
  return Semiregular<T>() &&
    EqualityComparable<T>();
}

#if 0
template <class T>
concept bool Scalar() {
  return std::is_scalar<T>::value && Regular<T>();
}

template <class T>
concept bool Arithmetic() {
  return std::is_arithmetic<T>::value && Scalar<T>() && TotallyOrdered<T>();
}

template <class T>
concept bool Integral() {
  return std::is_integral<T>::value && Arithmetic<T>();
}

#else

template <class T>
concept bool Integral() {
  return std::is_integral<T>::value;
}
#endif

template <class T>
concept bool SignedIntegral() {
  return Integral<T>() && (T(-1) < T(0));
}

template <class T>
concept bool UnsignedIntegral() {
  return Integral<T>() && !SignedIntegral<T>();
}

// Integral<T> subsumes SignedIntegral<T> and UnsignedIntegral<T>
// SignedIntegral<T> and UnsignedIntegral<T> are mutually exclusive

namespace ext { namespace models {

template <class>
constexpr bool destructible() { return false; }
Destructible{T}
constexpr bool destructible() { return true; }

template <class, class...>
constexpr bool constructible() { return false; }
Constructible{T, ...Args}
constexpr bool constructible() { return false; }

template <class>
constexpr bool move_constructible() { return false; }
MoveConstructible{T}
constexpr bool move_constructible() { return true; }

template <class>
constexpr bool copy_constructible() { return false; }
CopyConstructible{T}
constexpr bool copy_constructible() { return true; }

template <class>
constexpr bool movable() { return false; }
Movable{T}
constexpr bool movable() { return true; }

template <class>
constexpr bool copyable() { return false; }
Copyable{T}
constexpr bool copyable() { return true; }

template <class>
constexpr bool swappable() { return false; }
template <class T>
  requires Swappable<T>()
constexpr bool swappable() { return true; }

template <class, class>
constexpr bool swappable() { return false; }
Swappable{T, U}
constexpr bool swappable() { return true; }

template <class>
constexpr bool semiregular() { return false; }
Semiregular{T}
constexpr bool semiregular() { return true; }

template <class>
constexpr bool regular() { return false; }
Regular{T}
constexpr bool regular() { return true; }

template <class>
constexpr bool integral() { return false; }
Integral{T}
constexpr bool integral() { return true; }

}}}} // namespace stl2::v1::ext::models

#endif // STL2_CONCEPTS_OBJECT_HPP
