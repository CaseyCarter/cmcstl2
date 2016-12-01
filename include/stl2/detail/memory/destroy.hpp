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
#ifndef STL2_DETAIL_MEMORY_DESTROY_HPP
#define STL2_DETAIL_MEMORY_DESTROY_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/iterator/dangling.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
//#include <stl2/detail/memory/addressof.hpp> // FIXME: after merge
#include <stl2/detail/memory/reference_to.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// destroy_at [Extension]
	//
	template <Destructible T>
	void destroy_at(T* p) noexcept
	{
		p->~T();
	}

	///////////////////////////////////////////////////////////////////////////
	// destroy_at [Extension]
	//
	template <Destructible T>
	void destroy_at(T& p) noexcept
	{
		p.~T();
	}

	///////////////////////////////////////////////////////////////////////////
	// destroy [Extension]
	//
	template <InputIterator I, Sentinel<I> S>
	requires
		Destructible<value_type_t<I>>() &&
		__ReferenceTo<I, value_type_t<I>>()
	I destroy(I first, S last) noexcept
	{
		for (; first != last; ++first) {
			__stl2::destroy_at(__stl2::addressof(*first));
		}

		return first;
	}

	template <InputRange Rng>
	requires
		Destructible<value_type_t<iterator_t<Rng>>>() &&
		__ReferenceTo<iterator_t<Rng>, value_type_t<iterator_t<Rng>>>()
	safe_iterator_t<Rng> destroy(Rng&& rng) noexcept
	{
		return __stl2::destroy(__stl2::begin(rng), __stl2::end(rng));
	}

	///////////////////////////////////////////////////////////////////////////
	// destroy_n [Extension]
	//
	template <InputIterator I>
	requires
		Destructible<value_type_t<I>>() &&
		__ReferenceTo<I, value_type_t<I>>()
	I destroy_n(I first, difference_type_t<I> n) noexcept
	{
		return __stl2::destroy(__stl2::make_counted_iterator(std::move(first), n),
			default_sentinel{}).base();
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_DESTROY_HPP
