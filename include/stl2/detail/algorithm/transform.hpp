// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_TRANSFORM_HPP
#define STL2_DETAIL_ALGORITHM_TRANSFORM_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// transform [alg.transform]
//
STL2_OPEN_NAMESPACE {
	template<InputIterator I, Sentinel<I> S, WeaklyIncrementable O,
		CopyConstructible F, class Proj = identity>
	requires
		Writable<O,
			indirect_result_t<F&, projected<I, Proj>>>
	tagged_pair<tag::in(I), tag::out(O)>
	transform(I first, S last, O result, F op, Proj proj = {})
	{
		for (; first != last; ++first, ++result) {
			*result = __stl2::invoke(op, __stl2::invoke(proj, *first));
		}
		return {std::move(first), std::move(result)};
	}

	template<InputRange R, WeaklyIncrementable O, CopyConstructible F, class Proj = identity>
	requires
		Writable<O,
			indirect_result_t<F&, projected<iterator_t<R>, Proj>>>
	tagged_pair<tag::in(safe_iterator_t<R>), tag::out(O)>
	transform(R&& r, O result, F op, Proj proj = {})
	{
		return __stl2::transform(
			begin(r), end(r), std::move(result),
			__stl2::ref(op), __stl2::ref(proj));
	}

	template<InputIterator I1, Sentinel<I1> S1, class I2, WeaklyIncrementable O,
		CopyConstructible F, class Proj1 = identity, class Proj2 = identity>
	[[deprecated]] tagged_tuple<tag::in1(I1), tag::in2(std::decay_t<I2>), tag::out(O)>
	transform(I1 first1, S1 last1, I2&& first2_, O result,
		F op, Proj1 proj1 = {}, Proj2 proj2 = {})
	requires
		InputIterator<std::decay_t<I2>> && !Range<I2> &&
		Writable<O,
			indirect_result_t<F&,
				projected<I1, Proj1>,
				projected<std::decay_t<I2>, Proj2>>>
	{
		auto first2 = std::forward<I2>(first2_);
		for (; first1 != last1; ++first1, ++first2, ++result) {
			*result = __stl2::invoke(op, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2));
		}
		return {std::move(first1), std::move(first2), std::move(result)};
	}

	template<InputRange Rng, class I, WeaklyIncrementable O, CopyConstructible F,
		class Proj1 = identity, class Proj2 = identity>
	[[deprecated]]
	tagged_tuple<tag::in1(safe_iterator_t<Rng>),
		tag::in2(std::decay_t<I>), tag::out(O)>
	transform(Rng&& r1, I&& first2_, O result, F op,
		Proj1 proj1 = {}, Proj2 proj2 = {})
	requires
		InputIterator<std::decay_t<I>> && !Range<I> &&
		Writable<O,
			indirect_result_t<F&,
				projected<iterator_t<Rng>, Proj1>,
				projected<std::decay_t<I>, Proj2>>>
	{
		auto first2 = std::forward<I>(first2_);
		return __stl2::transform(
			begin(r1), end(r1),
			std::move(first2), std::move(result),
			__stl2::ref(op), __stl2::ref(proj1),
			__stl2::ref(proj2));
	}

	template<InputIterator I1, Sentinel<I1> S1,
		InputIterator I2, Sentinel<I2> S2,
		WeaklyIncrementable O, CopyConstructible F,
		class Proj1 = identity, class Proj2 = identity>
	requires
		Writable<O,
			indirect_result_t<F&,
				projected<I1, Proj1>,
				projected<I2, Proj2>>>
	tagged_tuple<tag::in1(I1), tag::in2(I2), tag::out(O)>
	transform(I1 first1, S1 last1, I2 first2, S2 last2, O result,
		F op, Proj1 proj1 = {}, Proj2 proj2 = {})
	{
		for (; first1 != last1 && first2 != last2; ++first1, ++first2, ++result) {
			*result = __stl2::invoke(op, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2));
		}
		return {std::move(first1), std::move(first2), std::move(result)};
	}

	template<InputRange Rng1, InputRange Rng2, WeaklyIncrementable O, CopyConstructible F,
		class Proj1 = identity, class Proj2 = identity>
	requires
		Writable<O,
			indirect_result_t<F&,
				projected<iterator_t<Rng1>, Proj1>,
				projected<iterator_t<Rng2>, Proj2>>>
	tagged_tuple<
		tag::in1(safe_iterator_t<Rng1>),
		tag::in2(safe_iterator_t<Rng2>),
		tag::out(O)>
	transform(Rng1&& r1, Rng2&& r2, O result, F op,
		Proj1 proj1 = {}, Proj2 proj2 = {})
	{
		return __stl2::transform(
			begin(r1), end(r1),
			begin(r2), end(r2),
			std::move(result), __stl2::ref(op),
			__stl2::ref(proj1),
			__stl2::ref(proj2));
	}
} STL2_CLOSE_NAMESPACE

#endif
