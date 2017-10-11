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
#ifndef STL2_DETAIL_ALGORITHM_SWAP_RANGES_HPP
#define STL2_DETAIL_ALGORITHM_SWAP_RANGES_HPP

#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>

///////////////////////////////////////////////////////////////////////////
// swap_ranges [alg.swap]
//
STL2_OPEN_NAMESPACE {
	namespace __swap_ranges {
		template <ForwardIterator I1, Sentinel<I1> S1, ForwardIterator I2>
		requires
			IndirectlySwappable<I1, I2>
		constexpr tagged_pair<tag::in1(I1), tag::in2(I2)>
		impl(I1 first1, S1 last1, I2 first2)
		{
			for (; first1 != last1; ++first1, void(++first2)) {
				__stl2::iter_swap(first1, first2);
			}
			return {std::move(first1), std::move(first2)};
		}
	}

	template <ForwardIterator I1, Sentinel<I1> S1, class I2>
	[[deprecated]] tagged_pair<tag::in1(I1), tag::in2(std::decay_t<I2>)>
	swap_ranges(I1 first1, S1 last1, I2&& first2_)
	requires ForwardIterator<std::decay_t<I2>> && !Range<I2> &&
		IndirectlySwappable<I1, std::decay_t<I2>>
	{
		return __swap_ranges::impl(
			std::move(first1), std::move(last1), std::forward<I2>(first2_));
	}

	template <ForwardRange Rng, class I>
	[[deprecated]] tagged_pair<tag::in1(safe_iterator_t<Rng>), tag::in2(std::decay_t<I>)>
	swap_ranges(Rng&& rng1, I&& first2_)
	requires ForwardIterator<std::decay_t<I>> && !Range<I> &&
		IndirectlySwappable<iterator_t<Rng>, std::decay_t<I>>
	{
		auto first2 = std::forward<I>(first2_);
		return __swap_ranges::impl(__stl2::begin(rng1), __stl2::end(rng1), std::move(first2));
	}

	template <ForwardIterator I1, Sentinel<I1> S1,
		ForwardIterator I2, Sentinel<I2> S2>
	requires
		IndirectlySwappable<I1, I2>
	tagged_pair<tag::in1(I1), tag::in2(I2)>
	swap_ranges(I1 first1, S1 last1, I2 first2, S2 last2)
	{
		for (; first1 != last1 && first2 != last2; ++first1, void(++first2)) {
			__stl2::iter_swap(first1, first2);
		}
		return {std::move(first1), std::move(first2)};
	}

	template <ForwardRange Rng1, ForwardRange Rng2>
	requires
		IndirectlySwappable<iterator_t<Rng1>, iterator_t<Rng2>>
	tagged_pair<tag::in1(safe_iterator_t<Rng1>),
							tag::in2(safe_iterator_t<Rng2>)>
	swap_ranges(Rng1&& rng1, Rng2&& rng2)
	{
		return __stl2::swap_ranges(
			__stl2::begin(rng1), __stl2::end(rng1),
			__stl2::begin(rng2), __stl2::end(rng2));
	}
} STL2_CLOSE_NAMESPACE

#endif
