#ifndef STL2_CONCEPTS_COMPARE_HPP
#define STL2_CONCEPTS_COMPARE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/core.hpp>

/////////////////////////////////////////////
// Comparison Concepts [concepts.lib.compare]
//
namespace stl2 { inline namespace v1 {

template <class B>
concept bool Boolean() {
  return Convertible<B, bool>() &&
    requires(const B& b1, const B& b2, const bool a) {
      !b1; requires Convertible<decltype(!b1), bool>();
      b1 && b2; requires Same<decltype(b1 && b2), bool>();
      b1 && a; requires Same<decltype(b1 && a), bool>();
      a && b2; requires Same<decltype(a && b2), bool>();
      b1 || b2; requires Same<decltype(b1 || b2), bool>();
      b1 || a; requires Same<decltype(b1 || a), bool>();
      a || b2; requires Same<decltype(a || b2), bool>();
      b1 == b2; requires Convertible<decltype(b1 == b2), bool>();
      b1 == a; requires Convertible<decltype(b1 == a), bool>();
      a == b2; requires Convertible<decltype(a == b2), bool>();
      b1 != b2; requires Convertible<decltype(b1 != b2), bool>();
      b1 != a; requires Convertible<decltype(b1 != a), bool>();
      a != b2; requires Convertible<decltype(a != b2), bool>();
    };
}

namespace detail {

template <class T, class U>
concept bool EqualityComparable_ =
  requires (const T& t, const U& u) {
#if 0 // FIXME: ICE
    { t == u } -> Boolean;
    { t != u } -> Boolean;
#else
    { t == u } -> bool;
    { t != u } -> bool;
#endif
  };

} // namespace detail

namespace ext {

template <class T>
concept bool WeaklyEqualityComparable() {
  return detail::EqualityComparable_<T, T>;
}

template <class T, class U>
concept bool WeaklyEqualityComparable() {
  return WeaklyEqualityComparable<T>() &&
    WeaklyEqualityComparable<U>() &&
    detail::EqualityComparable_<T, U> &&
    detail::EqualityComparable_<U, T>;
}

} // namespace ext

template <class T>
concept bool EqualityComparable() {
  return ext::WeaklyEqualityComparable<T>();
}

template <class T, class U>
concept bool EqualityComparable() {
  return ext::WeaklyEqualityComparable<T, U>() &&
    (Same<T, U>() ||
      (Common<T, U>() &&
       EqualityComparable<CommonType<T, U>>()));
}

namespace detail {

template <class T, class U>
concept bool TotallyOrdered_ =
  EqualityComparable_<T, U> &&
  requires (const T& a, const U& b) {
#if 0 // FIXME: ICE
    //{ a < b } -> Boolean;
    //{ a > b } -> Boolean;
    //{ a <= b } -> Boolean;
    //{ a >= b } -> Boolean;
#else
    { a < b } -> bool;
    { a > b } -> bool;
    { a <= b } -> bool;
    { a >= b } -> bool;
#endif
  };

} // namespace detail

namespace ext {

template <class T>
concept bool WeaklyTotallyOrdered() {
  return detail::TotallyOrdered_<T, T>;
}

template <class T, class U>
concept bool WeaklyTotallyOrdered() {
  return WeaklyTotallyOrdered<T>() &&
    WeaklyTotallyOrdered<U>() &&
    detail::TotallyOrdered_<T, U> &&
    detail::TotallyOrdered_<U, T>;
}

} // namespace ext

template <class T>
concept bool TotallyOrdered() {
  return ext::WeaklyTotallyOrdered<T>();
}

template <class T, class U>
concept bool TotallyOrdered() {
  return ext::WeaklyTotallyOrdered<T, U>() &&
    (Same<T, U>() ||
      (Common<T, U>() &&
       TotallyOrdered<CommonType<T, U>>()));
}

namespace ext { namespace models {

template <class>
constexpr bool boolean() { return false; }
Boolean{T}
constexpr bool boolean() { return true; }

template <class>
constexpr bool equality_comparable() { return false; }
template <class T>
  requires EqualityComparable<T>()
constexpr bool equality_comparable() { return true; }
template <class, class>
constexpr bool equality_comparable() { return false; }
EqualityComparable{T, U}
constexpr bool equality_comparable() { return true; }

template <class>
constexpr bool totally_ordered() { return false; }
template <class T>
  requires TotallyOrdered<T>()
constexpr bool totally_ordered() { return true; }
template <class, class>
constexpr bool totally_ordered() { return false; }
TotallyOrdered{T, U}
constexpr bool totally_ordered() { return true; }

}}}} // namespace stl2::v1::ext::models

#endif // STL2_CONCEPTS_COMPARE_HPP
