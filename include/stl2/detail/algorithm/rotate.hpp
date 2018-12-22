// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
#ifndef STL2_DETAIL_ALGORITHM_ROTATE_HPP
#define STL2_DETAIL_ALGORITHM_ROTATE_HPP

#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/algorithm/move_backward.hpp>
#include <stl2/detail/algorithm/swap_ranges.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/view/subrange.hpp>

///////////////////////////////////////////////////////////////////////////
// rotate [alg.rotate]
//
STL2_OPEN_NAMESPACE {
	template<class I>
	requires
		Permutable<I>
	subrange<I> __rotate_left(I first, I last)
	{
		iter_value_t<I> tmp = iter_move(first);
		I lm1 = __stl2::move(next(first), last, first).second;
		*lm1 = std::move(tmp);
		return {std::move(lm1), std::move(last)};
	}

	template<BidirectionalIterator I>
	requires
		Permutable<I>
	subrange<I> __rotate_right(I first, I last)
	{
		I lm1 = prev(last);
		iter_value_t<I> tmp = iter_move(lm1);
		I fp1 = __stl2::move_backward(first, std::move(lm1), last).second;
		*first = std::move(tmp);
		return {std::move(fp1), std::move(last)};
	}

	template<Permutable I, Sentinel<I> S>
	subrange<I> __rotate_forward(I first, I middle, S last)
	{
		I i = middle;
		while (true) {
			iter_swap(first, i);
			++first;
			if (++i == last) {
				break;
			}
			if (first == middle) {
				middle = i;
			}
		}
		I r = first;
		if (first != middle) {
			I j = middle;
			while (true) {
				iter_swap(first, j);
				++first;
				if (++j == last) {
					if (first == middle) {
						break;
					}
					j = middle;
				} else if(first == middle) {
					middle = j;
				}
			}
		}
		return {std::move(r), std::move(i)};
	}

	template<Integral I>
	constexpr I __gcd(I x, I y)
	{
		do {
			auto t = x % y;
			x = y;
			y = t;
		} while(y);
		return x;
	}

	template<RandomAccessIterator I>
	requires
		Permutable<I>
	subrange<I> __rotate_gcd(I first, I middle, I last)
	{
		using D = iter_difference_t<I>;
		D const m1 = middle - first;
		D const m2 = last - middle;
		if (m1 == m2) {
			__swap_ranges::impl(first, middle, middle);
			return {std::move(middle), std::move(last)};
		}
		auto const g = __stl2::__gcd(m1, m2);
		for (I p = first + g; p != first;) {
			iter_value_t<I> t = iter_move(--p);
			I p1 = p;
			I p2 = p1 + m1;
			do {
				*p1 = iter_move(p2);
				p1 = p2;
				D const d = last - p2;
				if (m1 < d) {
					p2 += m1;
				} else {
					p2 = first + D{m1 - d};
				}
			} while(p2 != p);
			*p1 = std::move(t);
		}
		first += m2;
		return {std::move(first), std::move(last)};
	}

	template<Permutable I, Sentinel<I> S>
	subrange<I> __rotate(I first, I middle, S last)
	{
		return __stl2::__rotate_forward(
			std::move(first), std::move(middle), std::move(last));
	}

	template<Permutable I>
	subrange<I> __rotate(I first, I middle, I last)
	{
		if (std::is_trivially_move_assignable_v<iter_value_t<I>>) {
			if (next(first) == middle) {
				return __stl2::__rotate_left(std::move(first), std::move(last));
			}
		}
		return __stl2::__rotate_forward(
			std::move(first), std::move(middle), std::move(last));
	}

	template<BidirectionalIterator I>
	requires Permutable<I>
	subrange<I> __rotate(I first, I middle, I last)
	{
		if (std::is_trivially_move_assignable_v<iter_value_t<I>>) {
			if (next(first) == middle) {
				return __stl2::__rotate_left(std::move(first), std::move(last));
			}
			if (next(middle) == last) {
				return __stl2::__rotate_right(std::move(first), std::move(last));
			}
		}
		return __stl2::__rotate_forward(
			std::move(first), std::move(middle), std::move(last));
	}

	template<RandomAccessIterator I>
	requires Permutable<I>
	subrange<I> __rotate(I first, I middle, I last)
	{
		if (std::is_trivially_move_assignable_v<iter_value_t<I>>) {
			if (next(first) == middle) {
				return __stl2::__rotate_left(std::move(first), std::move(last));
			}
			if (next(middle) == last) {
				return __stl2::__rotate_right(std::move(first), std::move(last));
			}
			return __stl2::__rotate_gcd(
				std::move(first), std::move(middle), std::move(last));
		}
		return __stl2::__rotate_forward(
			std::move(first), std::move(middle), std::move(last));
	}

	template<Permutable I, Sentinel<I> S>
	subrange<I> rotate(I first, I middle, S last)
	{
		if (first == middle) {
			first = next(std::move(first), std::move(last));
			return {first, std::move(first)};
		}
		if (middle == last) {
			return {std::move(first), std::move(middle)};
		}
		return __stl2::__rotate(
			std::move(first), std::move(middle), std::move(last));
	}

	template<ForwardRange Rng>
	requires
		Permutable<iterator_t<Rng>>
	safe_subrange_t<Rng> rotate(Rng&& rng, iterator_t<Rng> middle)
	{
		return __stl2::rotate(begin(rng), std::move(middle), end(rng));
	}
} STL2_CLOSE_NAMESPACE

#endif
