#ifndef STL2_DETAIL_CONCEPTS_COMPARE_HPP
#define STL2_DETAIL_CONCEPTS_COMPARE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/type_traits.hpp>

/////////////////////////////////////////////
// Comparison Concepts [concepts.lib.compare]
//
namespace stl2 { inline namespace v1 {

template <class B>
concept bool Boolean() {
  return requires (const B& b1, const B& b2, const bool a) {
    // Requirements common to both Boolean and BooleanTestable.
    STL2_CONVERSION_CONSTRAINT(b1, bool);
    STL2_CONVERSION_CONSTRAINT(!b1, bool);
    STL2_EXACT_TYPE_CONSTRAINT(b1 && a, bool);
    STL2_EXACT_TYPE_CONSTRAINT(b1 || a, bool);

    // Requirements of Boolean that are also be valid for
    // BooleanTestable, but for which BooleanTestable does not
    // require validation.
    STL2_EXACT_TYPE_CONSTRAINT(b1 && b2, bool);
    STL2_EXACT_TYPE_CONSTRAINT(a && b2, bool);
    STL2_EXACT_TYPE_CONSTRAINT(b1 || b2, bool);
    STL2_EXACT_TYPE_CONSTRAINT(a || b2, bool);

#if 1
    // Requirements of Boolean that are not required by
    // BooleanTestable.
    STL2_CONVERSION_CONSTRAINT(b1 == b2, bool);
    STL2_CONVERSION_CONSTRAINT(b1 == a, bool);
    STL2_CONVERSION_CONSTRAINT(a == b2, bool);
    STL2_CONVERSION_CONSTRAINT(b1 != b2, bool);
    STL2_CONVERSION_CONSTRAINT(b1 != a, bool);
    STL2_CONVERSION_CONSTRAINT(a != b2, bool);
#endif
  };
}

namespace detail {

template <class T, class U>
concept bool EqualityComparable_ =
  requires (const T& t, const U& u) {
    STL2_DEDUCTION_CONSTRAINT(t == u, Boolean);
    STL2_DEDUCTION_CONSTRAINT(t != u, Boolean);
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
    CommonReference<const T&, const U&>() &&
    EqualityComparable<CommonType<T, U>>();
}

namespace detail {

template <class T, class U>
concept bool TotallyOrdered_ =
  EqualityComparable_<T, U> &&
  requires (const T& a, const U& b) {
    STL2_DEDUCTION_CONSTRAINT(a < b, Boolean);
    STL2_DEDUCTION_CONSTRAINT(a > b, Boolean);
    STL2_DEDUCTION_CONSTRAINT(a <= b, Boolean);
    STL2_DEDUCTION_CONSTRAINT(a >= b, Boolean);
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
    CommonReference<const T&, const U&>() &&
    TotallyOrdered<CommonType<T, U>>();
}

namespace ext { namespace models {

template <class>
constexpr bool boolean() { return false; }
Boolean{T}
constexpr bool boolean() { return true; }

template <class>
constexpr bool equality_comparable() { return false; }
EqualityComparable{T}
constexpr bool equality_comparable() { return true; }
template <class, class>
constexpr bool equality_comparable() { return false; }
EqualityComparable{T, U}
constexpr bool equality_comparable() { return true; }

template <class>
constexpr bool totally_ordered() { return false; }
TotallyOrdered{T}
constexpr bool totally_ordered() { return true; }
template <class, class>
constexpr bool totally_ordered() { return false; }
TotallyOrdered{T, U}
constexpr bool totally_ordered() { return true; }

}}}} // namespace stl2::v1::ext::models

#endif
