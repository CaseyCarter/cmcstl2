// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Christopher Di Bella 2016
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_MEMORY_CONCEPTS_HPP
#define STL2_DETAIL_MEMORY_CONCEPTS_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// _NoThrowInputIterator [Exposition]
	//
	template<class I>
	META_CONCEPT _NoThrowInputIterator =
		input_iterator<I> &&
		std::is_lvalue_reference_v<iter_reference_t<I>> &&
		same_as<__uncvref<iter_reference_t<I>>, iter_value_t<I>>;
		// Axiom: no exceptions are thrown from increment, copy, move, assignment,
		//        indirection through valid iterators.

	///////////////////////////////////////////////////////////////////////////
	// _NoThrowSentinel [Exposition]
	//
	template<class S, class I>
	META_CONCEPT _NoThrowSentinel =
		sentinel_for<S, I>;
		// Axiom: no exceptions are thrown from comparisons between objects of types
		//        I and S.

	///////////////////////////////////////////////////////////////////////////
	// _NoThrowInputRange [Exposition]
	//
	template<class Rng>
	META_CONCEPT _NoThrowInputRange =
		range<Rng> &&
		_NoThrowInputIterator<iterator_t<Rng>> &&
		_NoThrowSentinel<sentinel_t<Rng>, iterator_t<Rng>>;
		// Axiom: no exceptions are thrown from calls to begin and end on a Rng.

	///////////////////////////////////////////////////////////////////////////
	// _NoThrowForwardIterator [Exposition]
	//
	template<class I>
	META_CONCEPT _NoThrowForwardIterator =
		_NoThrowInputIterator<I> &&
		forward_iterator<I> &&
		_NoThrowSentinel<I, I>;

	///////////////////////////////////////////////////////////////////////////
	// _NoThrowForwardRange [Exposition]
	//
	template<class Rng>
	META_CONCEPT _NoThrowForwardRange =
		_NoThrowInputRange<Rng> &&
		_NoThrowForwardIterator<iterator_t<Rng>>;
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_CONCEPTS_HPP
