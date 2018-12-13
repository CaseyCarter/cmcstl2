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
#ifndef STL2_DETAIL_ALGORITHM_SORT_HPP
#define STL2_DETAIL_ALGORITHM_SORT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/forward_sort.hpp>
#include <stl2/detail/algorithm/random_access_sort.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <utility>

///////////////////////////////////////////////////////////////////////////
// sort [sort]
//
STL2_OPEN_NAMESPACE {
	struct __sort_fn : private __niebloid {
	private:
		static constexpr std::ptrdiff_t introsort_threshold = 16;

		template<RandomAccessIterator I, class Comp, class Proj>
		requires Sortable<I, Comp, Proj>
		static constexpr I choose_pivot(I first, I last, Comp& comp, Proj& proj)
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
		requires Sortable<I, Comp, Proj>
		static constexpr I unguarded_partition(I first, I last, Comp& comp, Proj& proj)
		{
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

		template<RandomAccessIterator I, class Comp, class Proj>
		requires Sortable<I, Comp, Proj>
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

		template<BidirectionalIterator I, class Comp, class Proj>
		requires Sortable<I, Comp, Proj>
		static constexpr void unguarded_insertion_sort(I first, I last, Comp& comp, Proj& proj)
		{
			for (I i = first; i != last; ++i) {
				detail::rsort::unguarded_linear_insert(i, iter_move(i), comp, proj);
			}
		}

		template<RandomAccessIterator I, class Comp, class Proj>
		requires Sortable<I, Comp, Proj>
		static constexpr void final_insertion_sort(I first, I last, Comp &comp, Proj &proj)
		{
			if (distance(first, last) > introsort_threshold) {
				detail::rsort::insertion_sort(first, first + introsort_threshold, comp, proj);
				unguarded_insertion_sort(first + introsort_threshold, last, comp, proj);
			} else {
				detail::rsort::insertion_sort(first, last, comp, proj);
			}
		}

		template<Integral I>
		static constexpr auto log2(I n) {
			STL2_EXPECT(n > 0);
			I k = 0;
			for (; n != 1; n /= 2) {
				++k;
			}
			return k;
		}
	public:
		/// Extension: sort using forward iterators
		///
		template<ForwardIterator I, Sentinel<I> S, class Comp = less, class Proj = identity>
		requires Sortable<I, Comp, Proj>
		constexpr I operator()(I first, S sent, Comp comp = {}, Proj proj = {}) const
		{
			if constexpr (RandomAccessIterator<I>) {
				if (first == sent) {
					return first;
				}
				auto last = next(first, std::move(sent));
				auto n = distance(first, last);
				introsort_loop(first, last, log2(n) * 2, comp, proj);
				final_insertion_sort(first, last, comp, proj);
				return last;
			}
			else {
				auto n = distance(first, std::move(sent));
				return detail::fsort_n(std::move(first), n, std::move(comp), std::move(proj));
			}
		}

		/// Extension: sort using forward ranges
		///
		template<ForwardRange R, class Comp = less, class Proj = identity>
		requires Sortable<iterator_t<R>, Comp, Proj>
		constexpr safe_iterator_t<R> operator()(R&& r, Comp comp = {}, Proj proj = {}) const
		{
			return (*this)(begin(r), end(r), std::move(comp), std::move(proj));
		}
	};

	inline constexpr __sort_fn sort {};
} STL2_CLOSE_NAMESPACE

#endif
