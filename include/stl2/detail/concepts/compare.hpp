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
#ifndef STL2_DETAIL_CONCEPTS_COMPARE_HPP
#define STL2_DETAIL_CONCEPTS_COMPARE_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>

// FIXME: Do a full build time comparison.
#define STL2_BOOLEAN_BICONVERTIBLE 1

/////////////////////////////////////////////
// Comparison Concepts [concepts.lib.compare]
//
STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // Boolean [concepts.lib.compare.boolean]
  //
  namespace detail {
    template <class B>
    concept bool Boolean_() {
      return requires (const B& b1, const B& b2, const bool a) {
        // Requirements common to both Boolean and BooleanTestable.
#if STL2_BOOLEAN_BICONVERTIBLE
        STL2_BINARY_DEDUCTION_CONSTRAINT(b1, ConvertibleTo, bool);
        STL2_BINARY_DEDUCTION_CONSTRAINT(!b1, ConvertibleTo, bool);
#else
        {b1} -> bool;
        {!b1} -> bool;
#endif
        STL2_EXACT_TYPE_CONSTRAINT(b1 && a, bool);
        STL2_EXACT_TYPE_CONSTRAINT(b1 || a, bool);

        // Requirements of Boolean that are also be valid for
        // BooleanTestable, but for which BooleanTestable does not
        // require validation.
        STL2_EXACT_TYPE_CONSTRAINT(b1 && b2, bool);
        STL2_EXACT_TYPE_CONSTRAINT(a && b2, bool);
        STL2_EXACT_TYPE_CONSTRAINT(b1 || b2, bool);
        STL2_EXACT_TYPE_CONSTRAINT(a || b2, bool);

        // Requirements of Boolean that are not required by
        // BooleanTestable.
#if STL2_BOOLEAN_BICONVERTIBLE
        STL2_BINARY_DEDUCTION_CONSTRAINT(b1 == b2, ConvertibleTo, bool);
        STL2_BINARY_DEDUCTION_CONSTRAINT(b1 == a, ConvertibleTo, bool);
        STL2_BINARY_DEDUCTION_CONSTRAINT(a == b2, ConvertibleTo, bool);
        STL2_BINARY_DEDUCTION_CONSTRAINT(b1 != b2, ConvertibleTo, bool);
        STL2_BINARY_DEDUCTION_CONSTRAINT(b1 != a, ConvertibleTo, bool);
        STL2_BINARY_DEDUCTION_CONSTRAINT(a != b2, ConvertibleTo, bool);
#else
        {b1 == b2} -> bool;
        {b1 == a} -> bool;
        {a == b2} -> bool;
        {b1 != b2} -> bool;
        {b1 != a} -> bool;
        {a != b2} -> bool;
#endif
      };
    }
  }

  namespace models {
    template <class>
    constexpr bool Boolean = false;
    __stl2::detail::Boolean_{T}
    constexpr bool Boolean<T> = true;
  }

  template <class T>
  concept bool Boolean() {
    return models::Boolean<T>;
  }

  ///////////////////////////////////////////////////////////////////////////
  // EqualityComparable [concepts.lib.compare.equalitycomparable]
  //
  namespace detail {
    template <class T, class U>
    concept bool EqualityComparable_ =
      requires (const T& t, const U& u) {
        STL2_DEDUCTION_CONSTRAINT(t == u, Boolean);
        STL2_DEDUCTION_CONSTRAINT(t != u, Boolean);
      };
  }

  namespace ext {
    ///////////////////////////////////////////////////////////////////////////
    // WeaklyEqualityComparable
    // Extension: Equivalent to EqualityComparable, except that it doesn't
    //            cause compile errors if common_type is unspecialized.
    //
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
  }

  namespace models {
    template <class T, class U = T>
    constexpr bool WeaklyEqualityComparable = false;
    __stl2::ext::WeaklyEqualityComparable{T}
    constexpr bool WeaklyEqualityComparable<T, T> = true;
    __stl2::ext::WeaklyEqualityComparable{T, U}
    constexpr bool WeaklyEqualityComparable<T, U> = true;
  }

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

  namespace models {
    template <class T, class U = T>
    constexpr bool EqualityComparable = false;
    __stl2::EqualityComparable{T}
    constexpr bool EqualityComparable<T, T> = true;
    __stl2::EqualityComparable{T, U}
    constexpr bool EqualityComparable<T, U> = true;

    // EqualityComparable<Ts>() && ...
    template <class...Ts>
    constexpr bool AllEqualityComparable = false;
    template <__stl2::EqualityComparable...Ts>
    constexpr bool AllEqualityComparable<Ts...> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // TotallyOrdered [concepts.lib.compare.totallyordered]
  //
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
  }

  namespace ext {
    ///////////////////////////////////////////////////////////////////////////
    // WeaklyTotallyOrdered
    // Extension: Equivalent to TotallyOrdered, except that it doesn't
    //            cause compile errors if common_type is unspecialized.
    //
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
  }

  namespace models {
    template <class T, class U = T>
    constexpr bool WeaklyTotallyOrdered = false;
    __stl2::ext::WeaklyTotallyOrdered{T}
    constexpr bool WeaklyTotallyOrdered<T, T> = true;
    __stl2::ext::WeaklyTotallyOrdered{T, U}
    constexpr bool WeaklyTotallyOrdered<T, U> = true;
  }

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

  namespace models {
    template <class T, class U = T>
    constexpr bool TotallyOrdered = false;
    __stl2::TotallyOrdered{T}
    constexpr bool TotallyOrdered<T, T> = true;
    __stl2::TotallyOrdered{T, U}
    constexpr bool TotallyOrdered<T, U> = true;

    // TotallyOrdered<Ts>() && ...
    template <class...Ts>
    constexpr bool AllTotallyOrdered = false;
    template <__stl2::TotallyOrdered...Ts>
    constexpr bool AllTotallyOrdered<Ts...> = true;
  }
} STL2_CLOSE_NAMESPACE

#undef STL2_BOOLEAN_BICONVERTIBLE
#endif
