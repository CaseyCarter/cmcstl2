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

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/view/subrange.hpp>

///////////////////////////////////////////////////////////////////////////
// search [alg.search]
//
STL2_OPEN_NAMESPACE {
	class __search_fn {
		template <class I1, class S1, class I2, class S2, class Pred, class Proj1, class Proj2>
		static constexpr subrange<I1> unsized(I1 first1, S1 last1, I2 first2, S2 last2,
			Pred pred, Proj1 proj1, Proj2 proj2)
		{
			if (first2 == last2) {
				return {first1, first1};
			}

			for (; first1 != last1; ++first1) {
				if (__stl2::invoke(pred, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2))) {
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

		template <class I1, class S1, class I2, class S2, class Pred, class Proj1, class Proj2>
		static constexpr subrange<I1> sized(
			const I1 first1_, S1 last1, const iter_difference_t<I1> d1_,
			I2 first2, S2 last2, const iter_difference_t<I2> d2,
			Pred pred, Proj1 proj1, Proj2 proj2)
		{
			if (d2 == 0) {
				return {first1_, first1_};
			}

			auto d1 = d1_;
			auto first1 = ext::uncounted(first1_);
			for(; d1 >= d2; ++first1, --d1) {
				if (__stl2::invoke(pred, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2))) {
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
			auto end = __stl2::next(ext::recounted(first1_, first1, d1_ - d1), last1);
			return {end, end};
		}

		template <class I1, class S1, class I2, class S2, class Pred, class Proj1, class Proj2>
		static constexpr subrange<I1> impl(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred,
			Proj1 proj1, Proj2 proj2)
		{
			return __search_fn::unsized(first1, last1, first2, last2,
				std::ref(pred), std::ref(proj1),
				std::ref(proj2));
		}

		template <class R1, class R2, class Pred, class Proj1, class Proj2>
		static constexpr safe_subrange_t<R1> impl(R1&& r1, R2&& r2, Pred pred,
			Proj1 proj1, Proj2 proj2)
		{
			return __search_fn::unsized(
				__stl2::begin(r1), __stl2::end(r1),
				__stl2::begin(r2), __stl2::end(r2),
				std::ref(pred), std::ref(proj1), std::ref(proj2));
		}

		/////////////////
		// [Extension]
		template <class I1, SizedSentinel<I1> S1, class I2, SizedSentinel<I1> S2, class Pred,
			class Proj1, class Proj2>
		static constexpr subrange<I1> impl(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred,
			Proj1 proj1, Proj2 proj2)
		{
			return __search_fn::sized(
				first1, last1, __stl2::distance(first1, last1),
				first2, last2, __stl2::distance(first2, last2),
				std::ref(pred), std::ref(proj1),
				std::ref(proj2));
		}

		/////////////////
		// [Extension]
		template <SizedRange R1, SizedRange R2, class Pred, class Proj1, class Proj2>
		static constexpr safe_subrange_t<R1> impl(R1&& r1, R2&& r2, Pred pred,
			Proj1 proj1, Proj2 proj2)
		{
			return __search_fn::sized(
				__stl2::begin(r1), __stl2::end(r1), __stl2::distance(r1),
				__stl2::begin(r2), __stl2::end(r2), __stl2::distance(r2),
				std::ref(pred), std::ref(proj1), std::ref(proj2));
		}
	public:
		template<ForwardIterator I1, Sentinel<I1> S1, ForwardIterator I2,
			Sentinel<I2> S2, class Pred = ranges::equal_to<>,
			class Proj1 = identity, class Proj2 = identity>
		requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
		constexpr subrange<I1> operator()(I1 first1, S1 last1, I2 first2, S2 last2,
			Pred pred = Pred{}, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			return __search_fn::impl(
				std::move(first1), std::move(last1),
				std::move(first2), std::move(last2),
				std::ref(pred), std::ref(proj1), std::ref(proj2));
		}

		template<ForwardRange R1, ForwardRange R2, class Pred = ranges::equal_to<>,
			class Proj1 = identity, class Proj2 = identity>
		requires IndirectlyComparable<iterator_t<R1>, iterator_t<R2>, Pred, Proj1, Proj2>
		constexpr safe_subrange_t<R1> operator()(R1&& r1, R2&& r2, Pred pred = Pred{},
			Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			return __search_fn::impl(
				__stl2::begin(r1), __stl2::end(r1),
				__stl2::begin(r2), __stl2::end(r2),
				std::ref(pred), std::ref(proj1), std::ref(proj2));
		}
	};

	inline constexpr __search_fn search {};
} STL2_CLOSE_NAMESPACE

#endif
