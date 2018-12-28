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

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/forward_sort.hpp>
#include <stl2/detail/algorithm/inplace_merge.hpp>
#include <stl2/detail/algorithm/merge.hpp>
#include <stl2/detail/algorithm/min.hpp>
#include <stl2/detail/algorithm/random_access_sort.hpp>

///////////////////////////////////////////////////////////////////////////
// stable_sort [stable.sort]
//
STL2_OPEN_NAMESPACE {
	struct __stable_sort_fn : private __niebloid {
		template<class I>
		using buf_t = detail::temporary_buffer<iter_value_t<I>>;

		static constexpr int merge_sort_chunk_size = 7;

		template<RandomAccessIterator I, class C, class P>
		requires Sortable<I, C, P>
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

		template<RandomAccessIterator I, WeaklyIncrementable O, class C, class P>
		requires Sortable<I, C, P>
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

		template<RandomAccessIterator I, class C, class P>
		requires Sortable<I, C, P>
		static void chunk_insertion_sort(I first, I last,
			iter_difference_t<I> chunk_size, C &comp, P &proj) {
			while (last - first >= chunk_size) {
				detail::rsort::insertion_sort(first, first + chunk_size, comp, proj);
				first += chunk_size;
			}
			detail::rsort::insertion_sort(first, last, comp, proj);
		}

		template<RandomAccessIterator I, class C, class P>
		requires Sortable<I, C, P>
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

		template<RandomAccessIterator I, class C, class P>
		requires Sortable<I, C, P>
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

		// Extension: Supports forward iterators.
		template<class I, class S, class Comp = less, class Proj = identity>
		requires Sentinel<__f<S>, I> && Sortable<I, Comp, Proj>
		I operator()(I first, S&& last_, Comp comp = {}, Proj proj = {}) const {
			if constexpr (RandomAccessIterator<I>) {
				auto last = next(first, std::forward<S>(last_));
				auto len = iter_difference_t<I>(last - first);
				auto buf = len > 256 ? buf_t<I>{len} : buf_t<I>{};
				if (!buf.size_) {
					inplace_stable_sort(first, last, comp, proj);
				} else {
					stable_sort_adaptive(first, last, buf, comp, proj);
				}
				return last;
			} else {
				auto n = distance(first, std::forward<S>(last_));
				return detail::fsort_n(std::move(first), n,
					__stl2::ref(comp), __stl2::ref(proj));
			}
		}

		// Extension: supports forward ranges.
		template<ForwardRange Rng, class Comp = less, class Proj = identity>
		requires Sortable<iterator_t<Rng>, Comp, Proj>
		safe_iterator_t<Rng> operator()(Rng&& rng, Comp comp = {}, Proj proj = {}) const {
			if constexpr (RandomAccessRange<Rng>) {
				return (*this)(begin(rng), end(rng),
					__stl2::ref(comp), __stl2::ref(proj));
			} else {
				return detail::fsort_n(begin(rng), distance(rng),
					__stl2::ref(comp), __stl2::ref(proj));
			}
		}
	};

	inline constexpr __stable_sort_fn stable_sort {};
} STL2_CLOSE_NAMESPACE

#endif
