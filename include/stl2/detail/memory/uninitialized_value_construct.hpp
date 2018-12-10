// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_HPP
#define STL2_DETAIL_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/memory/concepts.hpp>
#include <stl2/detail/memory/construct_at.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <stl2/detail/tagged.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// uninitialized_value_construct [Extension]
	//
	template<__NoThrowForwardIterator I, Sentinel<I> S>
	requires
		DefaultConstructible<iter_value_t<I>>
	I uninitialized_value_construct(I first, S last)
	{
		auto guard = detail::destroy_guard<I>{first};
		for (; first != last; ++first) {
			__stl2::__construct_at(*first);
		}
		guard.release();
		return first;
	}

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_value_construct [Extension]
	//
	template<__NoThrowForwardRange Rng>
	requires
		DefaultConstructible<iter_value_t<iterator_t<Rng>>>
	safe_iterator_t<Rng> uninitialized_value_construct(Rng&& rng)
	{
		return __stl2::uninitialized_value_construct(begin(rng), end(rng));
	}

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_value_construct_n [Extension]
	//
	template<__NoThrowForwardIterator I>
	requires
		DefaultConstructible<iter_value_t<I>>
	I uninitialized_value_construct_n(I first, iter_difference_t<I> n)
	{
		return __stl2::uninitialized_value_construct(
			__stl2::make_counted_iterator(first, n),
			default_sentinel{}).base();
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_UNINITIALIZED_VALUE_CONSTRUCT_HPP
