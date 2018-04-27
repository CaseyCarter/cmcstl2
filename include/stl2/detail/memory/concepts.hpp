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
	template <class I>
	concept bool __NoThrowInputIterator =
		InputIterator<I> &&
		_Is<reference_t<I>, std::is_lvalue_reference> &&
		Same<__uncvref<reference_t<I>>, value_type_t<I>>;
		// Axiom: no exceptions are thrown from increment, assignment, comparison,
		//        indirection through valid iterators, or comparisons with valid sentinels.

	///////////////////////////////////////////////////////////////////////////
	// __NoThrowInputRange [Exposition]
	//
	template <class Rng>
	concept bool __NoThrowInputRange =
		Range<Rng> &&
		__NoThrowInputIterator<iterator_t<Rng>>;

	///////////////////////////////////////////////////////////////////////////
	// __NoThrowForwardIterator [Exposition]
	//
	template <class I>
	concept bool __NoThrowForwardIterator =
		__NoThrowInputIterator<I> &&
		ForwardIterator<I>;

	///////////////////////////////////////////////////////////////////////////
	// __NoThrowForwardRange [Exposition]
	//
	template <class Rng>
	concept bool __NoThrowForwardRange =
		__NoThrowInputRange<Rng> &&
		ForwardRange<Rng>;
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_CONCEPTS_HPP
