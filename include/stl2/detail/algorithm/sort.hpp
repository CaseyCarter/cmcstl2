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

///////////////////////////////////////////////////////////////////////////
// sort [sort]
//
STL2_OPEN_NAMESPACE {
	struct __sort_fn : private __niebloid {
	private:
		static constexpr std::ptrdiff_t introsort_threshold = 16;

		template<RandomAccessIterator I, class Comp, class Proj>
		requires Sortable<I, Comp, Proj>
		static constexpr I unguarded_partition(I first, I last, Comp& comp, Proj& proj)
		{
			I pivot_pnt = detail::rsort::choose_pivot(first, last, comp, proj);

			// Do the partition:
			while (true) {
				auto &&v = *pivot_pnt;
				auto &&pivot = __stl2::invoke(proj, (decltype(v) &&)v);
				while (__stl2::invoke(comp, __stl2::invoke(proj, *first), pivot)) {
					++first;
				}
				while (__stl2::invoke(comp, pivot, __stl2::invoke(proj, *--last))) {
					[[fallthrough]]; // there's no [[do_nothing]] attribute
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
		static constexpr void introsort_loop(I first, I last, iter_difference_t<I> depth_limit,
			Comp& comp, Proj& proj)
		{
			while (distance(first, last) > introsort_threshold) {
				if (depth_limit == 0) {
					__stl2::partial_sort(first, last, last, std::ref(comp), std::ref(proj));
					return;
				}
				I cut = __sort_fn::unguarded_partition(first, last, comp, proj);
				__sort_fn::introsort_loop(cut, last, --depth_limit, comp, proj);
				last = cut;
			}
		}

		template<BidirectionalIterator I, class Comp, class Proj>
		requires Sortable<I, Comp, Proj>
		static constexpr void unguarded_insertion_sort(I first, I last, Comp& comp, Proj& proj)
		{
			for (I i = first; i != last; ++i) {
				detail::rsort::unguarded_linear_insert(i, __stl2::iter_move(i), comp, proj);
			}
		}

		template<RandomAccessIterator I, class Comp, class Proj>
		requires Sortable<I, Comp, Proj>
		static constexpr void final_insertion_sort(I first, I last, Comp &comp, Proj &proj)
		{
			if (distance(first, last) > introsort_threshold) {
				detail::rsort::insertion_sort(first, first + introsort_threshold, comp, proj);
				__sort_fn::unguarded_insertion_sort(first + introsort_threshold, last, comp, proj);
			} else {
				detail::rsort::insertion_sort(first, last, comp, proj);
			}
		}

		template <Integral I>
		static constexpr auto log2(I n) {
			STL2_EXPECT(n > 0);
			I k = 0;
			for (; n != 1; n /= 2) {
				++k;
			}
			return k;
		}
	public:
		template<RandomAccessIterator I, Sentinel<I> S, class Comp = less, class Proj = identity>
		requires Sortable<I, Comp, Proj>
		constexpr I operator()(I first, S sent, Comp comp = {}, Proj proj = {}) const
		{
			if (first == sent) {
				return first;
			}
			auto last = next(first, std::move(sent));
			auto n = distance(first, last);
			__sort_fn::introsort_loop(first, last, __sort_fn::log2(n) * 2, comp, proj);
			__sort_fn::final_insertion_sort(first, last, comp, proj);
			return last;
		}

		template<RandomAccessRange R, class Comp = less, class Proj = identity>
		requires Sortable<iterator_t<R>, Comp, Proj>
		constexpr safe_iterator_t<R> operator()(R&& r, Comp comp = {}, Proj proj = {}) const
		{
			return (*this)(begin(r), end(r), std::ref(comp), std::ref(proj));
		}

		/// Extension: sort using forward iterators
		///
		template<ForwardIterator I, Sentinel<I> S, class Comp = less, class Proj = identity>
		requires Sortable<I, Comp, Proj>
		constexpr I operator()(I first, S last, Comp comp = {}, Proj proj = {}) const
		{
			auto n = distance(first, std::move(last));
			return detail::fsort::sort_n(std::move(first), n, std::ref(comp), std::ref(proj));
		}

		/// Extension: sort using forward ranges
		///
		template<ForwardRange R, class Comp = less, class Proj = identity>
		requires Sortable<iterator_t<R>, Comp, Proj>
		constexpr safe_iterator_t<R>
		operator()(R r, Comp comp = {}, Proj proj = {}) const
		{
			return detail::fsort::sort_n(begin(r), distance(r), std::ref(comp), std::ref(proj));
		}
	};

	inline constexpr __sort_fn sort {};
} STL2_CLOSE_NAMESPACE

#endif
