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
#ifndef STL2_DETAIL_ALGORITHM_STABLE_SORT_HPP
#define STL2_DETAIL_ALGORITHM_STABLE_SORT_HPP

#include <stl2/detail/algorithm/inplace_merge.hpp>
#include <stl2/detail/algorithm/merge.hpp>
#include <stl2/detail/algorithm/min.hpp>
#include <stl2/detail/algorithm/sort.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/iterator/insert_iterators.hpp>
#include <stl2/detail/iterator/move_iterator.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// stable_sort [stable.sort]
//
STL2_OPEN_NAMESPACE {
	struct __stable_sort_fn : private __niebloid {
		template<random_access_iterator I, class S, class Comp = less, class Proj = identity>
		requires sentinel_for<__f<S>, I> && sortable<I, Comp, Proj>
		I operator()(I first, S&& last_, Comp comp = {}, Proj proj = {}) const {
			auto last = next(first, static_cast<S&&>(last_));
			auto len = iter_difference_t<I>(last - first);
			auto buf = len > 256 ? buf_t<I>{len} : buf_t<I>{};
			if (!buf.size()) {
				inplace_stable_sort(first, last, comp, proj);
			} else {
				stable_sort_adaptive(first, last, buf, comp, proj);
			}
			return last;
		}

		template<random_access_range R, class Comp = less, class Proj = identity>
		requires sortable<iterator_t<R>, Comp, Proj>
		safe_iterator_t<R> operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			return (*this)(begin(r), end(r), static_cast<Comp&&>(comp), static_cast<Proj&&>(proj));
		}
	private:
		template<class I>
		using buf_t = detail::temporary_buffer<iter_value_t<I>>;

		static constexpr int merge_sort_chunk_size = 7;

		template<random_access_iterator I, class C, class P>
		requires sortable<I, C, P>
		static void inplace_stable_sort(I first, I last, C &pred, P &proj) {
			if (last - first < 15) {
				detail::rsort::insertion_sort(first, last, pred, proj);
			} else {
				I middle = first + iter_difference_t<I>(last - first) / 2;
				inplace_stable_sort(first, middle, pred, proj);
				inplace_stable_sort(middle, last, pred, proj);
				detail::inplace_merge_no_buffer(first, middle, last,
					middle - first, last - middle,
					__stl2::ref(pred), __stl2::ref(proj));
			}
		}

		template<random_access_iterator I, weakly_incrementable O, class C, class P>
		requires sortable<I, C, P>
		static void merge_sort_loop(I first, I last, O result,
			iter_difference_t<I> step_size, C &pred, P &proj) {
			auto two_step = iter_difference_t<I>(2 * step_size);
			while (last - first >= two_step) {
				result = merge(
					__stl2::make_move_iterator(first),
					__stl2::make_move_iterator(first + step_size),
					__stl2::make_move_iterator(first + step_size),
					__stl2::make_move_iterator(first + two_step),
					result, __stl2::ref(pred),
					__stl2::ref(proj), __stl2::ref(proj)).out;
				first += two_step;
			}
			step_size = min(iter_difference_t<I>(last - first), step_size);
			merge(
				__stl2::make_move_iterator(first),
				__stl2::make_move_iterator(first + step_size),
				__stl2::make_move_iterator(first + step_size),
				__stl2::make_move_iterator(last),
				result, __stl2::ref(pred),
				__stl2::ref(proj), __stl2::ref(proj));
		}

		template<random_access_iterator I, class C, class P>
		requires sortable<I, C, P>
		static void chunk_insertion_sort(I first, I last,
			iter_difference_t<I> chunk_size, C &comp, P &proj) {
			while (last - first >= chunk_size) {
				detail::rsort::insertion_sort(first, first + chunk_size, comp, proj);
				first += chunk_size;
			}
			detail::rsort::insertion_sort(first, last, comp, proj);
		}

		template<random_access_iterator I, class C, class P>
		requires sortable<I, C, P>
		static void merge_sort_with_buffer(I first, I last, buf_t<I>& buf, C &comp, P &proj) {
			auto len = iter_difference_t<I>(last - first);
			auto step_size = iter_difference_t<I>(merge_sort_chunk_size);
			chunk_insertion_sort(first, last, step_size, comp, proj);
			if (step_size >= len) {
				return;
			}
			detail::temporary_vector<iter_value_t<I>> vec{buf};
			merge_sort_loop(first, last, __stl2::back_inserter(vec), step_size, comp, proj);
			step_size *= 2;
			while (true) {
				merge_sort_loop(vec.begin(), vec.end(), first, step_size, comp, proj);
				step_size *= 2;
				if (step_size >= len) {
					return;
				}
				merge_sort_loop(first, last, vec.begin(), step_size, comp, proj);
				step_size *= 2;
			}
		}

		template<random_access_iterator I, class C, class P>
		requires sortable<I, C, P>
		static void stable_sort_adaptive(I first, I last, buf_t<I>& buf, C &comp, P &proj) {
			auto len = iter_difference_t<I>((last - first + 1) / 2);
			auto middle = first + len;
			if (len > buf.size()) {
				stable_sort_adaptive(first, middle, buf, comp, proj);
				stable_sort_adaptive(middle, last, buf, comp, proj);
			} else {
				merge_sort_with_buffer(first, middle, buf, comp, proj);
				merge_sort_with_buffer(middle, last, buf, comp, proj);
			}
			detail::merge_adaptive(first, middle, last,
				middle - first, last - middle, buf,
				__stl2::ref(comp), __stl2::ref(proj));
		}
	};

	inline constexpr __stable_sort_fn stable_sort{};
} STL2_CLOSE_NAMESPACE

#endif
