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
#ifndef STL2_DETAIL_ALGORITHM_COPY_HPP
#define STL2_DETAIL_ALGORITHM_COPY_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>

///////////////////////////////////////////////////////////////////////////
// copy [alg.copy]
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O>
	requires
		models::IndirectlyCopyable<I, O>
	tagged_pair<tag::in(I), tag::out(O)>
	copy(I first, S last, O result)
	{
		for (; first != last; ++first, ++result) {
			*result = *first;
		}
		return {__stl2::move(first), __stl2::move(result)};
	}

	template <InputRange Rng, class O>
	requires
		models::WeaklyIncrementable<__f<O>> &&
		models::IndirectlyCopyable<iterator_t<Rng>, __f<O>>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
	copy(Rng&& rng, O&& result)
	{
		return __stl2::copy(__stl2::begin(rng), __stl2::end(rng),
												__stl2::forward<O>(result));
	}

	// Extension
	template <class E, class O>
	requires
		models::WeaklyIncrementable<__f<O>> &&
		models::IndirectlyCopyable<const E*, __f<O>>
	tagged_pair<tag::in(dangling<const E*>), tag::out(__f<O>)>
	copy(std::initializer_list<E>&& rng, O&& result)
	{
		return __stl2::copy(rng, __stl2::forward<O>(result));
	}

	// Extension: two-range copy
	namespace ext {
		template <InputIterator I1, Sentinel<I1> S1, Iterator I2, Sentinel<I2> S2>
		requires
			models::IndirectlyCopyable<I1, I2>
		tagged_pair<tag::in(I1), tag::out(I2)>
		copy(I1 first, S1 last, I2 result_first, S2 result_last)
		{
			for (; first != last && result_first != result_last; ++first, ++result_first) {
				*result_first = *first;
			}
			return {__stl2::move(first), __stl2::move(result_first)};
		}

		template <InputRange Rng1, Range Rng2>
		requires
			models::IndirectlyCopyable<iterator_t<Rng1>, iterator_t<Rng2>>
		tagged_pair<tag::in(safe_iterator_t<Rng1>), tag::out(safe_iterator_t<Rng2>)>
		copy(Rng1&& rng1, Rng2&& rng2)
		{
			return ext::copy(__stl2::begin(rng1), __stl2::end(rng1),
											 __stl2::begin(rng2), __stl2::end(rng2));
		}

		template <class E, Range Rng2>
		requires
			models::IndirectlyCopyable<const E*, iterator_t<Rng2>>
		tagged_pair<tag::in(dangling<const E*>), tag::out(safe_iterator_t<Rng2>)>
		copy(std::initializer_list<E>&& rng1, Rng2&& rng2)
		{
			return ext::copy(rng1, rng2);
		}
	}
} STL2_CLOSE_NAMESPACE

#endif
