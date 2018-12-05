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
	namespace __search {
		template<ForwardIterator I1, Sentinel<I1> S1,
			ForwardIterator I2, Sentinel<I2> S2, class Pred = equal_to,
			class Proj1 = identity, class Proj2 = identity>
		requires
			IndirectlyComparable<
				I1, I2, Pred, Proj1, Proj2>
		subrange<I1> unsized(I1 first1, S1 last1, I2 first2, S2 last2,
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

		template<ForwardIterator I1, Sentinel<I1> S1,
			ForwardIterator I2, Sentinel<I2> S2,
			class Pred, class Proj1, class Proj2>
		requires
			IndirectlyComparable<
				I1, I2, Pred, Proj1, Proj2>
		subrange<I1> sized(
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
	}

	template<ForwardIterator I1, Sentinel<I1> S1,
		ForwardIterator I2, Sentinel<I2> S2, class Pred = equal_to,
		class Proj1 = identity, class Proj2 = identity>
	requires
		IndirectlyComparable<
			I1, I2, Pred, Proj1, Proj2>
	subrange<I1> search(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __search::unsized(first1, last1, first2, last2,
			std::ref(pred), std::ref(proj1),
			std::ref(proj2));
	}

	// Extension
	template<ForwardIterator I1, Sentinel<I1> S1,
		ForwardIterator I2, Sentinel<I2> S2, class Pred = equal_to,
		class Proj1 = identity, class Proj2 = identity>
	requires
		SizedSentinel<S1, I1> &&
		SizedSentinel<S2, I2> &&
		IndirectlyComparable<
			I1, I2, Pred, Proj1, Proj2>
	subrange<I1> search(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __search::sized(
			first1, last1, __stl2::distance(first1, last1),
			first2, last2, __stl2::distance(first2, last2),
			std::ref(pred), std::ref(proj1),
			std::ref(proj2));
	}

	template<ForwardRange Rng1, ForwardRange Rng2, class Pred = equal_to,
		class Proj1 = identity, class Proj2 = identity>
	requires
		IndirectlyComparable<
			iterator_t<Rng1>, iterator_t<Rng2>, Pred, Proj1, Proj2>
	safe_subrange_t<Rng1> search(Rng1&& rng1, Rng2&& rng2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __search::unsized(
			__stl2::begin(rng1), __stl2::end(rng1),
			__stl2::begin(rng2), __stl2::end(rng2),
			std::ref(pred), std::ref(proj1),
			std::ref(proj2));
	}

	// Extension
	template<ForwardRange Rng1, ForwardRange Rng2, class Pred = equal_to,
						class Proj1 = identity, class Proj2 = identity>
	requires
		SizedRange<Rng1> && SizedRange<Rng2> &&
		IndirectlyComparable<
			iterator_t<Rng1>, iterator_t<Rng2>, Pred, Proj1, Proj2>
	safe_subrange_t<Rng1> search(Rng1&& rng1, Rng2&& rng2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __search::sized(
			__stl2::begin(rng1), __stl2::end(rng1), __stl2::distance(rng1),
			__stl2::begin(rng2), __stl2::end(rng2), __stl2::distance(rng2),
			std::ref(pred), std::ref(proj1),
			std::ref(proj2));
	}
} STL2_CLOSE_NAMESPACE

#endif
