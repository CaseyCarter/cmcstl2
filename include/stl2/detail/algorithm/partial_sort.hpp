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

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/heap_sift.hpp>
#include <stl2/detail/algorithm/make_heap.hpp>
#include <stl2/detail/algorithm/sort_heap.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// partial_sort [partial.sort]
//
STL2_OPEN_NAMESPACE {
	struct __partial_sort_fn {
		template<RandomAccessIterator I, Sentinel<I> S, class Comp = ranges::less<>,
			class Proj = identity>
		requires Sortable<I, Comp, Proj>
		constexpr I operator()(I first, I middle, S last, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			__stl2::make_heap(first, middle, std::ref(comp), std::ref(proj));
			const auto len = __stl2::distance(first, middle);
			I i = middle;
			for(; i != last; ++i) {
				if(__stl2::invoke(comp, __stl2::invoke(proj, *i), __stl2::invoke(proj, *first))) {
					__stl2::iter_swap(i, first);
					__stl2::detail::sift_down_n(first, len, first, std::ref(comp), std::ref(proj));
				}
			}
			__stl2::sort_heap(first, middle, std::ref(comp), std::ref(proj));
			return i;
		}

		template<RandomAccessRange R, class Comp = ranges::less<>, class Proj = identity>
		requires Sortable<iterator_t<R>, Comp, Proj>
		constexpr safe_iterator_t<R>
		operator()(R&& r, iterator_t<R> middle, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return (*this)(
				__stl2::begin(r), std::move(middle), __stl2::end(r),
				std::ref(comp), std::ref(proj));
		}
	};

	inline constexpr __partial_sort_fn partial_sort {};
} STL2_CLOSE_NAMESPACE

#endif
