#ifndef STL2_FUNCTIONAL_HPP
#define STL2_FUNCTIONAL_HPP

#include <functional>
#include <type_traits>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>

///////////////////////////////////////////////////////////////////////////
// Functional stuff
//
namespace stl2 { inline namespace v1 {

using std::reference_wrapper;
using std::ref;
using std::cref;

///////////////////////////////////////////////////////////////////////////
// identity
//
struct identity {
  template <class T>
  constexpr T&& operator()(T&& t) const noexcept {
    return stl2::forward<T>(t);
  }
};

///////////////////////////////////////////////////////////////////////////
// equal_to
//
template <class T = void>
  requires Same<T, void>() || EqualityComparable<T>()
struct equal_to {
  constexpr bool operator()(const T& a, const T& b) const {
    return bool(a == b);
  }

  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template <>
struct equal_to<void> {
  EqualityComparable{T, U}
  constexpr auto operator()(const T& t, const U& u) const ->
    decltype(t == u) {
    return {t == u};
  }

  using is_transparent = std::true_type;
};

///////////////////////////////////////////////////////////////////////////
// not_equal_to
//
template <class T = void>
  requires Same<T, void>() || EqualityComparable<T>()
struct not_equal_to {
  constexpr bool operator()(const T& a, const T& b) const {
    return bool(a != b);
  }

  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template <>
struct not_equal_to<void> {
  EqualityComparable{T, U}
  constexpr auto operator()(const T& t, const U& u) const ->
    decltype(t != u) {
    return {t != u};
  }

  using is_transparent = std::true_type;
};

///////////////////////////////////////////////////////////////////////////
// greater
//
template <class T = void>
  requires Same<T, void>() || TotallyOrdered<T>()
struct greater {
  constexpr bool operator()(const T& a, const T& b) const {
    return bool(a > b);
  }

  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template <>
struct greater<void> {
  TotallyOrdered{T, U}
  constexpr auto operator()(const T& t, const U& u) const ->
    decltype(t > u) {
    return {t > u};
  }

  using is_transparent = std::true_type;
};

///////////////////////////////////////////////////////////////////////////
// less
//
template <class T = void>
  requires Same<T, void>() || TotallyOrdered<T>()
struct less {
  constexpr bool operator()(const T& a, const T& b) const {
    return bool(a < b);
  }

  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template <>
struct less<void> {
  TotallyOrdered{T, U}
  constexpr auto operator()(const T& t, const U& u) const ->
    decltype(t < u) {
    return {t < u};
  }

  using is_transparent = std::true_type;
};

///////////////////////////////////////////////////////////////////////////
// greater_equal
//
template <class T = void>
  requires Same<T, void>() || TotallyOrdered<T>()
struct greater_equal {
  constexpr bool operator()(const T& a, const T& b) const {
    return bool(a >= b);
  }

  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template <>
struct greater_equal<void> {
  TotallyOrdered{T, U}
  constexpr auto operator()(const T& t, const U& u) const ->
    decltype(t >= u) {
    return {t >= u};
  }

  using is_transparent = std::true_type;
};

///////////////////////////////////////////////////////////////////////////
// less_equal
//
template <class T = void>
  requires Same<T, void>() || TotallyOrdered<T>()
struct less_equal {
  constexpr bool operator()(const T& a, const T& b) const {
    return bool(a <= b);
  }

  using first_argument_type = T;
  using second_argument_type = T;
  using result_type = bool;
};

template <>
struct less_equal<void> {
  TotallyOrdered{T, U}
  constexpr auto operator()(const T& t, const U& u) const ->
    decltype(t <= u) {
    return {t <= u};
  }

  using is_transparent = std::true_type;
};
}}

#endif
