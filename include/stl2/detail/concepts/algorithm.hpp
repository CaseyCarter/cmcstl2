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
	template <class I1, class I2, class R = equal_to<>, class P1 = identity,
		class P2 = identity>
	concept bool IndirectlyComparable() {
		return IndirectInvocableRelation<R, projected<I1, P1>, projected<I2, P2>>();
	}

	namespace models {
		template <class, class, class = equal_to<>, class = identity, class = identity>
		constexpr bool IndirectlyComparable = false;
		__stl2::IndirectlyComparable{I1, I2, R, P1, P2}
		constexpr bool IndirectlyComparable<I1, I2, R, P1, P2> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Permutable [commmonalgoreq.permutable]
	//
	template <class I>
	concept bool Permutable() {
		return ForwardIterator<I>() &&
			IndirectlyMovableStorable<I, I>() &&
			IndirectlySwappable<I, I>();
	}

	namespace models {
		template <class>
		constexpr bool Permutable = false;
		__stl2::Permutable{I}
		constexpr bool Permutable<I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Mergeable [commmonalgoreq.mergeable]
	//
	template <class I1, class I2, class Out, class R = less<>,
		class P1 = identity, class P2 = identity>
	concept bool Mergeable() {
		return InputIterator<I1>() &&
			InputIterator<I2>() &&
			WeaklyIncrementable<Out>() &&
			IndirectlyCopyable<I1, Out>() &&
			IndirectlyCopyable<I2, Out>() &&
			IndirectInvocableStrictWeakOrder<R, projected<I1, P1>, projected<I2, P2>>();
	}

	namespace models {
		template <class, class, class, class, class, class>
		constexpr bool Mergeable = false;
		__stl2::Mergeable{I1, I2, Out, R, P1, P2}
		constexpr bool Mergeable<I1, I2, Out, R, P1, P2> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Sortable [commmonalgoreq.sortable]
	//
	template <class I, class R = less<>, class P = identity>
	concept bool Sortable() {
		return Permutable<I>() &&
			IndirectInvocableStrictWeakOrder<R, projected<I, P>>();
	}

	namespace models {
		template <class, class, class>
		constexpr bool Sortable = false;
		__stl2::Sortable{I, R, P}
		constexpr bool Sortable<I, R, P> = true;
	}
} STL2_CLOSE_NAMESPACE

#endif
