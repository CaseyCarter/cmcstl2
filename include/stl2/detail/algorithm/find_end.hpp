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
#ifndef STL2_DETAIL_ALGORITHM_FIND_END_HPP
#define STL2_DETAIL_ALGORITHM_FIND_END_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/optional.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// find_end [alg.find.end]
//
STL2_OPEN_NAMESPACE {
	template <ForwardIterator I1, Sentinel<I1> S1,
		ForwardIterator I2, Sentinel<I2> S2,
		class Pred = equal_to<>, class Proj = identity>
	requires
		IndirectRelation<
			Pred, I2, projected<I1, Proj>>
	I1 find_end(I1 first1, const S1 last1,
		const I2 first2, const S2 last2,
		Pred pred = Pred{}, Proj proj = Proj{})
	{
		if (first2 == last2) {
			return __stl2::next(first1, last1);
		}

		optional<I1> res;
		for (; first1 != last1; ++first1) {
			if (__stl2::invoke(pred, __stl2::invoke(proj, *first1), *first2)) {
				auto m1 = first1;
				auto m2 = first2;
				do {
					if (++m2 == last2) {
						res = first1;
						break;
					}
					if (++m1 == last1) {
						return std::move(res).value_or(std::move(m1));
					}
				} while (__stl2::invoke(pred, __stl2::invoke(proj, *m1), *m2));
			}
		}
		return std::move(res).value_or(std::move(first1));
	}

	template <BidirectionalIterator I1, BidirectionalIterator I2,
		class Pred = equal_to<>, class Proj = identity>
	requires
		IndirectRelation<
			Pred, I2, projected<I1, Proj>>
	I1 find_end(I1 first1, I1 last1, I2 first2, I2 last2,
		Pred pred = Pred{}, Proj proj = Proj{})
	{
		if (first2 == last2) {
			return last1;  // Everything matches an empty sequence
		}

		--last2;
		auto l1 = last1;
		while (l1 != first1) {
			if (__stl2::invoke(pred, __stl2::invoke(proj, *--l1), *last2)) {
				auto m1 = l1;
				auto m2 = last2;
				do {
					if (m2 == first2) {
						return m1;
					}
					if (m1 == first1) {
						return last1;
					}
				} while (__stl2::invoke(pred, __stl2::invoke(proj, *--m1), *--m2));
			}
		}

		return last1;
	}

	template <RandomAccessIterator I1, RandomAccessIterator I2,
		class Pred = equal_to<>, class Proj = identity>
	requires
		IndirectRelation<
			Pred, I2, projected<I1, Proj>>
	I1 find_end(I1 first1, I1 last1, I2 first2, I2 last2,
		Pred pred = Pred{}, Proj proj = Proj{})
	{
		// Take advantage of knowing source and pattern lengths.
		// Stop short when source is smaller than pattern
		const auto len2 = last2 - first2;
		if (len2 == 0 || last1 - first1 < len2) {
			return last1;
		}

		// End of pattern match can't go before here
		const auto s = first1 + (len2 - 1);

		for (auto l1 = last1; l1 != s; --l1) {
			auto m1 = l1;
			auto m2 = last2;
			while (__stl2::invoke(pred, __stl2::invoke(proj, *--m1), *--m2)) {
				if (m2 == first2) {
					return m1;
				}
			}
		}
		return last1;
	}

	template <BidirectionalIterator I1, Sentinel<I1> S1,
		BidirectionalIterator I2, Sentinel<I2> S2,
		class Pred = equal_to<>, class Proj = identity>
	requires
		IndirectRelation<
			Pred, I2, projected<I1, Proj>>
	I1 find_end(I1 first1, S1 s1, I2 first2, S2 s2, Pred pred = Pred{}, Proj proj = Proj{})
	{
		auto last1 = __stl2::next(first1, std::move(s1));
		auto last2 = __stl2::next(first2, std::move(s2));
		return __stl2::find_end(
			std::move(first1), std::move(last1),
			std::move(first2), std::move(last2),
			std::ref(pred), std::ref(proj));
	}

	template <ForwardRange Rng1, ForwardRange Rng2,
		class Pred = equal_to<>, class Proj = identity>
	requires
		IndirectRelation<
			Pred, iterator_t<Rng2>, projected<iterator_t<Rng1>, Proj>>
	safe_iterator_t<Rng1>
	find_end(Rng1&& rng1, Rng2&& rng2, Pred pred = Pred{}, Proj proj = Proj{})
	{
		return __stl2::find_end(
			__stl2::begin(rng1), __stl2::end(rng1),
			__stl2::begin(rng2), __stl2::end(rng2),
			std::ref(pred), std::ref(proj));
	}

	// Holding off on initializer_list overloads for now; this
	// overload set is already very fragile.
} STL2_CLOSE_NAMESPACE

#endif
