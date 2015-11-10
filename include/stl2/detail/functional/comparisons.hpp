// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_FUNCTIONAL_COMPARISONS_HPP
#define STL2_DETAIL_FUNCTIONAL_COMPARISONS_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // equal_to [comparisons]
  //
  template <class T = void>
    requires Same<T, void>() || EqualityComparable<T>()
  struct equal_to {
    constexpr bool operator()(const T& a, const T& b) const {
      return a == b;
    }
  };

  template <>
  struct equal_to<void> {
    EqualityComparable{T, U}
    constexpr decltype(auto) operator()(const T& t, const U& u) const {
      return t == u;
    }

    using is_transparent = true_type;
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
  };

  template <>
  struct not_equal_to<void> {
    EqualityComparable{T, U}
    constexpr decltype(auto) operator()(const T& t, const U& u) const {
      return t != u;
    }

    using is_transparent = true_type;
  };

  ///////////////////////////////////////////////////////////////////////////
  // greater
  //
  template <class T = void>
    requires Same<T, void>() || StrictTotallyOrdered<T>()
  struct greater {
    constexpr bool operator()(const T& a, const T& b) const {
      return bool(a > b);
    }
  };

  template <>
  struct greater<void> {
    StrictTotallyOrdered{T, U}
    constexpr decltype(auto) operator()(const T& t, const U& u) const {
      return t > u;
    }

    using is_transparent = true_type;
  };

  ///////////////////////////////////////////////////////////////////////////
  // less
  //
  template <class T = void>
    requires Same<T, void>() || StrictTotallyOrdered<T>()
  struct less {
    constexpr bool operator()(const T& a, const T& b) const {
      return bool(a < b);
    }
  };

  template <>
  struct less<void> {
    StrictTotallyOrdered{T, U}
    constexpr decltype(auto) operator()(const T& t, const U& u) const {
      return t < u;
    }

    using is_transparent = true_type;
  };

  ///////////////////////////////////////////////////////////////////////////
  // greater_equal
  //
  template <class T = void>
    requires Same<T, void>() || StrictTotallyOrdered<T>()
  struct greater_equal {
    constexpr bool operator()(const T& a, const T& b) const {
      return bool(a >= b);
    }
  };

  template <>
  struct greater_equal<void> {
    StrictTotallyOrdered{T, U}
    constexpr decltype(auto) operator()(const T& t, const U& u) const {
      return t >= u;
    }

    using is_transparent = true_type;
  };

  ///////////////////////////////////////////////////////////////////////////
  // less_equal
  //
  template <class T = void>
    requires Same<T, void>() || StrictTotallyOrdered<T>()
  struct less_equal {
    constexpr bool operator()(const T& a, const T& b) const {
      return bool(a <= b);
    }
  };

  template <>
  struct less_equal<void> {
    StrictTotallyOrdered{T, U}
    constexpr decltype(auto) operator()(const T& t, const U& u) const {
      return t <= u;
    }

    using is_transparent = true_type;
  };
} STL2_CLOSE_NAMESPACE

#endif
