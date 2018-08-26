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
	class __pop_heap_fn {
		template <RandomAccessIterator I, class Proj, class Comp>
		requires
			Sortable<I, Comp, Proj>
		static constexpr void pop_heap_n(I first, iter_difference_t<I> n, Comp comp, Proj proj)
		{
			if (n > 1) {
				__stl2::iter_swap(first, first + (n - 1));
				__stl2::detail::sift_down_n(first, n - 1, first, std::ref(comp),
					std::ref(proj));
			}
		}

		friend class __sort_heap_fn;
	public:
		template<RandomAccessIterator I, Sentinel<I> S, class Comp = ranges::less<>,
			class Proj = identity>
		requires Sortable<I, Comp, Proj>
		constexpr I operator()(I first, S last, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			auto n = __stl2::distance(first, std::move(last));
			__pop_heap_fn::pop_heap_n(first, n, std::ref(comp), std::ref(proj));
			return first + n;
		}

		template<RandomAccessRange R, class Comp = ranges::less<>, class Proj = identity>
		requires Sortable<iterator_t<R>, Comp, Proj>
		constexpr safe_iterator_t<R> operator()(R&& r, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			auto n = __stl2::distance(r);
			__pop_heap_fn::pop_heap_n(__stl2::begin(r), n, std::ref(comp), std::ref(proj));
			return __stl2::begin(r) + n;
		}
	};

	inline constexpr __pop_heap_fn pop_heap {};
} STL2_CLOSE_NAMESPACE

#endif
