// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_MISMATCH_HPP
#define STL2_DETAIL_ALGORITHM_MISMATCH_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// mismatch [mismatch]
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I1, Sentinel<I1> S1, class I2,
		class Pred = equal_to<>, class Proj1 = identity, class Proj2 = identity>
	[[deprecated]] tagged_pair<tag::in1(I1), tag::in2(std::decay_t<I2>)>
	mismatch(I1 first1, S1 last1, I2&& first2_, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	requires
        IndirectlyComparable<I1, std::decay_t<I2>, Pred, Proj1, Proj2>
	{
		auto first2 = std::forward<I2>(first2_);
		for (; first1 != last1; ++first1, ++first2) {
			if (!__stl2::invoke(pred, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2))) {
				break;
			}
		}
		return {std::move(first1), std::move(first2)};
	}

	template <InputIterator I1, Sentinel<I1> S1,
		InputIterator I2, Sentinel<I2> S2, class Pred = equal_to<>,
		class Proj1 = identity, class Proj2 = identity>
	requires
        IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
	tagged_pair<tag::in1(I1), tag::in2(I2)>
	mismatch(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
			if (!__stl2::invoke(pred, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2))) {
				break;
			}
		}
		return {std::move(first1), std::move(first2)};
	}

	template <InputRange Rng1, class I2, class Pred = equal_to<>,
		class Proj1 = identity, class Proj2 = identity>
	[[deprecated]]
	tagged_pair<tag::in1(safe_iterator_t<Rng1>), tag::in2(__f<I2>)>
	mismatch(Rng1&& rng1, I2&& first2_, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	requires
		InputIterator<std::decay_t<I2>> &&
		!Range<I2> &&
        IndirectlyComparable<iterator_t<Rng1>, std::decay_t<I2>, Pred, Proj1, Proj2>
	{
		auto first2 = std::forward<I2>(first2_);
		return __stl2::mismatch(
			__stl2::begin(rng1), __stl2::end(rng1),
			std::move(first2), std::ref(pred),
			std::ref(proj1), std::ref(proj2));
	}

	template <InputRange Rng1, InputRange Rng2, class Pred = equal_to<>,
		class Proj1 = identity, class Proj2 = identity>
	requires
        IndirectlyComparable<iterator_t<Rng1>, iterator_t<Rng2>, Pred, Proj1, Proj2>
	tagged_pair<tag::in1(safe_iterator_t<Rng1>), tag::in2(safe_iterator_t<Rng2>)>
	mismatch(Rng1&& rng1, Rng2&& rng2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __stl2::mismatch(
			__stl2::begin(rng1), __stl2::end(rng1),
			__stl2::begin(rng2), __stl2::end(rng2),
			std::ref(pred),
			std::ref(proj1),
			std::ref(proj2));
	}
} STL2_CLOSE_NAMESPACE

#endif
