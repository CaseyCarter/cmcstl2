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
#ifndef STL2_DETAIL_ALGORITHM_IS_HEAP_UNTIL_HPP
#define STL2_DETAIL_ALGORITHM_IS_HEAP_UNTIL_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// is_heap_until [is.heap]
//
STL2_OPEN_NAMESPACE {
	namespace detail {
		struct __is_heap_until_n_fn {
			template<RandomAccessIterator I, class Proj = identity,
				IndirectStrictWeakOrder<projected<I, Proj>> Comp = less>
			constexpr I operator()(I first, const iter_difference_t<I> n,
				Comp comp = {}, Proj proj = {}) const
			{
				STL2_EXPECT(0 <= n);
				iter_difference_t<I> p = 0, c = 1;
				I pp = first;
				while (c < n) {
					I cp = first + c;
					if (__stl2::invoke(comp,
							__stl2::invoke(proj, *pp),
							__stl2::invoke(proj, *cp))) {
						return cp;
					}
					++c;
					++cp;
					if (c == n || __stl2::invoke(comp,
							__stl2::invoke(proj, *pp),
							__stl2::invoke(proj, *cp))) {
						return cp;
					}
					++p;
					++pp;
					c = 2 * p + 1;
				}
				return first + n;
			}
		};

		inline constexpr __is_heap_until_n_fn is_heap_until_n {};
	}

	struct __is_heap_until_fn : private __niebloid {
		template<RandomAccessIterator I, Sentinel<I> S, class Proj = identity,
			IndirectStrictWeakOrder<projected<I, Proj>> Comp = less>
		constexpr I
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
			auto n = distance(first, std::move(last));
			return detail::is_heap_until_n(std::move(first), n,
				__stl2::ref(comp), __stl2::ref(proj));
		}

		template<RandomAccessRange Rng, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<Rng>, Proj>> Comp = less>
		constexpr safe_iterator_t<Rng>
		operator()(Rng&& rng, Comp comp = {}, Proj proj = {}) const {
			return detail::is_heap_until_n(begin(rng), distance(rng),
				__stl2::ref(comp), __stl2::ref(proj));
		}
	};

	inline constexpr __is_heap_until_fn is_heap_until {};
} STL2_CLOSE_NAMESPACE

#endif
