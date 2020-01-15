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
//  Copyright (c) 1994
//  Hewlett-Packard Company
//
//  Permission to use, copy, modify, distribute and sell this software
//  and its documentation for any purpose is hereby granted without fee,
//  provided that the above copyright notice appear in all copies and
//  that both that copyright notice and this permission notice appear
//  in supporting documentation.  Hewlett-Packard Company makes no
//  representations about the suitability of this software for any
//  purpose.  It is provided "as is" without express or implied warranty.
//
//  Copyright (c) 1996
//  Silicon Graphics Computer Systems, Inc.
//
//  Permission to use, copy, modify, distribute and sell this software
//  and its documentation for any purpose is hereby granted without fee,
//  provided that the above copyright notice appear in all copies and
//  that both that copyright notice and this permission notice appear
//  in supporting documentation.  Silicon Graphics makes no
//  representations about the suitability of this software for any
//  purpose.  It is provided "as is" without express or implied warranty.
//
#ifndef STL2_DETAIL_ALGORITHM_SORT_HPP
#define STL2_DETAIL_ALGORITHM_SORT_HPP

#include <stl2/detail/algorithm/move_backward.hpp>
#include <stl2/detail/algorithm/partial_sort.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// sort [sort]
//
STL2_OPEN_NAMESPACE {
	namespace detail {
		struct rsort {
			template<bidirectional_iterator I, class Comp, class Proj>
			requires sortable<I, Comp, Proj>
			static constexpr void
			unguarded_linear_insert(I last, iter_value_t<I> val, Comp& comp, Proj& proj)
			{
				I next = prev(last);
				while (__stl2::invoke(comp, __stl2::invoke(proj, val), __stl2::invoke(proj, *next))) {
					*last = iter_move(next);
					last = next;
					--next;
				}
				*last = std::move(val);
			}
			template<bidirectional_iterator I, class Comp, class Proj>
			requires sortable<I, Comp, Proj>
			static constexpr void insertion_sort(I first, I last, Comp& comp, Proj& proj)
			{
				if (first != last) {
					for (I i = next(first); i != last; ++i) {
						linear_insert(first, i, comp, proj);
					}
				}
			}
		private:
			template<bidirectional_iterator I, class Comp, class Proj>
			requires sortable<I, Comp, Proj>
			static constexpr void linear_insert(I first, I last, Comp& comp, Proj& proj)
			{
				iter_value_t<I> val = iter_move(last);
				if (__stl2::invoke(comp, __stl2::invoke(proj, val), __stl2::invoke(proj, *first))) {
					move_backward(first, last, last + 1);
					*first = std::move(val);
				} else {
					unguarded_linear_insert(last, std::move(val), comp, proj);
				}
			}
		};
	}

	struct __sort_fn : private __niebloid {
		template<random_access_iterator I, sentinel_for<I> S, class Comp = less,
			class Proj = identity>
		requires sortable<I, Comp, Proj>
		constexpr I
		operator()(I first, S sent, Comp comp = {}, Proj proj = {}) const {
			if (first == sent) return first;
			auto last = next(first, static_cast<S&&>(sent));
			auto n = distance(first, last);
			introsort_loop(first, last, log2(n) * 2, comp, proj);
			final_insertion_sort(first, last, comp, proj);
			return last;
		}

		template<random_access_range R, class Comp = less, class Proj = identity>
		requires sortable<iterator_t<R>, Comp, Proj>
		constexpr safe_iterator_t<R>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			return (*this)(begin(r), end(r), static_cast<Comp&&>(comp),
				static_cast<Proj&&>(proj));
		}
	private:
		static constexpr std::ptrdiff_t introsort_threshold = 16;

		template<random_access_iterator I, class Comp, class Proj>
		requires sortable<I, Comp, Proj>
		static constexpr I
		choose_pivot(I first, I last, Comp& comp, Proj& proj) {
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

		template<random_access_iterator I, class Comp, class Proj>
		requires sortable<I, Comp, Proj>
		static constexpr I
		unguarded_partition(I first, I last, Comp& comp, Proj& proj) {
			I pivot_pnt = choose_pivot(first, last, comp, proj);

			// Do the partition:
			while (true) {
				auto&& v = *pivot_pnt;
				auto&& pivot = __stl2::invoke(proj, std::forward<decltype(v)>(v));
				while (__stl2::invoke(comp, __stl2::invoke(proj, *first), pivot)) {
					++first;
				}
				while (__stl2::invoke(comp, pivot, __stl2::invoke(proj, *--last))) {
					; // do nothing
				}
				if (!(first < last)) {
					return first;
				}
				iter_swap(first, last);
				pivot_pnt = pivot_pnt == first ? last : (pivot_pnt == last ? first : pivot_pnt);
				++first;
			}
		}

		template<random_access_iterator I, class Comp, class Proj>
		requires sortable<I, Comp, Proj>
		static constexpr void
		introsort_loop(I first, I last, iter_difference_t<I> depth_limit, Comp& comp, Proj& proj)
		{
			while (distance(first, last) > introsort_threshold) {
				if (depth_limit == 0) {
					partial_sort(first, last, last, __stl2::ref(comp), __stl2::ref(proj));
					return;
				}
				I cut = unguarded_partition(first, last, comp, proj);
				introsort_loop(cut, last, --depth_limit, comp, proj);
				last = cut;
			}
		}

		template<bidirectional_iterator I, class Comp, class Proj>
		requires sortable<I, Comp, Proj>
		static constexpr void
		unguarded_insertion_sort(I first, I last, Comp& comp, Proj& proj) {
			for (I i = first; i != last; ++i) {
				detail::rsort::unguarded_linear_insert(i, iter_move(i), comp, proj);
			}
		}

		template<random_access_iterator I, class Comp, class Proj>
		requires sortable<I, Comp, Proj>
		static constexpr void
		final_insertion_sort(I first, I last, Comp &comp, Proj &proj) {
			if (distance(first, last) > introsort_threshold) {
				detail::rsort::insertion_sort(first, first + introsort_threshold, comp, proj);
				unguarded_insertion_sort(first + introsort_threshold, last, comp, proj);
			} else {
				detail::rsort::insertion_sort(first, last, comp, proj);
			}
		}

		template<integral I>
		static constexpr auto log2(I n) {
			STL2_EXPECT(n > 0);
			I k = 0;
			for (; n != 1; n /= 2) {
				++k;
			}
			return k;
		}
	};

	inline constexpr __sort_fn sort;
} STL2_CLOSE_NAMESPACE

#endif
