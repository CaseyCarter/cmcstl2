#ifndef STL2_CONCEPTS_FOUNDATIONAL_HPP
#define STL2_CONCEPTS_FOUNDATIONAL_HPP

#include <stl2/detail/config.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/core.hpp>

#include <type_traits>

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

template <class T>
concept bool Semiregular =
  Copyable<T> &&
  DefaultConstructible<T>;

} // namespace concepts

template <Movable T>
constexpr void swap(T& a, T& b)
  noexcept(std::is_nothrow_move_constructible<T>::value &&
           std::is_nothrow_move_assignable<T>::value);

namespace detail {

template <class, class>
struct swappable_array :
  std::false_type {};

template <class T, class U>
  requires requires(T& t, U&u) {
    swap(t, u);
    swap(u, t);
  }
struct swappable_array<T, U> {
  static constexpr bool value = true;
  static constexpr bool nothrow =
    noexcept(swap(stl2::declval<T&>(),
                  stl2::declval<U&>()));
};

template <class T, class U, std::size_t N>
struct swappable_array<T[N], U[N]> :
  swappable_array<T, U> {};

} // namespace detail

template <class T, class U, std::size_t N>
  requires detail::swappable_array<T, U>::value
constexpr void swap(T (&t)[N], U (&u)[N])
  noexcept(detail::swappable_array<T, U>::nothrow);

#ifdef STL2_SWAPPABLE_POINTERS
namespace detail {

template <class T, class U>
concept bool SwappableLvalue =
  requires(T& t, U& u) {
    swap(t, u);
    swap(u, t);
  };

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
#endif // STL2_SWAPPABLE_POINTERS

namespace concepts {

template <class T, class U = T>
concept bool Swappable =
  requires (T&& t, U&& u) {
    swap(forward<T>(t), forward<U>(u));
    swap(forward<U>(u), forward<T>(t));
  };

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
concept bool EqualityComparable_ =
  requires(T&& t, U&& u) {
    { forward<T>(t) == forward<U>(u) } -> Boolean;
    { forward<T>(t) != forward<U>(u) } -> Boolean;
  };

} // namespace detail

namespace concepts {

template <class T, class U = T>
concept bool EqualityComparable =
  detail::EqualityComparable_<T, T> &&
  (Same<T, U> ||
    (detail::EqualityComparable_<T, U> &&
     detail::EqualityComparable_<U, T> &&
     detail::EqualityComparable_<U, U>));

template <class T, class U = T>
concept bool StronglyEqualityComparable =
  EqualityComparable<T, U> &&
  (Same<T, U> ||
    (Common<T, U> &&
     EqualityComparable<CommonType<T, U>>));

template <class T>
concept bool Regular =
  Semiregular<T> &&
  EqualityComparable<T>;

} // namespace concepts

namespace detail {

template <class T, class U>
concept bool TotallyOrdered_ =
  EqualityComparable<T, U> &&
  requires(T&& a, U&& b) {
    { a < b } -> Boolean;
    { a > b } -> Boolean;
    { a <= b } -> Boolean;
    { a >= b } -> Boolean;
  };

} // namespace detail

namespace concepts {

template <class T, class U = T>
concept bool TotallyOrdered =
  detail::TotallyOrdered_<T, T> &&
  (Same<T, U> ||
    (detail::TotallyOrdered_<T, U> &&
     detail::TotallyOrdered_<U, T> &&
     detail::TotallyOrdered_<U, U>));

template <class T, class U = T>
concept bool StronglyTotallyOrdered =
  TotallyOrdered<T, U> &&
  (Same<T, U> ||
    (Common<T, U> &&
     TotallyOrdered<CommonType<T, U>>));

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

namespace models {

template <class>
constexpr bool movable() { return false; }

template <Movable>
constexpr bool movable() { return true; }


template <class>
constexpr bool copyable() { return false; }

template <Copyable>
constexpr bool copyable() { return true; }


template <class>
constexpr bool swappable() { return false; }

template <class T>
  requires Swappable<T>
constexpr bool swappable() { return true; }

template <class, class>
constexpr bool swappable() { return false; }

template <class T, Swappable<T> U>
constexpr bool swappable() { return true; }


template <class>
constexpr bool equality_comparable() { return false; }

template <class T>
  requires EqualityComparable<T>
constexpr bool equality_comparable() { return true; }

template <class, class>
constexpr bool equality_comparable() { return false; }

template <class T, class U>
  requires EqualityComparable<T, U>
constexpr bool equality_comparable() { return true; }


template <class>
constexpr bool semiregular() { return false; }

template <Semiregular>
constexpr bool semiregular() { return true; }


template <class>
constexpr bool regular() { return false; }

template <Regular>
constexpr bool regular() { return true; }


template <class>
constexpr bool boolean() { return false; }

template <Boolean>
constexpr bool boolean() { return true; }


template <class>
constexpr bool integral() { return false; }

template <Integral>
constexpr bool integral() { return true; }

}}}} // namespace stl2::v1::concepts::models

#endif // STL2_CONCEPTS_FOUNDATIONAL_HPP
