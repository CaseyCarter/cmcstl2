#ifndef STL2_DETAIL_CONCEPTS_FUNDAMENTAL_HPP
#define STL2_DETAIL_CONCEPTS_FUNDAMENTAL_HPP

#include <type_traits>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

namespace stl2 { inline namespace v1 { namespace ext {
template <class T>
concept bool Scalar() {
  return _Is<T, std::is_scalar> && Regular<T>();
}

template <class T>
concept bool Arithmetic() {
  return _Is<T, std::is_arithmetic> && Scalar<T>() && TotallyOrdered<T>();
}

template <class T>
concept bool FloatingPoint() {
  return _Is<T, std::is_floating_point> && Arithmetic<T>();
}
} // namespace ext

template <class T>
concept bool Integral() {
  return _Is<T, std::is_integral> && ext::Arithmetic<T>();
}

// 20150712: Not to spec.
template <class T>
concept bool SignedIntegral() {
  return Integral<T>() && (T(-1) < T(0));
}

template <class T>
concept bool UnsignedIntegral() {
  return Integral<T>() && !SignedIntegral<T>();
}

namespace ext { namespace models {
template <class>
constexpr bool scalar() { return false; }
Scalar{T}
constexpr bool scalar() { return true; }

template <class>
constexpr bool arithmetic() { return false; }
Arithmetic{T}
constexpr bool arithmetic() { return true; }

template <class>
constexpr bool floating_point() { return false; }
FloatingPoint{T}
constexpr bool floating_point() { return true; }

template <class>
constexpr bool integral() { return false; }
Integral{T}
constexpr bool integral() { return true; }

template <class>
constexpr bool signed_integral() { return false; }
SignedIntegral{T}
constexpr bool signed_integral() { return true; }

template <class>
constexpr bool unsigned_integral() { return false; }
UnsignedIntegral{T}
constexpr bool unsigned_integral() { return true; }
}}}} // namespace stl2::v1::ext::models

#endif
