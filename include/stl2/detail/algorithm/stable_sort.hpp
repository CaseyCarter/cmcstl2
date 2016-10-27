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
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// stable_sort [stable.sort]
//
STL2_OPEN_NAMESPACE {
	namespace detail {
		namespace ssort {
			template <class I>
			using buf_t = temporary_buffer<value_type_t<I>>;

			constexpr int merge_sort_chunk_size = 7;

			template <RandomAccessIterator I, class C, class P>
			requires
				models::Sortable<I, C, P>
			void inplace_stable_sort(I first, I last, C &pred, P &proj)
			{
				if (last - first < 15) {
					rsort::insertion_sort(first, last, pred, proj);
				} else {
					I middle = first + difference_type_t<I>(last - first) / 2;
					ssort::inplace_stable_sort(first, middle, pred, proj);
					ssort::inplace_stable_sort(middle, last, pred, proj);
					detail::inplace_merge_no_buffer(first, middle, last,
						middle - first, last - middle,
						__stl2::ref(pred), __stl2::ref(proj));
				}
			}

			template <RandomAccessIterator I, WeaklyIncrementable O, class C, class P>
			requires
				models::Sortable<I, C, P>
			void merge_sort_loop(I first, I last, O result,
				difference_type_t<I> step_size, C &pred, P &proj)
			{
				auto two_step = difference_type_t<I>(2 * step_size);
				while (last - first >= two_step) {
					result = __stl2::merge(
						__stl2::make_move_iterator(first),
						__stl2::make_move_iterator(first + step_size),
						__stl2::make_move_iterator(first + step_size),
						__stl2::make_move_iterator(first + two_step),
						result, __stl2::ref(pred),
						__stl2::ref(proj), __stl2::ref(proj)).out();
					first += two_step;
				}
				step_size = __stl2::min(difference_type_t<I>(last - first), step_size);
				__stl2::merge(
					__stl2::make_move_iterator(first),
					__stl2::make_move_iterator(first + step_size),
					__stl2::make_move_iterator(first + step_size),
					__stl2::make_move_iterator(last),
					result, __stl2::ref(pred),
					__stl2::ref(proj), __stl2::ref(proj));
			}

			template <RandomAccessIterator I, class C, class P>
			requires
				models::Sortable<I, C, P>
			void chunk_insertion_sort(I first, I last, difference_type_t<I> chunk_size,
				C &comp, P &proj)
			{
				while (last - first >= chunk_size) {
					rsort::insertion_sort(first, first + chunk_size, comp, proj);
					first += chunk_size;
				}
				rsort::insertion_sort(first, last, comp, proj);
			}

			template <RandomAccessIterator I, class C, class P>
			requires
				models::Sortable<I, C, P>
			void merge_sort_with_buffer(I first, I last, buf_t<I>& buf, C &comp, P &proj)
			{
				auto len = difference_type_t<I>(last - first);
				auto step_size = difference_type_t<I>(merge_sort_chunk_size);
				ssort::chunk_insertion_sort(first, last, step_size, comp, proj);
				if (step_size >= len) {
					return;
				}
				temporary_vector<value_type_t<I>> vec{buf};
				ssort::merge_sort_loop(first, last, __stl2::back_inserter(vec), step_size, comp, proj);
				step_size *= 2;
				while (true) {
					ssort::merge_sort_loop(vec.begin(), vec.end(), first, step_size, comp, proj);
					step_size *= 2;
					if (step_size >= len) {
						return;
					}
					ssort::merge_sort_loop(first, last, vec.begin(), step_size, comp, proj);
					step_size *= 2;
				}
			}

			template <RandomAccessIterator I, class C, class P>
			requires
				models::Sortable<I, C, P>
			void stable_sort_adaptive(I first, I last, buf_t<I>& buf, C &comp, P &proj)
			{
				auto len = difference_type_t<I>((last - first + 1) / 2);
				auto middle = first + len;
				if (len > buf.size()) {
					ssort::stable_sort_adaptive(first, middle, buf, comp, proj);
					ssort::stable_sort_adaptive(middle, last, buf, comp, proj);
				} else {
					ssort::merge_sort_with_buffer(first, middle, buf, comp, proj);
					ssort::merge_sort_with_buffer(middle, last, buf, comp, proj);
				}
				detail::merge_adaptive(first, middle, last,
					middle - first, last - middle, buf,
					__stl2::ref(comp), __stl2::ref(proj));
			}
		}
	}

	// Extension: Supports forward iterators.
	template <class I, class S, class Comp = less<>, class Proj = identity>
	requires
		models::Sentinel<__f<S>, I> &&
		models::Sortable<I, Comp, Proj>
	I stable_sort(I first, S&& last, Comp comp = Comp{}, Proj proj = Proj{})
	{
		auto n = __stl2::distance(first, __stl2::forward<S>(last));
		return detail::fsort::sort_n(__stl2::move(first), n,
			__stl2::ref(comp), __stl2::ref(proj));
	}

	template <RandomAccessIterator I, class S, class Comp = less<>,
						class Proj = identity>
	requires
		models::Sentinel<__f<S>, I> &&
		models::Sortable<I, Comp, Proj>
	I stable_sort(I first, S&& last_, Comp comp = Comp{}, Proj proj = Proj{})
	{
		auto last = __stl2::next(first, __stl2::forward<S>(last_));
		auto len = difference_type_t<I>(last - first);
		using buf_t = detail::ssort::buf_t<I>;
		auto buf = len > 256 ? buf_t{len} : buf_t{};
		if (!buf.size_) {
			detail::ssort::inplace_stable_sort(first, last, comp, proj);
		} else {
			detail::ssort::stable_sort_adaptive(first, last, buf, comp, proj);
		}
		return last;
	}

	// Extension: supports forward ranges.
	template <ForwardRange Rng, class Comp = less<>, class Proj = identity>
	requires
		models::Sortable<iterator_t<Rng>, Comp, Proj>
	safe_iterator_t<Rng>
	stable_sort(Rng&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return detail::fsort::sort_n(__stl2::begin(rng), __stl2::distance(rng),
			__stl2::ref(comp), __stl2::ref(proj));
	}

	template <RandomAccessRange Rng, class Comp = less<>, class Proj = identity>
	requires
		models::Sortable<iterator_t<Rng>, Comp, Proj>
	safe_iterator_t<Rng>
	stable_sort(Rng&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __stl2::stable_sort(__stl2::begin(rng), __stl2::end(rng),
			__stl2::ref(comp), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
