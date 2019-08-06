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
	// boolean [concepts.lib.compare.boolean]
	//
	template<class B>
	META_CONCEPT boolean =
		movable<std::decay_t<B>> &&
		requires(const std::remove_reference_t<B>& b1,
			     const std::remove_reference_t<B>& b2, const bool a) {
			// Requirements common to both boolean and BooleanTestable.
			{  b1      } -> STL2_RVALUE_REQ(convertible_to<bool>);
			{ !b1      } -> STL2_RVALUE_REQ(convertible_to<bool>);
			{  b1 && a } -> STL2_RVALUE_REQ(same_as<bool>);
			{  b1 || a } -> STL2_RVALUE_REQ(same_as<bool>);

			// Requirements of boolean that are also be valid for
			// BooleanTestable, but for which BooleanTestable does not
			// require validation.
			{ b1 && b2 } -> STL2_RVALUE_REQ(same_as<bool>);
			{  a && b2 } -> STL2_RVALUE_REQ(same_as<bool>);
			{ b1 || b2 } -> STL2_RVALUE_REQ(same_as<bool>);
			{  a || b2 } -> STL2_RVALUE_REQ(same_as<bool>);

			// Requirements of boolean that are not required by
			// BooleanTestable.
			{ b1 == b2 } -> STL2_RVALUE_REQ(convertible_to<bool>);
			{ b1 == a  } -> STL2_RVALUE_REQ(convertible_to<bool>);
			{  a == b2 } -> STL2_RVALUE_REQ(convertible_to<bool>);
			{ b1 != b2 } -> STL2_RVALUE_REQ(convertible_to<bool>);
			{ b1 != a  } -> STL2_RVALUE_REQ(convertible_to<bool>);
			{  a != b2 } -> STL2_RVALUE_REQ(convertible_to<bool>);
		};

	///////////////////////////////////////////////////////////////////////////
	// WeaklyEqualityComparable [concepts.lib.compare.equalitycomparable]
	// Relaxation of equality_comparable<T, U> that doesn't require
	// equality_comparable<T>, equality_comparable<U>, common_with<T, U>, or
	// equality_comparable<common_type_t<T, U>>. I.e., provides exactly the
	// requirements for Sentinel's operator ==.
	//
	template<class T, class U>
	META_CONCEPT WeaklyEqualityComparable =
		requires(const std::remove_reference_t<T>& t,
				 const std::remove_reference_t<U>& u) {
			{ t == u } -> STL2_RVALUE_REQ(boolean);
			{ t != u } -> STL2_RVALUE_REQ(boolean);
			{ u == t } -> STL2_RVALUE_REQ(boolean);
			{ u != t } -> STL2_RVALUE_REQ(boolean);
		};

	///////////////////////////////////////////////////////////////////////////
	// equality_comparable [concepts.lib.compare.equalitycomparable]
	//
	template<class T>
	META_CONCEPT equality_comparable =
		WeaklyEqualityComparable<T, T>;

	template<class T, class U>
	META_CONCEPT equality_comparable_with =
		equality_comparable<T> &&
		equality_comparable<U> &&
		WeaklyEqualityComparable<T, U> &&
		common_reference_with<
			const std::remove_reference_t<T>&,
			const std::remove_reference_t<U>&> &&
		equality_comparable<
			common_reference_t<
				const std::remove_reference_t<T>&,
				const std::remove_reference_t<U>&>>;

	///////////////////////////////////////////////////////////////////////////
	// totally_ordered [concepts.lib.compare.stricttotallyordered]
	//
	template<class T, class U>
	META_CONCEPT __totally_ordered =
		requires(const std::remove_reference_t<T>& t,
		         const std::remove_reference_t<U>& u) {
			{ t <  u } -> STL2_RVALUE_REQ(boolean);
			{ t >  u } -> STL2_RVALUE_REQ(boolean);
			{ t <= u } -> STL2_RVALUE_REQ(boolean);
			{ t >= u } -> STL2_RVALUE_REQ(boolean);
			// Axiom: t < u, t > u, t <= u, t >= u have the same definition space.
			// Axiom: If bool(t < u) then bool(t <= u)
			// Axiom: If bool(t > u) then bool(t >= u)
			// Axiom: Exactly one of bool(t < u), bool(t > u), or
			//        (bool(t <= u) && bool(t >= u)) is true
		};

	template<class T>
	META_CONCEPT totally_ordered =
		equality_comparable<T> && __totally_ordered<T, T>;
		// Axiom: t1 == t2 and t1 < t2 have the same definition space.
		// Axiom: bool(t <= t)

	template<class T, class U>
	META_CONCEPT totally_ordered_with =
		totally_ordered<T> &&
		totally_ordered<U> &&
		equality_comparable_with<T, U> &&
		__totally_ordered<T, U> &&
		__totally_ordered<U, T> &&
		common_reference_with<
			const std::remove_reference_t<T>&,
			const std::remove_reference_t<U>&> &&
		totally_ordered<
			common_reference_t<
				const std::remove_reference_t<T>&,
				const std::remove_reference_t<U>&>>;
} STL2_CLOSE_NAMESPACE

#endif
