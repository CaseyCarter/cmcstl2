// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_CONCEPTS_ALGORITHM_HPP
#define STL2_DETAIL_CONCEPTS_ALGORITHM_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// Common algorithm requirements [commmonalgoreq]
//
STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// IndirectlyComparable [commmonalgoreq.indirectlycomparable]
	//
	template<class I1, class I2, class R = equal_to<>, class P1 = identity,
		class P2 = identity>
	STL2_CONCEPT IndirectlyComparable =
		IndirectRelation<R, projected<I1, P1>, projected<I2, P2>>;

	///////////////////////////////////////////////////////////////////////////
	// Permutable [commmonalgoreq.permutable]
	//
	template<class I>
	STL2_CONCEPT Permutable =
		ForwardIterator<I> &&
		IndirectlyMovableStorable<I, I> &&
		IndirectlySwappable<I, I>;

	///////////////////////////////////////////////////////////////////////////
	// Mergeable [commmonalgoreq.mergeable]
	//
	template<class I1, class I2, class Out, class R = less<>,
		class P1 = identity, class P2 = identity>
	STL2_CONCEPT Mergeable =
		InputIterator<I1> &&
		InputIterator<I2> &&
		WeaklyIncrementable<Out> &&
		IndirectlyCopyable<I1, Out> &&
		IndirectlyCopyable<I2, Out> &&
		IndirectStrictWeakOrder<R, projected<I1, P1>, projected<I2, P2>>;

	///////////////////////////////////////////////////////////////////////////
	// Sortable [commmonalgoreq.sortable]
	//
	template<class I, class R = less<>, class P = identity>
	STL2_CONCEPT Sortable =
		Permutable<I> &&
		IndirectStrictWeakOrder<R, projected<I, P>>;
} STL2_CLOSE_NAMESPACE

#endif
