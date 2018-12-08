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
#ifndef STL2_DETAIL_ALGORITHM_RANDOM_ACCESS_SORT_HPP
#define STL2_DETAIL_ALGORITHM_RANDOM_ACCESS_SORT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/tuple.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/move_backward.hpp>
#include <stl2/detail/algorithm/partial_sort.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/fundamental.hpp>

///////////////////////////////////////////////////////////////////////////
// sort [sort]
//
STL2_OPEN_NAMESPACE {
	namespace detail {
		namespace rsort {
			template<RandomAccessIterator I, class Comp, class Proj>
			requires
				Sortable<I, Comp, Proj>
			I choose_pivot(I first, I last, Comp& comp, Proj& proj)
			{
				STL2_EXPECT(first != last);
				I mid = first + iter_difference_t<I>(last - first) / 2;
				--last;
				// Find the median:
				return [&](auto&& a, auto&& b, auto&& c) {
					return __stl2::invoke(comp, a, b)
						? (__stl2::invoke(comp, b, c) ? mid   : (__stl2::invoke(comp, a, c) ? last : first))
						: (__stl2::invoke(comp, a, c) ? first : (__stl2::invoke(comp, b, c) ? last : mid  ));
				}(__stl2::invoke(proj, *first), __stl2::invoke(proj, *mid), __stl2::invoke(proj, *last));
			}

			template<RandomAccessIterator I, class Comp, class Proj>
			requires
				Sortable<I, Comp, Proj>
			I unguarded_partition(I first, I last, Comp& comp, Proj& proj)
			{
				I pivot_pnt = rsort::choose_pivot(first, last, comp, proj);

				// Do the partition:
				while (true) {
					auto &&v = *pivot_pnt;
					auto &&pivot = __stl2::invoke(proj, (decltype(v) &&)v);
					while (__stl2::invoke(comp, __stl2::invoke(proj, *first), pivot)) {
						++first;
					}
					while (__stl2::invoke(comp, pivot, __stl2::invoke(proj, *--last))) {
						;
					}
					if (!(first < last)) {
						return first;
					}
					__stl2::iter_swap(first, last);
					pivot_pnt = pivot_pnt == first ? last : (pivot_pnt == last ? first : pivot_pnt);
					++first;
				}
			}

			template<BidirectionalIterator I, class Comp, class Proj>
			requires
				Sortable<I, Comp, Proj>
			void unguarded_linear_insert(I last, iter_value_t<I> val, Comp& comp, Proj& proj)
			{
				I next = __stl2::prev(last);
				while (__stl2::invoke(comp, __stl2::invoke(proj, val), __stl2::invoke(proj, *next))) {
					*last = __stl2::iter_move(next);
					last = next;
					--next;
				}
				*last = std::move(val);
			}

			template<BidirectionalIterator I, class Comp, class Proj>
			requires
				Sortable<I, Comp, Proj>
			void linear_insert(I first, I last, Comp& comp, Proj& proj)
			{
				iter_value_t<I> val = __stl2::iter_move(last);
				if (__stl2::invoke(comp, __stl2::invoke(proj, val), __stl2::invoke(proj, *first))) {
					__stl2::move_backward(first, last, last + 1);
					*first = std::move(val);
				} else {
					rsort::unguarded_linear_insert(last, std::move(val), comp, proj);
				}
			}

			template<BidirectionalIterator I, class Comp, class Proj>
			requires
				Sortable<I, Comp, Proj>
			void insertion_sort(I first, I last, Comp& comp, Proj& proj)
			{
				if (first != last) {
					for (I i = __stl2::next(first); i != last; ++i) {
						rsort::linear_insert(first, i, comp, proj);
					}
				}
			}

			template<BidirectionalIterator I, class Comp, class Proj>
			requires
				Sortable<I, Comp, Proj>
			void unguarded_insertion_sort(I first, I last, Comp& comp, Proj& proj)
			{
				for (I i = first; i != last; ++i) {
					rsort::unguarded_linear_insert(i, __stl2::iter_move(i), comp, proj);
				}
			}

			constexpr std::ptrdiff_t introsort_threshold = 16;

			template <Integral I>
			constexpr auto log2(I n) {
				STL2_EXPECT(n > 0);
				I k = 0;
				for (; n != 1; n /= 2) {
					++k;
				}
				return k;
			}

			template<RandomAccessIterator I, class Comp, class Proj>
			requires
				Sortable<I, Comp, Proj>
			void introsort_loop(I first, I last, iter_difference_t<I> depth_limit,
				Comp& comp, Proj& proj)
			{
				while (__stl2::distance(first, last) > introsort_threshold) {
					if (depth_limit == 0) {
						__stl2::partial_sort(first, last, last, std::ref(comp), std::ref(proj));
						return;
					}
					I cut = rsort::unguarded_partition(first, last, comp, proj);
					rsort::introsort_loop(cut, last, --depth_limit, comp, proj);
					last = cut;
				}
			}

			template<RandomAccessIterator I, class Comp, class Proj>
			requires
				Sortable<I, Comp, Proj>
			void final_insertion_sort(I first, I last, Comp &comp, Proj &proj)
			{
				if (__stl2::distance(first, last) > introsort_threshold) {
					rsort::insertion_sort(first, first + introsort_threshold, comp, proj);
					rsort::unguarded_insertion_sort(first + introsort_threshold, last, comp, proj);
				} else {
					rsort::insertion_sort(first, last, comp, proj);
				}
			}
		}
	}
} STL2_CLOSE_NAMESPACE

#endif
