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
#include <stl2/detail/concepts/object/move_constructible.hpp>

/////////////////////////////////////////////
// Comparison Concepts [concepts.lib.compare]
//
STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // Boolean [concepts.lib.compare.boolean]
  // Not to spec: Boolean requires MoveConstructible
  //
  template <class>
  constexpr bool __boolean = false;
  template <class B>
    requires requires (const B& b1, const B& b2, const bool a) {
      // Requirements common to both Boolean and BooleanTestable.
      STL2_BINARY_DEDUCTION_CONSTRAINT(b1, ConvertibleTo, bool);
      STL2_BINARY_DEDUCTION_CONSTRAINT(!b1, ConvertibleTo, bool);
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
      STL2_BINARY_DEDUCTION_CONSTRAINT(b1 == b2, ConvertibleTo, bool);
      STL2_BINARY_DEDUCTION_CONSTRAINT(b1 == a, ConvertibleTo, bool);
      STL2_BINARY_DEDUCTION_CONSTRAINT(a == b2, ConvertibleTo, bool);
      STL2_BINARY_DEDUCTION_CONSTRAINT(b1 != b2, ConvertibleTo, bool);
      STL2_BINARY_DEDUCTION_CONSTRAINT(b1 != a, ConvertibleTo, bool);
      STL2_BINARY_DEDUCTION_CONSTRAINT(a != b2, ConvertibleTo, bool);
    }
  constexpr bool __boolean<B> = true;

  template <class B>
  concept bool Boolean() {
    return MoveConstructible<B>() && __boolean<B>;
  }

  namespace models {
    template <class>
    constexpr bool Boolean = false;
    __stl2::Boolean{B}
    constexpr bool Boolean<B> = true;
  }

  template <class T, class U>
    constexpr bool __equality_comparable = false;
  template <class T, class U>
    requires requires (const T& t, const U& u) {
      STL2_DEDUCTION_CONSTRAINT(t == u, Boolean);
      STL2_DEDUCTION_CONSTRAINT(t != u, Boolean);
    }
  constexpr bool __equality_comparable<T, U> = true;

  ///////////////////////////////////////////////////////////////////////////
  // WeaklyEqualityComparable
  // Extension: Equivalent to EqualityComparable, except that it doesn't
  //            cause compile errors if common_type is unspecialized.
  //
  namespace ext {
    template <class T>
    concept bool WeaklyEqualityComparable() {
      return __equality_comparable<T, T>;
    }

    template <class T, class U>
    concept bool WeaklyEqualityComparable() {
      return WeaklyEqualityComparable<T>() &&
        WeaklyEqualityComparable<U>() &&
        __equality_comparable<T, U> &&
        __equality_comparable<U, T>;
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

  ///////////////////////////////////////////////////////////////////////////
  // EqualityComparable [concepts.lib.compare.equalitycomparable]
  //
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
  // StrictTotallyOrdered [concepts.lib.compare.stricttotallyordered]
  //
  template <class T, class U>
  constexpr bool __totally_ordered = false;
  template <class T, class U>
    requires requires (const T& a, const U& b) {
      STL2_DEDUCTION_CONSTRAINT(a < b, Boolean);
      STL2_DEDUCTION_CONSTRAINT(a > b, Boolean);
      STL2_DEDUCTION_CONSTRAINT(a <= b, Boolean);
      STL2_DEDUCTION_CONSTRAINT(a >= b, Boolean);
    }
  constexpr bool __totally_ordered<T, U> = true;

  namespace ext {
    ///////////////////////////////////////////////////////////////////////////
    // WeaklyStrictTotallyOrdered
    // Extension: Equivalent to StrictTotallyOrdered, except that it doesn't
    //            cause compile errors if common_type is unspecialized.
    //
    template <class T>
    concept bool WeaklyStrictTotallyOrdered() {
      return WeaklyEqualityComparable<T>() &&
        __totally_ordered<T, T>;
    }

    template <class T, class U>
    concept bool WeaklyStrictTotallyOrdered() {
      return WeaklyStrictTotallyOrdered<T>() &&
        WeaklyStrictTotallyOrdered<U>() &&
        WeaklyEqualityComparable<T, U>() &&
        __totally_ordered<T, U> &&
        __totally_ordered<U, T>;
    }
  }

  namespace models {
    template <class T, class U = T>
    constexpr bool WeaklyStrictTotallyOrdered = false;
    __stl2::ext::WeaklyStrictTotallyOrdered{T}
    constexpr bool WeaklyStrictTotallyOrdered<T, T> = true;
    __stl2::ext::WeaklyStrictTotallyOrdered{T, U}
    constexpr bool WeaklyStrictTotallyOrdered<T, U> = true;
  }

  template <class T>
  concept bool StrictTotallyOrdered() {
    return ext::WeaklyStrictTotallyOrdered<T>();
  }

  template <class T, class U>
  concept bool StrictTotallyOrdered() {
    return ext::WeaklyStrictTotallyOrdered<T, U>() &&
      CommonReference<const T&, const U&>() &&
      StrictTotallyOrdered<CommonType<T, U>>();
  }

  namespace models {
    template <class T, class U = T>
    constexpr bool StrictTotallyOrdered = false;
    __stl2::StrictTotallyOrdered{T}
    constexpr bool StrictTotallyOrdered<T, T> = true;
    __stl2::StrictTotallyOrdered{T, U}
    constexpr bool StrictTotallyOrdered<T, U> = true;

    // StrictTotallyOrdered<Ts>() && ...
    template <class...Ts>
    constexpr bool AllTotallyOrdered = false;
    template <__stl2::StrictTotallyOrdered...Ts>
    constexpr bool AllTotallyOrdered<Ts...> = true;
  }
} STL2_CLOSE_NAMESPACE

#endif
