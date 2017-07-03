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
#ifndef STL2_DETAIL_ALGORITHM_EQUAL_HPP
#define STL2_DETAIL_ALGORITHM_EQUAL_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// equal [alg.equal]
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I1, Sentinel<I1> S1, InputIterator I2,
		class Pred, class Proj1, class Proj2>
	requires
		IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
	bool __equal_3(I1 first1, S1 last1, I2 first2, Pred& pred,
		Proj1& proj1, Proj2& proj2)
	{
		for (; first1 != last1; ++first1, ++first2) {
			if (!__stl2::invoke(pred, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2))) {
				return false;
			}
		}
		return true;
	}

	template <InputIterator I1, Sentinel<I1> S1,
		InputIterator I2, Sentinel<I2> S2,
		class Pred, class Proj1, class Proj2>
	requires
		IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
	bool __equal_4(I1 first1, S1 last1, I2 first2, S2 last2, Pred& pred,
		Proj1& proj1, Proj2& proj2)
	{
		for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
			if (!__stl2::invoke(pred, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2))) {
				return false;
			}
		}
		return first1 == last1 && first2 == last2;
	}

	template <class I1, class S1, class I2, class Pred = equal_to<>,
		class Proj1 = identity, class Proj2 = identity>
	[[deprecated]] bool
	equal(I1&& first1, S1&& last1, I2&& first2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	requires
		InputIterator<__f<I1>> &&
		Sentinel<__f<S1>, __f<I1>> &&
		InputIterator<__f<I2>> &&
		IndirectlyComparable<
			__f<I1>, __f<I2>, Pred, Proj1, Proj2>
	{
		return __stl2::__equal_3(
			std::forward<I1>(first1), std::forward<S1>(last1),
			std::forward<I2>(first2), pred, proj1, proj2);
	}

	template <InputRange Rng1, class I2, class Pred = equal_to<>,
		class Proj1 = identity, class Proj2 = identity>
	[[deprecated]] bool equal(Rng1&& rng1, I2&& first2_, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	requires
		!is_array<remove_reference_t<I2>>::value &&
		InputIterator<__f<I2>> &&
		IndirectlyComparable<
			iterator_t<Rng1>, __f<I2>, Pred, Proj1, Proj2>
	{
		auto first2 = std::forward<I2>(first2_);
		return __stl2::__equal_3(__stl2::begin(rng1), __stl2::end(rng1),
			std::move(first2), pred, proj1, proj2);
	}

	template <class I1, class S1, class I2, class S2, class Pred = equal_to<>,
		class Proj1 = identity, class Proj2 = identity>
	requires
		InputIterator<__f<I1>> &&
		InputIterator<__f<I2>> &&
		Sentinel<__f<S1>, __f<I1>> &&
		Sentinel<__f<S2>, __f<I2>> &&
		IndirectlyComparable<
			__f<I1>, __f<I2>, Pred, Proj1, Proj2>
	bool equal(I1&& first1, S1&& last1, I2&& first2, S2&& last2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __stl2::__equal_4(
			std::forward<I1>(first1), std::forward<S1>(last1),
			std::forward<I2>(first2), std::forward<S2>(last2),
			pred, proj1, proj2);
	}

	template <class I1, class S1, class I2, class S2, class Pred = equal_to<>,
		class Proj1 = identity, class Proj2 = identity>
	requires
		InputIterator<__f<I1>> &&
		InputIterator<__f<I2>> &&
		SizedSentinel<__f<S1>, __f<I1>> &&
		SizedSentinel<__f<S2>, __f<I2>> &&
		IndirectlyComparable<
			__f<I1>, __f<I2>, Pred, Proj1, Proj2>
	bool equal(I1&& first1_, S1&& last1_, I2&& first2_, S2&& last2_, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		// This function is a bit weird: it takes first1...last2 by
		// forwarding reference, despite actually using them, so that the
		// compiler can tell this function is more constrained than the
		// non-SizedSentinel overload which takes those arguments by
		// forwarding reference for good reason.
		auto first1 = std::forward<I1>(first1_);
		auto last1 = std::forward<S1>(last1_);
		auto first2 = std::forward<I2>(first2_);
		auto last2 = std::forward<S2>(last2_);
		auto len1 = __stl2::distance(first1, last1);
		auto len2 = __stl2::distance(first2, std::move(last2));
		return len1 == len2 &&
			__stl2::__equal_3(std::move(first1), std::move(last1),
				std::move(first2), pred, proj1, proj2);
	}

	template <InputRange Rng1, InputRange Rng2, class Pred = equal_to<>,
		class Proj1 = identity, class Proj2 = identity>
	requires
		IndirectlyComparable<
			iterator_t<Rng1>, iterator_t<Rng2>,
			Pred, Proj1, Proj2>
	bool equal(Rng1&& rng1, Rng2&& rng2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __stl2::__equal_4(
			__stl2::begin(rng1), __stl2::end(rng1),
			__stl2::begin(rng2), __stl2::end(rng2),
			pred, proj1, proj2);
	}

	template <InputRange Rng1, InputRange Rng2, class Pred = equal_to<>,
		class Proj1 = identity, class Proj2 = identity>
	requires
		SizedRange<Rng1> && SizedRange<Rng2> &&
		IndirectlyComparable<
			iterator_t<Rng1>, iterator_t<Rng2>,
			Pred, Proj1, Proj2>
	bool equal(Rng1&& rng1, Rng2&& rng2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __stl2::distance(rng1) == __stl2::distance(rng2) &&
			__stl2::__equal_3(
				__stl2::begin(rng1), __stl2::end(rng1),
				__stl2::begin(rng2), pred, proj1, proj2);
	}
} STL2_CLOSE_NAMESPACE

#endif
