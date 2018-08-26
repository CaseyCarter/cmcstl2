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
#ifndef STL2_DETAIL_ALGORITHM_PARTITION_HPP
#define STL2_DETAIL_ALGORITHM_PARTITION_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/find_if_not.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// partition [alg.partitions]
//
STL2_OPEN_NAMESPACE {
	class __partition_fn {
		template <Permutable I, Sentinel<I> S, class Pred, class Proj = identity>
		requires IndirectUnaryPredicate<Pred, projected<I, Proj>>
		static constexpr I impl(I first, S last, Pred pred, Proj proj = Proj{})
		{
			first = __stl2::find_if_not(std::move(first), last,
				std::ref(pred), std::ref(proj));
			if (first != last) {
				for (auto m = first; ++m != last;) {
					if (__stl2::invoke(pred, __stl2::invoke(proj, *m))) {
						__stl2::iter_swap(first, m);
						++first;
					}
				}
			}
			return first;
		}

		template <BidirectionalIterator I, Sentinel<I> S, class Pred, class Proj = identity>
		requires Permutable<I> &&
			IndirectUnaryPredicate<Pred, projected<I, Proj>>
		static constexpr I impl(I first, S last_, Pred pred, Proj proj = Proj{})
		{
			auto last = __stl2::next(first, std::move(last_));

			for (; first != last; ++first) {
				if (!__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
					while (true) {
						if (--last == first) {
							return first;
						}
						if (__stl2::invoke(pred, __stl2::invoke(proj, *last))) {
							__stl2::iter_swap(first, last);
							break;
						}
					}
				}
			}
			return first;
		}
	public:
		template<ForwardIterator I, Sentinel<I> S, class Proj = identity,
			IndirectUnaryPredicate<projected<I, Proj>> Pred>
		requires Permutable<I>
		constexpr I operator()(I first, S last, Pred pred, Proj proj = Proj{}) const
		{
			return __partition_fn::impl(std::move(first), std::move(last),
				std::ref(pred), std::ref(proj));
		}

		template<ForwardRange Rng, class Proj = identity,
			IndirectUnaryPredicate<projected<iterator_t<Rng>, Proj>> Pred>
		requires Permutable<iterator_t<Rng>>
		constexpr safe_iterator_t<Rng> operator()(Rng&& rng, Pred pred, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), std::ref(pred), std::ref(proj));
		}
	};

	inline constexpr __partition_fn partition {};
} STL2_CLOSE_NAMESPACE

#endif
