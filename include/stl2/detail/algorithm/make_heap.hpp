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

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/heap_sift.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// make_heap [make.heap]
//
STL2_OPEN_NAMESPACE {
	namespace detail {
		template <RandomAccessIterator I, class Comp, class Proj>
		requires
			models::Sortable<I, __f<Comp>, __f<Proj>>
		void make_heap_n(I first, difference_type_t<I> n, Comp&& comp_, Proj&& proj_)
		{
			if (n > 1) {
				auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
				auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
				// start from the first parent, there is no need to consider children
				for (auto start = (n - 2) / 2; start >= 0; --start) {
					detail::sift_down_n(first, n, first + start,
						__stl2::ref(comp), __stl2::ref(proj));
				}
			}
		}
	}

	template <RandomAccessIterator I, Sentinel<I> S, class Comp = less<>,
		class Proj = identity>
	requires
		models::Sortable<I, __f<Comp>, __f<Proj>>
	I make_heap(I first, S last, Comp&& comp = Comp{}, Proj&& proj = Proj{})
	{
		auto n = __stl2::distance(first, __stl2::move(last));
		detail::make_heap_n(first, n, __stl2::forward<Comp>(comp),
			__stl2::forward<Proj>(proj));
		return first + n;
	}

	template <RandomAccessRange Rng, class Comp = less<>, class Proj = identity>
	requires
		models::Sortable<iterator_t<Rng>, __f<Comp>, __f<Proj>>
	safe_iterator_t<Rng>
	make_heap(Rng&& rng, Comp&& comp = Comp{}, Proj&& proj = Proj{})
	{
		auto n = __stl2::distance(rng);
		detail::make_heap_n(__stl2::begin(rng), n, __stl2::forward<Comp>(comp),
			__stl2::forward<Proj>(proj));
		return __stl2::begin(rng) + n;
	}
} STL2_CLOSE_NAMESPACE

#endif
