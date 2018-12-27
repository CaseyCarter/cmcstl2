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
#ifndef STL2_DETAIL_ALGORITHM_MAKE_HEAP_HPP
#define STL2_DETAIL_ALGORITHM_MAKE_HEAP_HPP

#include <stl2/detail/algorithm/heap_sift.hpp>
#include <stl2/detail/range/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// make_heap [make.heap]
//
STL2_OPEN_NAMESPACE {
	struct __make_heap_fn : private __niebloid {
		template<RandomAccessIterator I, Sentinel<I> S, class Comp = less,
			class Proj = identity>
		requires Sortable<I, Comp, Proj>
		constexpr I
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
			auto n = distance(first, std::move(last));
			make_heap_n(first, n, __stl2::ref(comp), __stl2::ref(proj));
			return first + n;
		}

		template<RandomAccessRange Rng, class Comp = less, class Proj = identity>
		requires Sortable<iterator_t<Rng>, Comp, Proj>
		constexpr safe_iterator_t<Rng>
		operator()(Rng&& rng, Comp comp = {}, Proj proj = {}) const {
			auto n = distance(rng);
			make_heap_n(begin(rng), n, __stl2::ref(comp), __stl2::ref(proj));
			return begin(rng) + n;
		}
	private:
		template<RandomAccessIterator I, class Comp, class Proj>
		requires Sortable<I, Comp, Proj>
		static void make_heap_n(I first, iter_difference_t<I> n, Comp comp, Proj proj) {
			if (n > 1) {
				// start from the first parent, there is no need to consider children
				for (auto start = (n - 2) / 2; start >= 0; --start) {
					detail::sift_down_n(first, n, first + start,
						__stl2::ref(comp), __stl2::ref(proj));
				}
			}
		}
	};

	inline constexpr __make_heap_fn make_heap {};
} STL2_CLOSE_NAMESPACE

#endif
