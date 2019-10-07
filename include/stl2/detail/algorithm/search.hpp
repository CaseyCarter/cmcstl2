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
#ifndef STL2_DETAIL_ALGORITHM_SEARCH_HPP
#define STL2_DETAIL_ALGORITHM_SEARCH_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/view/subrange.hpp>

///////////////////////////////////////////////////////////////////////////
// search [alg.search]
//
STL2_OPEN_NAMESPACE {
	struct __search_fn : private __niebloid {
		template<forward_iterator I1, sentinel_for<I1> S1,
			forward_iterator I2, sentinel_for<I2> S2, class Pred = equal_to,
			class Proj1 = identity, class Proj2 = identity>
		requires indirectly_comparable< I1, I2, Pred, Proj1, Proj2>
		constexpr subrange<I1> operator()(I1 first1, S1 last1, I2 first2,
			S2 last2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			if constexpr (sized_sentinel_for<S1, I1> && sized_sentinel_for<S2, I2>) {
				return sized(first1, last1, last1 - first1,
					first2, last2, last2 - first2,
					__stl2::ref(pred), __stl2::ref(proj1), __stl2::ref(proj2));
			} else {
				return unsized(first1, last1, first2, last2,
					__stl2::ref(pred), __stl2::ref(proj1), __stl2::ref(proj2));
			}
		}

		template<forward_range R1, forward_range R2, class Pred = equal_to,
			class Proj1 = identity, class Proj2 = identity>
		requires indirectly_comparable<iterator_t<R1>, iterator_t<R2>,
			Pred, Proj1, Proj2>
		constexpr safe_subrange_t<R1> operator()(R1&& r1, R2&& r2,
			Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			if constexpr (sized_range<R1> && sized_range<R2>) {
				return sized(begin(r1), end(r1), distance(r1),
					begin(r2), end(r2), distance(r2), __stl2::ref(pred),
					__stl2::ref(proj1), __stl2::ref(proj2));
			} else {
				return unsized(begin(r1), end(r1), begin(r2), end(r2),
					__stl2::ref(pred), __stl2::ref(proj1), __stl2::ref(proj2));
			}
		}
	private:
		template<forward_iterator I1, sentinel_for<I1> S1,
			forward_iterator I2, sentinel_for<I2> S2, class Pred = equal_to,
			class Proj1 = identity, class Proj2 = identity>
		requires indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
		static constexpr subrange<I1> unsized(I1 first1, S1 last1, I2 first2,
			S2 last2, Pred pred, Proj1 proj1, Proj2 proj2)
		{
			if (first2 == last2) {
				return {first1, first1};
			}

			for (; first1 != last1; ++first1) {
				if (__stl2::invoke(pred, __stl2::invoke(proj1, *first1),
						__stl2::invoke(proj2, *first2)))
				{
					// *first1 matches *first2, now match elements after here
					auto m1 = first1;
					auto m2 = first2;
					do {
						++m1;
						// If pattern exhausted, first1 is the answer (works for 1 element pattern)
						if (++m2 == last2) {
							return {first1, m1};
						}
						// Otherwise if source exhausted, pattern not found
						if (m1 == last1) {
							return {m1, m1};
						}
					} while (__stl2::invoke(pred, __stl2::invoke(proj1, *m1), __stl2::invoke(proj2, *m2)));
				}
			}

			return {first1, first1};
		}

		template<forward_iterator I1, sentinel_for<I1> S1,
			forward_iterator I2, sentinel_for<I2> S2,
			class Pred, class Proj1, class Proj2>
		requires indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
		static constexpr subrange<I1> sized(const I1 first1_, S1 last1,
			const iter_difference_t<I1> d1_, I2 first2, S2 last2,
			const iter_difference_t<I2> d2, Pred pred, Proj1 proj1, Proj2 proj2)
		{
			if (d2 == 0) {
				return {first1_, first1_};
			}

			auto d1 = d1_;
			auto first1 = ext::uncounted(first1_);
			for(; d1 >= d2; ++first1, (void) --d1) {
				if (__stl2::invoke(pred, __stl2::invoke(proj1, *first1),
						__stl2::invoke(proj2, *first2)))
				{
					auto m1 = first1;
					auto m2 = first2;
					do {
						++m1;
						if (++m2 == last2) {
							return {
								ext::recounted(first1_, first1, d1_ - d1),
								ext::recounted(first1_, m1, d1_ - d1 + d2)
							};
						}
					} while (__stl2::invoke(pred, __stl2::invoke(proj1, *m1), __stl2::invoke(proj2, *m2)));
				}
			}
			auto end = next(ext::recounted(first1_, first1, d1_ - d1), last1);
			return {end, end};
		}
	};

	inline constexpr __search_fn search {};
} STL2_CLOSE_NAMESPACE

#endif
