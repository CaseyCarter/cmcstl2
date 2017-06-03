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
	// Not to spec: see https://github.com/ericniebler/stl2/issues/155
	//
	template <class B>
	concept bool Boolean =
		Movable<decay_t<B>> &&
		requires(const remove_reference_t<B>& b1,
					const remove_reference_t<B>& b2, const bool a) {
			// Requirements common to both Boolean and BooleanTestable.
			{ b1 } -> ConvertibleTo<bool>&&;
			{ !b1 } -> ConvertibleTo<bool>&&;
			{ b1 && a } ->  Same<bool>&&;
			{ b1 || a } ->  Same<bool>&&;

			// Requirements of Boolean that are also be valid for
			// BooleanTestable, but for which BooleanTestable does not
			// require validation.
			{ b1 && b2 } -> Same<bool>&&;
			{  a && b2 } -> Same<bool>&&;
			{ b1 || b2 } -> Same<bool>&&;
			{  a || b2 } -> Same<bool>&&;

			// Requirements of Boolean that are not required by
			// BooleanTestable.
			{ b1 == b2 } -> ConvertibleTo<bool>&&;
			{ b1 == a  } -> ConvertibleTo<bool>&&;
			{  a == b2 } -> ConvertibleTo<bool>&&;
			{ b1 != b2 } -> ConvertibleTo<bool>&&;
			{ b1 != a  } -> ConvertibleTo<bool>&&;
			{  a != b2 } -> ConvertibleTo<bool>&&;
		};

	namespace models {
		template <class>
		constexpr bool Boolean = false;
		__stl2::Boolean{B}
		constexpr bool Boolean<B> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// WeaklyEqualityComparable [concepts.lib.compare.equalitycomparable]
	// Relaxation of EqualityComparable<T, U> that doesn't require
	// EqualityComparable<T>, EqualityComparable<U>, Common<T, U>, or
	// EqualityComparable<common_type_t<T, U>>. I.e., provides exactly the
	// requirements for Sentinel's operator ==.
	//
	template <class T, class U>
	concept bool WeaklyEqualityComparable =
		requires(const remove_reference_t<T>& t,
				 const remove_reference_t<U>& u) {
			{ t == u } -> Boolean&&;
			{ t != u } -> Boolean&&;
			{ u == t } -> Boolean&&;
			{ u != t } -> Boolean&&;
		};

	namespace models {
		template <class, class>
		constexpr bool WeaklyEqualityComparable = false;
		__stl2::WeaklyEqualityComparable{T, U}
		constexpr bool WeaklyEqualityComparable<T, U> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// EqualityComparable [concepts.lib.compare.equalitycomparable]
	//
	template <class T>
	concept bool EqualityComparable =
		WeaklyEqualityComparable<T, T>;

	template <class T, class U>
	concept bool EqualityComparableWith =
		EqualityComparable<T> &&
		EqualityComparable<U> &&
		WeaklyEqualityComparable<T, U> &&
		CommonReference<
			const remove_reference_t<T>&,
			const remove_reference_t<U>&> &&
		EqualityComparable<
			common_reference_t<
				const remove_reference_t<T>&,
				const remove_reference_t<U>&>>;

	namespace models {
		template <class T, class U = T>
		constexpr bool EqualityComparable = false;
		__stl2::EqualityComparable{T}
		constexpr bool EqualityComparable<T, T> = true;
		__stl2::EqualityComparableWith{T, U}
		constexpr bool EqualityComparable<T, U> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// StrictTotallyOrdered [concepts.lib.compare.stricttotallyordered]
	//
	template <class T, class U>
	concept bool __totally_ordered =
		requires(const remove_reference_t<T>& t,
		         const remove_reference_t<U>& u) {
			{ t <  u } -> Boolean&&;
			{ t >  u } -> Boolean&&;
			{ t <= u } -> Boolean&&;
			{ t >= u } -> Boolean&&;
			// Axiom: t < u, t > u, t <= u, t >= u have the same definition space.
			// Axiom: If bool(t < u) then bool(t <= u)
			// Axiom: If bool(t > u) then bool(t >= u)
			// Axiom: Exactly one of bool(t < u), bool(t > u), or
			//        (bool(t <= u) && bool(t >= u)) is true
		};

	template <class T>
	concept bool StrictTotallyOrdered =
		EqualityComparable<T> && __totally_ordered<T, T>;
		// Axiom: t1 == t2 and t1 < t2 have the same definition space.
		// Axiom: bool(t <= t)

	template <class T, class U>
	concept bool StrictTotallyOrderedWith =
		StrictTotallyOrdered<T> &&
		StrictTotallyOrdered<U> &&
		EqualityComparableWith<T, U> &&
		__totally_ordered<T, U> &&
		__totally_ordered<U, T> &&
		CommonReference<
			const remove_reference_t<T>&,
			const remove_reference_t<U>&> &&
		StrictTotallyOrdered<
			common_reference_t<
				const remove_reference_t<T>&,
				const remove_reference_t<U>&>>;

	namespace models {
		template <class T, class U = T>
		constexpr bool StrictTotallyOrdered = false;
		__stl2::StrictTotallyOrdered{T}
		constexpr bool StrictTotallyOrdered<T, T> = true;
		__stl2::StrictTotallyOrderedWith{T, U}
		constexpr bool StrictTotallyOrdered<T, U> = true;
	}
} STL2_CLOSE_NAMESPACE

#endif
