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

#include <stl2/detail/algorithm/find_if_not.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// partition [alg.partitions]
//
STL2_OPEN_NAMESPACE {
	struct __partition_fn : private __niebloid {
		template<Permutable I, Sentinel<I> S, class Proj = identity,
			IndirectUnaryPredicate<projected<I, Proj>> Pred>
		constexpr I operator()(I first, S last_, Pred pred, Proj proj = {}) const {
			if constexpr (BidirectionalIterator<I>) {
				auto last = next(first, std::move(last_));

				for (; first != last; ++first) {
					if (!__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
						while (true) {
							if (--last == first) {
								return first;
							}
							if (__stl2::invoke(pred, __stl2::invoke(proj, *last))) {
								iter_swap(first, last);
								break;
							}
						}
					}
				}
			} else {
				first = find_if_not(std::move(first), last_, __stl2::ref(pred),
					__stl2::ref(proj));
				if (first != last_) {
					for (auto m = first; ++m != last_;) {
						if (__stl2::invoke(pred, __stl2::invoke(proj, *m))) {
							iter_swap(first, m);
							++first;
						}
					}
				}
			}
			return first;
		}

		template<ForwardRange R, class Proj = identity,
			IndirectUnaryPredicate<projected<iterator_t<R>, Proj>> Pred>
		requires Permutable<iterator_t<R>>
		constexpr safe_iterator_t<R>
		operator()(R&& r, Pred pred, Proj proj = {}) const {
			return (*this)(begin(r), end(r), __stl2::ref(pred),
				__stl2::ref(proj));
		}
	};

	inline constexpr __partition_fn partition {};
} STL2_CLOSE_NAMESPACE

#endif
