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
#include <stl2/detail/concepts/object/movable.hpp>

/////////////////////////////////////////////
// Comparison Concepts [concepts.lib.compare]
//
STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Boolean [concepts.lib.compare.boolean]
	//
	template<class B>
	META_CONCEPT Boolean =
		Movable<std::decay_t<B>> &&
		requires(const std::remove_reference_t<B>& b1,
		         const std::remove_reference_t<B>& b2, const bool a) {
#if STL2_BROKEN_COMPOUND_REQUIREMENT
			 b1;       requires ConvertibleTo<decltype((b1)), bool>;
			!b1;       requires ConvertibleTo<decltype(!b1),  bool>;
			 b1 && b2; requires Same<decltype(b1 && b2), bool>;
			 b1 && a;  requires Same<decltype(b1 && a),  bool>;
			  a && b2; requires Same<decltype( a && b2), bool>;
			 b1 || b2; requires Same<decltype(b1 || b2), bool>;
			 b1 || a;  requires Same<decltype(b1 || a),  bool>;
			  a || b2; requires Same<decltype( a || b2), bool>;
			 b1 == b2; requires ConvertibleTo<decltype(b1 == b2), bool>;
			 b1 == a;  requires ConvertibleTo<decltype(b1 == a ), bool>;
			  a == b2; requires ConvertibleTo<decltype( a == b2), bool>;
			 b1 != b2; requires ConvertibleTo<decltype(b1 != b2), bool>;
			 b1 != a;  requires ConvertibleTo<decltype(b1 != a ), bool>;
			  a != b2; requires ConvertibleTo<decltype( a != b2), bool>;
#else
			{  b1 }       -> ConvertibleTo<bool>;
			{ !b1 }       -> ConvertibleTo<bool>;
			{  b1 && b2 } -> Same<bool>;
			{  b1 && a  } -> Same<bool>;
			{   a && b2 } -> Same<bool>;
			{  b1 || b2 } -> Same<bool>;
			{  b1 || a  } -> Same<bool>;
			{   a || b2 } -> Same<bool>;
			{  b1 == b2 } -> ConvertibleTo<bool>;
			{  b1 == a  } -> ConvertibleTo<bool>;
			{   a == b2 } -> ConvertibleTo<bool>;
			{  b1 != b2 } -> ConvertibleTo<bool>;
			{  b1 != a  } -> ConvertibleTo<bool>;
			{   a != b2 } -> ConvertibleTo<bool>;
#endif // STL2_BROKEN_COMPOUND_REQUIREMENT
		};

	///////////////////////////////////////////////////////////////////////////
	// WeaklyEqualityComparable [concepts.lib.compare.equalitycomparable]
	// Relaxation of EqualityComparable<T, U> that doesn't require
	// EqualityComparable<T>, EqualityComparable<U>, Common<T, U>, or
	// EqualityComparable<common_type_t<T, U>>. I.e., provides exactly the
	// requirements for Sentinel's operator ==.
	//
	template<class T, class U>
	META_CONCEPT WeaklyEqualityComparable =
		requires(const std::remove_reference_t<T>& t,
				 const std::remove_reference_t<U>& u) {
#if STL2_BROKEN_COMPOUND_REQUIREMENT
			t == u; requires Boolean<decltype(t == u)>;
			t != u; requires Boolean<decltype(t != u)>;
			u == t; requires Boolean<decltype(u == t)>;
			u != t; requires Boolean<decltype(u != t)>;
#else
			{ t == u } -> Boolean;
			{ t != u } -> Boolean;
			{ u == t } -> Boolean;
			{ u != t } -> Boolean;
#endif // STL2_BROKEN_COMPOUND_REQUIREMENT
		};

	///////////////////////////////////////////////////////////////////////////
	// EqualityComparable [concepts.lib.compare.equalitycomparable]
	//
	template<class T>
	META_CONCEPT EqualityComparable =
		WeaklyEqualityComparable<T, T>;

	template<class T, class U>
	META_CONCEPT EqualityComparableWith =
		EqualityComparable<T> &&
		EqualityComparable<U> &&
		WeaklyEqualityComparable<T, U> &&
		CommonReference<
			const std::remove_reference_t<T>&,
			const std::remove_reference_t<U>&> &&
		EqualityComparable<
			common_reference_t<
				const std::remove_reference_t<T>&,
				const std::remove_reference_t<U>&>>;

	///////////////////////////////////////////////////////////////////////////
	// StrictTotallyOrdered [concepts.lib.compare.stricttotallyordered]
	//
	template<class T, class U>
	META_CONCEPT __totally_ordered =
		requires(const std::remove_reference_t<T>& t,
		         const std::remove_reference_t<U>& u) {
#if STL2_BROKEN_COMPOUND_REQUIREMENT
			t <  u; requires Boolean<decltype(t <  u)>;
			t >  u; requires Boolean<decltype(t >  u)>;
			t <= u; requires Boolean<decltype(t <= u)>;
			t >= u; requires Boolean<decltype(t >= u)>;
#else
			{ t <  u } -> Boolean;
			{ t >  u } -> Boolean;
			{ t <= u } -> Boolean;
			{ t >= u } -> Boolean;
#endif // STL2_BROKEN_COMPOUND_REQUIREMENT
			// Axiom: t < u, t > u, t <= u, t >= u have the same definition space.
			// Axiom: If bool(t < u) then bool(t <= u)
			// Axiom: If bool(t > u) then bool(t >= u)
			// Axiom: Exactly one of bool(t < u), bool(t > u), or
			//        (bool(t <= u) && bool(t >= u)) is true
		};

	template<class T>
	META_CONCEPT StrictTotallyOrdered =
		EqualityComparable<T> && __totally_ordered<T, T>;
		// Axiom: t1 == t2 and t1 < t2 have the same definition space.
		// Axiom: bool(t <= t)

	template<class T, class U>
	META_CONCEPT StrictTotallyOrderedWith =
		StrictTotallyOrdered<T> &&
		StrictTotallyOrdered<U> &&
		EqualityComparableWith<T, U> &&
		__totally_ordered<T, U> &&
		__totally_ordered<U, T> &&
		CommonReference<
			const std::remove_reference_t<T>&,
			const std::remove_reference_t<U>&> &&
		StrictTotallyOrdered<
			common_reference_t<
				const std::remove_reference_t<T>&,
				const std::remove_reference_t<U>&>>;
} STL2_CLOSE_NAMESPACE

#endif
