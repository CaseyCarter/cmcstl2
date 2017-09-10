// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_MEMORY_UNINITIALIZED_MOVE_HPP
#define STL2_DETAIL_MEMORY_UNINITIALIZED_MOVE_HPP

#include <new>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/memory/concepts.hpp>
#include <stl2/detail/memory/construct_at.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <stl2/detail/tagged.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// uninitialized_move [Extension]
	//
	template <InputIterator I, Sentinel<I> S, __NoThrowForwardIterator O>
	requires
		Constructible<value_type_t<O>, rvalue_reference_t<I>> &&
		__ReferenceTo<I, value_type_t<O>>
	tagged_pair<tag::in(I), tag::out(O)> uninitialized_move(I first, S last, O result)
	{
		auto guard = detail::destroy_guard<O>{result};
		for (; first != last; (void)++result, ++first) {
			__stl2::__construct_at(*result, __stl2::iter_move(first));
		}
		guard.release();
		return {first, result};
	}

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_move [Extension]
	//
	template <InputRange Rng, __NoThrowForwardIterator O>
	requires
		Constructible<value_type_t<O>, rvalue_reference_t<iterator_t<Rng>>> &&
		__ReferenceTo<iterator_t<Rng>, value_type_t<O>>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
	uninitialized_move(Rng&& rng, O result)
	{
		return __stl2::uninitialized_move(
			__stl2::begin(rng), __stl2::end(rng), std::move(result));
	}

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_move_n [Extension]
	//
	template <InputIterator I, __NoThrowForwardIterator O>
	requires
		Constructible<value_type_t<O>, rvalue_reference_t<I>> &&
		__ReferenceTo<I, value_type_t<I>>
	tagged_pair<tag::in(I), tag::out(O)>
	uninitialized_move_n(I first, difference_type_t<I> n, O result)
	{
		auto r = __stl2::uninitialized_move(
			__stl2::make_counted_iterator(first, n),
			default_sentinel{}, std::move(result));
		return {r.in().base(), r.out()};
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_UNINITIALIZED_MOVE_HPP
