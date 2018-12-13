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
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
#ifndef STL2_DETAIL_ALGORITHM_PARTITION_POINT_HPP
#define STL2_DETAIL_ALGORITHM_PARTITION_POINT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// partition_point [alg.partitions]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		template<ForwardIterator I, class Pred, class Proj = identity>
		requires
			IndirectUnaryPredicate<
				Pred, projected<I, Proj>>
		I partition_point_n(I first, iter_difference_t<I> n,
			Pred pred, Proj proj = {})
		{
			STL2_EXPECT(0 <= n);
			while (n != 0) {
				auto const half = n / 2;
				auto middle = next(__stl2::ext::uncounted(first), half);
				if (__stl2::invoke(pred, __stl2::invoke(proj, *middle))) {
					first = __stl2::ext::recounted(
										 first, std::move(++middle), half + 1);
					n -= half + 1;
				} else {
					n = half;
				}
			}
			return first;
		}
	}

	template<ForwardIterator I, Sentinel<I> S, class Pred, class Proj = identity>
	requires
		IndirectUnaryPredicate<
			Pred, projected<I, Proj>>
	I partition_point(I first, S last, Pred pred, Proj proj = {})
	{
		// Probe exponentially for either end-of-range or an iterator
		// that is past the partition point (i.e., does not satisfy pred).
		auto n = iter_difference_t<I>{1};
		while (true) {
			auto m = first;
			auto d = __stl2::advance(m, n, last);
			if (m == last || !__stl2::invoke(pred, __stl2::invoke(proj, *m))) {
				n -= d;
				return ext::partition_point_n(std::move(first), n,
					std::move(pred), std::move(proj));
			}
			first = std::move(m);
			n *= 2;
		}
	}

	template<ForwardIterator I, Sentinel<I> S, class Pred, class Proj = identity>
	requires
		SizedSentinel<S, I> &&
		IndirectUnaryPredicate<
			Pred, projected<I, Proj>>
	I partition_point(I first, S last, Pred pred, Proj proj = {})
	{
		auto n = __stl2::distance(first, std::move(last));
		return __stl2::ext::partition_point_n(std::move(first), n,
			__stl2::ref(pred), __stl2::ref(proj));
	}

	template<ForwardRange Rng, class Pred, class Proj = identity>
	requires
		IndirectUnaryPredicate<
			Pred, projected<iterator_t<Rng>, Proj>>
	safe_iterator_t<Rng>
	partition_point(Rng&& rng, Pred pred, Proj proj = {})
	{
		return __stl2::partition_point(begin(rng), end(rng),
			__stl2::ref(pred), __stl2::ref(proj));
	}

	template<ForwardRange Rng, class Pred, class Proj = identity>
	requires
		SizedRange<Rng> &&
		IndirectUnaryPredicate<
			Pred, projected<iterator_t<Rng>, Proj>>
	safe_iterator_t<Rng>
	partition_point(Rng&& rng, Pred pred, Proj proj = {})
	{
		return ext::partition_point_n(begin(rng), __stl2::distance(rng),
			__stl2::ref(pred), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
