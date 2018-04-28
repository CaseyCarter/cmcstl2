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
#ifndef STL2_DETAIL_MEMORY_UNINITIALIZED_COPY_HPP
#define STL2_DETAIL_MEMORY_UNINITIALIZED_COPY_HPP

#include <new>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/memory/concepts.hpp>
#include <stl2/detail/memory/construct_at.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/span.hpp>
#include <stl2/detail/tagged.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// uninitialized_copy [Extension]
	//
	template <InputIterator I, Sentinel<I> S, __NoThrowForwardIterator O>
	requires
		Constructible<value_type_t<O>, reference_t<I>>
	tagged_pair<tag::in(I), tag::out(O)>
	uninitialized_copy(I first, S last, O result)
	{
		auto guard = detail::destroy_guard<O>{result};
		for (; first != last; ++result, (void)++first) {
			__stl2::__construct_at(*result, *first);
		}
		guard.release();
		return {std::move(first), std::move(result)};
	}

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_copy [Extension]
	//
	template <InputRange Rng, __NoThrowForwardIterator O>
	requires
		Constructible<value_type_t<O>, reference_t<iterator_t<Rng>>>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
	uninitialized_copy(Rng&& rng, O result)
	{
		return __stl2::uninitialized_copy(
			__stl2::begin(rng), __stl2::end(rng), result);
	}

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_copy [Extension]
	//
	template <InputRange IRng, class T>
	requires
		Constructible<T, reference_t<iterator_t<IRng>>>
	tagged_pair<tag::in(safe_iterator_t<IRng>), tag::out(iterator_t<ext::span<T>>)>
	uninitialized_copy(IRng&& irng, ext::span<T> orng)
	{
		STL2_EXPECT(__stl2::size(irng) == __stl2::size(orng));
		return __stl2::uninitialized_copy(
			__stl2::begin(irng), __stl2::end(irng), __stl2::begin(orng));
	}

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_copy_n [Extension]
	//
	template <InputIterator I, __NoThrowForwardIterator O>
	requires
		Constructible<value_type_t<O>, reference_t<I>>
	tagged_pair<tag::in(I), tag::out(O)>
	uninitialized_copy_n(I first, difference_type_t<I> n, O out)
	{
		auto result = __stl2::uninitialized_copy(
			__stl2::make_counted_iterator(first, n),
			default_sentinel{}, out);
		return {result.in().base(), result.out()};
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_UNINITIALIZED_COPY_HPP
