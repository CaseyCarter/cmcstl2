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
#include <stl2/detail/meta.hpp>
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
  requires
    requires (const T& t, const U& u) {
      STL2_DEDUCTION_CONSTRAINT(t == u, Boolean);
      STL2_DEDUCTION_CONSTRAINT(t != u, Boolean);
      // Axiom: t == u and t != u have the same definition space
      // Axiom: bool(t != u) == !bool(t == u)
    }
  constexpr bool __equality_comparable<T, U> = true;

  ///////////////////////////////////////////////////////////////////////////
  // WeaklyEqualityComparable [concepts.lib.compare.equalitycomparable]
  // Relaxation of EqualityComparable<T, U> that doesn't require
  // EqualityComparable<T>, EqualityComparable<U>, Common<T, U>, or
  // EqualityComparable<common_type_t<T, U>>. I.e., provides exactly the
  // requirements for Sentinel's operator ==.
  //
  template <class T, class U>
  concept bool WeaklyEqualityComparable() {
    return __equality_comparable<T, U> &&
      __equality_comparable<U, T>;
    // Axiom: u == t and t == u have the same definition space
    // Axiom: bool(u == t) == bool(t == u)
  }

  ///////////////////////////////////////////////////////////////////////////
  // EqualityComparable [concepts.lib.compare.equalitycomparable]
  //
  template <class T>
  concept bool EqualityComparable() {
    return WeaklyEqualityComparable<T, T>();
  }

  // Transitional hack that makes it ill-formed to check
  // satisfaction of EqualityComparable<Iterator, Sentinel>().
  // Remove this - and its specializations in iterator/concepts.hpp - later.
  template <class T, class U>
  constexpr bool __hack_sentinel_check = true;
  template <class T>
  constexpr bool __hack_sentinel_check<T, T> = true;

  template <class T, class U>
  constexpr bool __EC_sentinel_hack() {
    static_assert(__hack_sentinel_check<T, U>,
      "Differing-type Iterators and Sentinels do NOT satisfy EqualityComparable");
    return true;
  }

  template <class T, class U>
  concept bool EqualityComparable() {
    return __EC_sentinel_hack<T, U>() &&
      EqualityComparable<T>() &&
      EqualityComparable<U>() &&
      WeaklyEqualityComparable<T, U>() &&
      CommonReference<const T&, const U&>() &&
      EqualityComparable<__uncvref<common_reference_t<const T&, const U&>>>();
  }

  namespace models {
    template <class T, class U = T>
    constexpr bool EqualityComparable = false;
    __stl2::EqualityComparable{T}
    constexpr bool EqualityComparable<T, T> = true;
    __stl2::EqualityComparable{T, U}
    constexpr bool EqualityComparable<T, U> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // StrictTotallyOrdered [concepts.lib.compare.stricttotallyordered]
  //
  template <class T, class U>
  constexpr bool __totally_ordered = false;
  template <class T, class U>
    requires requires (const T& t, const U& u) {
      STL2_DEDUCTION_CONSTRAINT(t < u, Boolean);
      STL2_DEDUCTION_CONSTRAINT(t > u, Boolean);
      STL2_DEDUCTION_CONSTRAINT(t <= u, Boolean);
      STL2_DEDUCTION_CONSTRAINT(t >= u, Boolean);
      // Axiom: t < u, t > u, t <= u, t >= u have the same definition space.
      // Axiom: If bool(t < u) then bool(t <= u)
      // Axiom: If bool(t > u) then bool(t >= u)
      // Axiom: Exactly one of bool(t < u), bool(t > u), or
      //        (bool(t <= u) && bool(t >= u)) is true
    }
  constexpr bool __totally_ordered<T, U> = true;

  template <class T>
  concept bool StrictTotallyOrdered() {
    return EqualityComparable<T>() && __totally_ordered<T, T>;
  }

  template <class T, class U>
  concept bool StrictTotallyOrdered() {
    return
      StrictTotallyOrdered<T>() &&
      StrictTotallyOrdered<U>() &&
      EqualityComparable<T, U>() &&
      __totally_ordered<T, U> &&
      __totally_ordered<U, T> &&
      CommonReference<const T&, const U&>() &&
      StrictTotallyOrdered<__uncvref<common_reference_t<const T&, const U&>>>();
  }

  namespace models {
    template <class T, class U = T>
    constexpr bool StrictTotallyOrdered = false;
    __stl2::StrictTotallyOrdered{T}
    constexpr bool StrictTotallyOrdered<T, T> = true;
    __stl2::StrictTotallyOrdered{T, U}
    constexpr bool StrictTotallyOrdered<T, U> = true;
  }
} STL2_CLOSE_NAMESPACE

#endif
