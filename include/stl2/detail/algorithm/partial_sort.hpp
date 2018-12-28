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
#ifndef STL2_DETAIL_ALGORITHM_PARTIAL_SORT_HPP
#define STL2_DETAIL_ALGORITHM_PARTIAL_SORT_HPP

#include <stl2/detail/algorithm/heap_sift.hpp>
#include <stl2/detail/algorithm/make_heap.hpp>
#include <stl2/detail/algorithm/sort_heap.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// partial_sort [partial.sort]
//
STL2_OPEN_NAMESPACE {
	struct __partial_sort_fn : private __niebloid {
		template<RandomAccessIterator I, Sentinel<I> S, class Comp = less,
			class Proj = identity>
		requires Sortable<I, Comp, Proj>
		constexpr I operator()(I first, I middle, S last, Comp comp = {},
			Proj proj = {}) const
		{
			make_heap(first, middle, __stl2::ref(comp), __stl2::ref(proj));
			const auto len = distance(first, middle);
			I i = middle;
			for(; i != last; ++i) {
				if(__stl2::invoke(comp,
						__stl2::invoke(proj, *i),
						__stl2::invoke(proj, *first))) {
					iter_swap(i, first);
					detail::sift_down_n(first, len, first, __stl2::ref(comp),
						__stl2::ref(proj));
				}
			}
			__stl2::sort_heap(first, middle, __stl2::ref(comp),
				__stl2::ref(proj));
			return i;
		}

		template<RandomAccessRange R, class Comp = less, class Proj = identity>
		requires Sortable<iterator_t<R>, Comp, Proj>
		constexpr safe_iterator_t<R> operator()(R&& r, iterator_t<R> middle,
			Comp comp = {}, Proj proj = {}) const
		{
			return (*this)(begin(r), std::move(middle), end(r),
				__stl2::ref(comp), __stl2::ref(proj));
		}
	};

	inline constexpr __partial_sort_fn partial_sort {};
} STL2_CLOSE_NAMESPACE

#endif
