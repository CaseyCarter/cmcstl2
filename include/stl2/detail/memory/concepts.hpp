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
	// __NoThrowInputIterator [Exposition]
	//
	template<class I>
	META_CONCEPT __NoThrowInputIterator =
		InputIterator<I> &&
		std::is_lvalue_reference_v<iter_reference_t<I>> &&
		Same<__uncvref<iter_reference_t<I>>, iter_value_t<I>>;
		// Axiom: no exceptions are thrown from increment, copy, move, assignment,
		//        indirection through valid iterators.

	///////////////////////////////////////////////////////////////////////////
	// __NoThrowSentinel [Exposition]
	//
	template<class S, class I>
	META_CONCEPT __NoThrowSentinel =
		Sentinel<S, I>;
		// Axiom: no exceptions are thrown from comparisons between objects of types
		//        I and S.

	///////////////////////////////////////////////////////////////////////////
	// __NoThrowInputRange [Exposition]
	//
	template<class Rng>
	META_CONCEPT __NoThrowInputRange =
		Range<Rng> &&
		__NoThrowInputIterator<iterator_t<Rng>> &&
		__NoThrowSentinel<sentinel_t<Rng>, iterator_t<Rng>>;
		// Axiom: no exceptions are thrown from calls to begin and end on a Rng.

	///////////////////////////////////////////////////////////////////////////
	// __NoThrowForwardIterator [Exposition]
	//
	template<class I>
	META_CONCEPT __NoThrowForwardIterator =
		__NoThrowInputIterator<I> &&
		__NoThrowSentinel<I, I> &&
		ForwardIterator<I>;

	///////////////////////////////////////////////////////////////////////////
	// __NoThrowForwardRange [Exposition]
	//
	template<class Rng>
	META_CONCEPT __NoThrowForwardRange =
		__NoThrowForwardIterator<iterator_t<Rng>> &&
		__NoThrowInputRange<Rng> &&
		ForwardRange<Rng>;
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_CONCEPTS_HPP
