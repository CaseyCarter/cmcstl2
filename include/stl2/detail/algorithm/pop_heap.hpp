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
#ifndef STL2_DETAIL_ALGORITHM_POP_HEAP_HPP
#define STL2_DETAIL_ALGORITHM_POP_HEAP_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/heap_sift.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// pop_heap [pop.heap]
//
STL2_OPEN_NAMESPACE {
	namespace detail {
		template <RandomAccessIterator I, class Proj, class Comp>
		requires
			Sortable<I, Comp, Proj>
		void pop_heap_n(I first, iter_difference_t<I> n, Comp comp, Proj proj)
		{
			if (n > 1) {
				__stl2::iter_swap(first, first + (n - 1));
				detail::sift_down_n(first, n - 1, first, std::ref(comp),
					std::ref(proj));
			}
		}
	}

	template <RandomAccessIterator I, Sentinel<I> S, class Comp = less<>,
						class Proj = identity>
	requires
		Sortable<I, Comp, Proj>
	I pop_heap(I first, S last, Comp comp = Comp{}, Proj proj = Proj{})
	{
		auto n = __stl2::distance(first, std::move(last));
		detail::pop_heap_n(first, n, std::ref(comp), std::ref(proj));
		return first + n;
	}

	template <RandomAccessRange Rng, class Comp = less<>, class Proj = identity>
	requires
		Sortable<iterator_t<Rng>, Comp, Proj>
	safe_iterator_t<Rng>
	pop_heap(Rng&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		auto n = __stl2::distance(rng);
		detail::pop_heap_n(__stl2::begin(rng), n, std::ref(comp), std::ref(proj));
		return __stl2::begin(rng) + n;
	}
} STL2_CLOSE_NAMESPACE

#endif
