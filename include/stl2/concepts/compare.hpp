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
    requires(B&& b1, B&& b2) {
      // { !b1 } -> Boolean;
      !b1; requires Convertible<decltype(!b1),bool>();
      //{ b1 && b2 } -> Same<bool>();
      b1 && b2; requires Convertible<decltype(b1 && b2),bool>();
      //{ b1 || b2 } -> Same<bool>();
      b1 || b2; requires Convertible<decltype(b1 || b2),bool>();
    };
}

namespace detail {

template <class T, class U>
concept bool EqualityComparable_ =
  requires (T&& t, U&& u) {
#if 0 // FIXME: ICE
    { stl2::forward<T>(t) == stl2::forward<U>(u) } -> Boolean;
    { stl2::forward<T>(t) != stl2::forward<U>(u) } -> Boolean;
#else
    { stl2::forward<T>(t) == stl2::forward<U>(u) } -> bool;
    { stl2::forward<T>(t) != stl2::forward<U>(u) } -> bool;
#endif
  };

} // namespace detail

template <class T, class U = T>
concept bool EqualityComparable() {
  return detail::EqualityComparable_<T, T> &&
  (Same<T, U>() ||
    (detail::EqualityComparable_<T, U> &&
     detail::EqualityComparable_<U, T> &&
     detail::EqualityComparable_<U, U>));
}

template <class T, class U = T>
concept bool StronglyEqualityComparable() {
  return EqualityComparable<T, U>() &&
    (Same<T, U>() ||
      (Common<T, U>() &&
       EqualityComparable<CommonType<T, U>>()));
}

namespace detail {

template <class T, class U>
concept bool TotallyOrdered_ =
  EqualityComparable<T, U>() &&
  requires (T&& a, U&& b) {
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

template <class T, class U = T>
concept bool TotallyOrdered() {
  return detail::TotallyOrdered_<T, T> &&
    (Same<T, U>() ||
      (detail::TotallyOrdered_<T, U> &&
       detail::TotallyOrdered_<U, T> &&
       detail::TotallyOrdered_<U, U>));
}

template <class T, class U = T>
concept bool StronglyTotallyOrdered() {
  return TotallyOrdered<T, U>() &&
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
