#ifndef STL2_CONCEPTS_COMPARE_HPP
#define STL2_CONCEPTS_COMPARE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/core.hpp>
#include <stl2/common_type.hpp>

/////////////////////////////////////////////
// Comparison Concepts [concepts.lib.compare]
//
namespace stl2 { inline namespace v1 {

#if 1
// BooleanTestable from DR2114.
// (See https://github.com/CaseyCarter/stl2/issues/3#issuecomment-121147664)
template <class B>
concept bool Boolean() {
  return requires (const B& b, const bool a) {
    STL2_CONVERSION_CONSTRAINT(b, bool);
    STL2_CONVERSION_CONSTRAINT(!b, bool);
    STL2_EXACT_TYPE_CONSTRAINT(b && a, bool);
    STL2_EXACT_TYPE_CONSTRAINT(b || a, bool);
  };
}

#else

// Boolean from the proposal.
template <class B>
concept bool Boolean() {
  return requires (const B& b1, const B& b2, const bool a) {
    STL2_CONVERSION_CONSTRAINT(b1, bool);
    STL2_CONVERSION_CONSTRAINT(!b1, bool);
    STL2_EXACT_TYPE_CONSTRAINT(b1 && b2, bool);
    STL2_EXACT_TYPE_CONSTRAINT(b1 && a, bool);
    STL2_EXACT_TYPE_CONSTRAINT(a && b2, bool);
    STL2_EXACT_TYPE_CONSTRAINT(b1 || b2, bool);
    STL2_EXACT_TYPE_CONSTRAINT(b1 || a, bool);
    STL2_EXACT_TYPE_CONSTRAINT(a || b2, bool);
    STL2_CONVERSION_CONSTRAINT(b1 == b2, bool);
    STL2_CONVERSION_CONSTRAINT(b1 == a, bool);
    STL2_CONVERSION_CONSTRAINT(a == b2, bool);
    STL2_CONVERSION_CONSTRAINT(b1 != b2, bool);
    STL2_CONVERSION_CONSTRAINT(b1 != a, bool);
    STL2_CONVERSION_CONSTRAINT(a != b2, bool);
  };
}
#endif

namespace detail {

template <class T, class U>
concept bool EqualityComparable_ =
  requires (const T& t, const U& u) {
#if 1 //0 // BUG: OOM
    STL2_DEDUCTION_CONSTRAINT(t == u, Boolean);
    STL2_DEDUCTION_CONSTRAINT(t != u, Boolean);
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
    CommonReference<const T&, const U&>() &&
    EqualityComparable<CommonType<T, U>>();
}

namespace detail {

template <class T, class U>
concept bool TotallyOrdered_ =
  EqualityComparable_<T, U> &&
  requires (const T& a, const U& b) {
#if 1 //0 // FIXME: ICE
    STL2_DEDUCTION_CONSTRAINT(a < b, Boolean);
    STL2_DEDUCTION_CONSTRAINT(a > b, Boolean);
    STL2_DEDUCTION_CONSTRAINT(a <= b, Boolean);
    STL2_DEDUCTION_CONSTRAINT(a >= b, Boolean);
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

#endif // STL2_CONCEPTS_COMPARE_HPP
