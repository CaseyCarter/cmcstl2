#ifndef STL2_CONCEPTS_FOUNDATIONAL_HPP
#define STL2_CONCEPTS_FOUNDATIONAL_HPP

#include <stl2/detail/config.hpp>
#include <stl2/concepts/core.hpp>

#include <meta/meta.hpp>

#include <cassert>
#include <type_traits>
#include <utility>

////////////////////////
// Foundational Concepts
//
namespace stl2 { namespace v1 { namespace concepts {

template <class T>
concept bool Movable =
  Destructible<T> &&
  MoveConstructible<T> &&
  MoveAssignable<T> &&
  requires(T& t, std::size_t n) {
    &t; requires Same<T*, decltype(&t)>;
    { t.~T() } noexcept;
    new T; requires Same<T*, decltype(new T)>;
    new T[n]; requires Same<T*, decltype(new T[n])>;
    delete new T;
    delete new T[n];
  };

template <class T>
concept bool Copyable =
  Movable<T> &&
  CopyConstructible<T> &&
  CopyAssignable<T>;

} // namespace concepts

template <MoveConstructible T, AssignableTo<T> U = T>
constexpr T exchange(T& t, U&& u)
  noexcept(std::is_nothrow_move_constructible<T>::value &&
           std::is_nothrow_assignable<T&, U>::value);

template <class T>
  requires MoveConstructible<T> && MoveAssignable<T>
constexpr void swap(T& a, T& b)
  noexcept(noexcept(exchange(a, move(b))));

namespace detail {
template <class T, class U>
concept bool SwappableNonarrayLvalue_ =
  requires(T& t, U& u) {
    swap(t, u);
  };

template <class T, class U>
concept bool SwappableNonarrayLvalue =
  SwappableNonarrayLvalue_<T, T> &&
  (Same<T, U> ||
    (SwappableNonarrayLvalue_<U, U> &&
     SwappableNonarrayLvalue_<T, U> &&
     SwappableNonarrayLvalue_<U, T>));

template <class T, class U>
struct same_extents
  : meta::bool_<!(std::is_array<T>::value || std::is_array<U>::value)>
{};

template <class T, class U, std::size_t N>
struct same_extents<T[N], U[N]>
  : same_extents<T, U> {};

template <class T, class U>
concept bool SameExtents =
  same_extents<T, U>::value;

} // namespace detail

template <class T, class U, std::size_t N,
          class TE = std::remove_all_extents_t<T>,
          class UE = std::remove_all_extents_t<U>>
  requires detail::SameExtents<T, U> &&
    detail::SwappableNonarrayLvalue<TE, UE>
constexpr void swap(T (&a)[N], U (&b)[N])
  noexcept(noexcept(swap(declval<TE&>(),
                         declval<UE&>()))) {
  for (std::size_t i = 0; i < N; ++i) {
    swap(a[i], b[i]);
  }
}

#ifdef STL2_SWAPPABLE_POINTERS
namespace detail {

template <class T, class U>
concept bool SwappableLvalue_ =
  requires(T& t, U& u) {
    swap(t, u);
  };

template <class T, class U>
concept bool SwappableLvalue =
  SwappableLvalue_<T, T> &&
  (Same<T, U> ||
   (SwappableLvalue_<U, U> &&
    SwappableLvalue_<T, U> &&
    SwappableLvalue_<U, T>));

} // namespace detail

template <class T, class U>
  requires detail::SwappableLvalue<T, U>
constexpr void swap(T*&& a, U*&& b)
  noexcept(noexcept(swap(*a, *b)));

template <class T, class U>
  requires detail::SwappableLvalue<T, U>
constexpr void swap(T& a, U*&& b)
  noexcept(noexcept(swap(a, *b)));

template <class T, class U>
  requires detail::SwappableLvalue<T, U>
constexpr void swap(T*&& a, U& b)
  noexcept(noexcept(swap(*a, b)));
#endif

namespace detail {
template <class T, class U>
concept bool Swappable_ =
  requires(T&& t, U&& u) {
    swap(forward<T>(t), forward<U>(u));
  };

} // namespace detail

namespace concepts {

template <class T, class U = T>
concept bool Swappable =
  detail::Swappable_<T, T> &&
  (Same<T, U> ||
   (detail::Swappable_<U, U> &&
    detail::Swappable_<T, U> &&
    detail::Swappable_<U, T>));

template <class T>
concept bool Semiregular =
  Copyable<T> &&
  DefaultConstructible<T>;

template <class B>
concept bool Boolean =
  Convertible<B, bool> &&
  requires(B&& b1, B&& b2) {
    //{ !b1 } -> Boolean;
    !b1; requires Convertible<decltype(!b1),bool>;
    //{ b1 && b2 } -> Same<bool>;
    b1 && b2; requires Convertible<decltype(b1 && b2),bool>;
    //{ b1 || b2 } -> Same<bool>;
    b1 || b2; requires Convertible<decltype(b1 || b2),bool>;
  };

} // namespace concepts

namespace detail {

template <class T, class U>
concept bool EqualityComparable =
  requires(T&& t, U&& u) {
    { forward<T>(t) == forward<U>(u) } -> Boolean;
    { forward<T>(t) != forward<U>(u) } -> Boolean;
  };

} // namespace detail

namespace concepts {

template <class T, class U = T>
concept bool WeaklyEqualityComparable =
  detail::EqualityComparable<T, T> &&
  (Same<T, U> ||
    (detail::EqualityComparable<T, U> &&
     detail::EqualityComparable<U, T> &&
     detail::EqualityComparable<U, U>));

template <class T, class U = T>
concept bool EqualityComparable =
  WeaklyEqualityComparable<T, U> &&
  (Same<T, U> ||
    (Common<T, U> &&
     WeaklyEqualityComparable<CommonType<T, U>>));

template <class T>
concept bool Regular =
  Semiregular<T> &&
  EqualityComparable<T>;

} // namespace concepts

namespace detail {

template <class T, class U>
concept bool TotallyOrdered =
  WeaklyEqualityComparable<T, U> &&
  requires(T&& a, U&& b) {
    { a < b } -> Boolean;
    { a > b } -> Boolean;
    { a <= b } -> Boolean;
    { a >= b } -> Boolean;
  };

} // namespace detail

namespace concepts {

template <class T, class U = T>
concept bool WeaklyTotallyOrdered =
  detail::TotallyOrdered<T, T> &&
  (Same<T, U> ||
    (detail::TotallyOrdered<T, U> &&
     detail::TotallyOrdered<U, T> &&
     detail::TotallyOrdered<U, U>));

template <class T, class U = T>
concept bool TotallyOrdered =
  WeaklyTotallyOrdered<T, U> &&
  (Same<T, U> ||
    (Common<T, U> &&
     detail::TotallyOrdered<CommonType<T, U>, CommonType<T, U>>));

#if 0
template <class T>
concept bool Scalar =
  std::is_scalar<T>::value && Regular<T>;

template <class T>
concept bool Arithmetic =
  std::is_arithmetic<T>::value && Scalar<T> && TotallyOrdered<T>;

template <class T>
concept bool Integral =
  std::is_integral<T>::value && Arithmetic<T>;

#else

template <class T>
concept bool Integral =
  std::is_integral<T>::value;
#endif

template <class T>
concept bool SignedIntegral =
  Integral<T> && std::is_signed<T>::value;

template <class T>
concept bool UnsignedIntegral =
  Integral<T> && !SignedIntegral<T>;

// Integral<T> subsumes SignedIntegral<T> and UnsignedIntegral<T>
// SignedIntegral<T> and UnsignedIntegral<T> are mutually exclusive

namespace test {

template <class>
constexpr bool is_movable() { return false; }

template <Movable>
constexpr bool is_movable() { return true; }


template <class>
constexpr bool is_copyable() { return false; }

template <Copyable>
constexpr bool is_copyable() { return true; }


template <class>
constexpr bool is_swappable() { return false; }

template <class T>
  requires Swappable<T>
constexpr bool is_swappable() { return true; }

template <class, class>
constexpr bool is_swappable() { return false; }

template <class T, Swappable<T> U>
constexpr bool is_swappable() { return true; }


template <class>
constexpr bool is_semiregular() { return false; }

template <Semiregular>
constexpr bool is_semiregular() { return true; }


template <class>
constexpr bool is_boolean() { return false; }

template <Boolean>
constexpr bool is_boolean() { return true; }


template <class>
constexpr bool is_integral() { return false; }

template <Integral>
constexpr bool is_integral() { return true; }

}}}} // namespace stl2::v1::concepts::test

#endif // STL2_CONCEPTS_FOUNDATIONAL_HPP
