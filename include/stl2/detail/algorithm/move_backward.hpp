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
#ifndef STL2_DETAIL_ALGORITHM_MOVE_BACKWARD_HPP
#define STL2_DETAIL_ALGORITHM_MOVE_BACKWARD_HPP

#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>

///////////////////////////////////////////////////////////////////////////
// move_backward [alg.move]
//
STL2_OPEN_NAMESPACE {
	template <BidirectionalIterator I1, BidirectionalIterator I2>
	requires
		IndirectlyMovable<I1, I2>
	tagged_pair<tag::in(I1), tag::out(I2)>
	move_backward(I1 first, I1 last, I2 result)
	{
		auto i = last;
		while (i != first) {
			*--result = __stl2::iter_move(--i);
		}
		return {std::move(last), std::move(result)};
	}

	template <BidirectionalIterator I1, Sentinel<I1> S1, class I2>
	requires
		BidirectionalIterator<__f<I2>> &&
		IndirectlyMovable<I1, __f<I2>>
	tagged_pair<tag::in(I1), tag::out(__f<I2>)>
	move_backward(I1 first, S1 s, I2&& out)
	{
		auto last = __stl2::next(first, std::move(s));
		return __stl2::move_backward(
			std::move(first), std::move(last), std::forward<I2>(out));
	}

	template <BidirectionalRange Rng, class I>
	requires
		BidirectionalIterator<__f<I>> &&
		IndirectlyMovable<iterator_t<Rng>, __f<I>>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<I>)>
	move_backward(Rng&& rng, I&& result)
	{
		return __stl2::move_backward(
			__stl2::begin(rng), __stl2::end(rng), std::forward<I>(result));
	}
} STL2_CLOSE_NAMESPACE

#endif
