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
#include <stl2/tuple.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// transform [alg.transform]
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O,
		class F, class Proj = identity>
	requires
		models::Writable<O,
			indirect_result_of_t<__f<F>&(
				projected<I, Proj>)>>
	tagged_pair<tag::in(I), tag::out(O)>
	transform(I first, S last, O result, F op, Proj proj = Proj{})
	{
		for (; first != last; ++first, ++result) {
			*result = __stl2::invoke(op, __stl2::invoke(proj, *first));
		}
		return {__stl2::move(first), __stl2::move(result)};
	}

	template <InputRange R, class O, class F, class Proj = identity>
	requires
		models::WeaklyIncrementable<__f<O>> &&
		models::Writable<__f<O>,
			indirect_result_of_t<__f<F>&(
				projected<iterator_t<R>, Proj>)>>
	tagged_pair<tag::in(safe_iterator_t<R>), tag::out(__f<O>)>
	transform(R&& r, O&& result, F op, Proj proj = Proj{})
	{
		return __stl2::transform(
			__stl2::begin(r), __stl2::end(r), __stl2::forward<O>(result),
			__stl2::ref(op), __stl2::ref(proj));
	}

	// Extension
	template <class E, class O, class F, class Proj = identity>
	requires
		models::WeaklyIncrementable<__f<O>> &&
		models::Writable<__f<O>,
			indirect_result_of_t<__f<F>&(
				projected<const E*, Proj>)>>
	tagged_pair<tag::in(dangling<const E*>), tag::out(__f<O>)>
	transform(std::initializer_list<E>&& r, O&& result,
						F op, Proj proj = Proj{})
	{
		return __stl2::transform(
			__stl2::begin(r), __stl2::end(r), __stl2::forward<O>(result),
			__stl2::ref(op), __stl2::ref(proj));
	}

	template <InputIterator I1, Sentinel<I1> S1,
		InputIterator I2, WeaklyIncrementable O,
		class F, class Proj1 = identity, class Proj2 = identity>
	[[deprecated]] tagged_tuple<tag::in1(I1), tag::in2(I2), tag::out(O)>
	transform(I1 first1, S1 last1, I2 first2, O result,
		F op, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	requires
		models::Writable<O,
			indirect_result_of_t<__f<F>&(
				projected<I1, Proj1>,
				projected<I2, Proj2>)>>
	{
		for (; first1 != last1; ++first1, ++first2, ++result) {
			*result = __stl2::invoke(op, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2));
		}
		return {__stl2::move(first1), __stl2::move(first2), __stl2::move(result)};
	}

	template <InputRange Rng, class I, class O, class F,
		class Proj1 = identity, class Proj2 = identity>
	[[deprecated]]
	tagged_tuple<tag::in1(safe_iterator_t<Rng>),
		tag::in2(__f<I>), tag::out(__f<O>)>
	transform(Rng&& r1, I&& first2, O&& result, F op,
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	requires
		!is_array<remove_reference_t<I>>::value &&
		models::InputIterator<__f<I>> &&
		models::Writable<__f<O>,
			indirect_result_of_t<__f<F>&(
				projected<iterator_t<Rng>, Proj1>,
				projected<__f<I>, Proj2>)>>
	{
		return __stl2::transform(
			__stl2::begin(r1), __stl2::end(r1),
			__stl2::forward<I>(first2), __stl2::forward<O>(result),
			__stl2::ref(op), __stl2::ref(proj1),
			__stl2::ref(proj2));
	}

	template <InputIterator I1, Sentinel<I1> S1,
		InputIterator I2, Sentinel<I2> S2,
		WeaklyIncrementable O, class F,
		class Proj1 = identity, class Proj2 = identity>
	requires
		models::Writable<O,
			indirect_result_of_t<__f<F>&(
				projected<I1, Proj1>,
				projected<I2, Proj2>)>>
	tagged_tuple<tag::in1(I1), tag::in2(I2), tag::out(O)>
	transform(I1 first1, S1 last1, I2 first2, S2 last2, O result,
		F op, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		for (; first1 != last1 && first2 != last2; ++first1, ++first2, ++result) {
			*result = __stl2::invoke(op, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2));
		}
		return {__stl2::move(first1), __stl2::move(first2), __stl2::move(result)};
	}

	template <InputRange Rng1, InputRange Rng2, class O, class F,
		class Proj1 = identity, class Proj2 = identity>
	requires
		models::WeaklyIncrementable<__f<O>> &&
		models::Writable<__f<O>,
			indirect_result_of_t<__f<F>&(
				projected<iterator_t<Rng1>, Proj1>,
				projected<iterator_t<Rng2>, Proj2>)>>
	tagged_tuple<
		tag::in1(safe_iterator_t<Rng1>),
		tag::in2(safe_iterator_t<Rng2>),
		tag::out(__f<O>)>
	transform(Rng1&& r1, Rng2&& r2, O&& result, F op,
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __stl2::transform(
			__stl2::begin(r1), __stl2::end(r1),
			__stl2::begin(r2), __stl2::end(r2),
			__stl2::forward<O>(result), __stl2::ref(op),
			__stl2::ref(proj1),
			__stl2::ref(proj2));
	}

	// Extension
	template <class E, InputRange Rng2, class O, class F,
		class Proj1 = identity, class Proj2 = identity>
	requires
		models::WeaklyIncrementable<__f<O>> &&
		models::Writable<__f<O>,
			indirect_result_of_t<__f<F>&(
				projected<const E*, Proj1>,
				projected<iterator_t<Rng2>, Proj2>)>>
	tagged_tuple<
		tag::in1(dangling<const E*>),
		tag::in2(safe_iterator_t<Rng2>),
		tag::out(__f<O>)>
	transform(std::initializer_list<E>&& r1, Rng2&& r2, O&& result, F op,
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __stl2::transform(
			__stl2::begin(r1), __stl2::end(r1),
			__stl2::begin(r2), __stl2::end(r2),
			__stl2::forward<O>(result), __stl2::ref(op),
			__stl2::ref(proj1),
			__stl2::ref(proj2));
	}

	// Extension
	template <InputRange Rng1, class E, class O, class F,
		class Proj1 = identity, class Proj2 = identity>
	requires
		models::WeaklyIncrementable<__f<O>> &&
		models::Writable<__f<O>,
			indirect_result_of_t<__f<F>&(
				projected<iterator_t<Rng1>, Proj1>,
				projected<const E*, Proj2>)>>
	tagged_tuple<
		tag::in1(safe_iterator_t<Rng1>),
		tag::in2(dangling<const E*>),
		tag::out(__f<O>)>
	transform(Rng1&& r1, std::initializer_list<E>&& r2, O&& result, F op,
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __stl2::transform(
			__stl2::begin(r1), __stl2::end(r1),
			__stl2::begin(r2), __stl2::end(r2),
			__stl2::forward<O>(result), __stl2::ref(op),
			__stl2::ref(proj1),
			__stl2::ref(proj2));
	}

	// Extension
	template <class E1, class E2, class O, class F,
		class Proj1 = identity, class Proj2 = identity>
	requires
		models::WeaklyIncrementable<__f<O>> &&
		models::Writable<__f<O>,
			indirect_result_of_t<__f<F>&(
				projected<const E1*, Proj1>,
				projected<const E2*, Proj2>)>>
	tagged_tuple<
		tag::in1(dangling<const E1*>),
		tag::in2(dangling<const E2*>),
		tag::out(__f<O>)>
	transform(std::initializer_list<E1>&& r1,
		std::initializer_list<E2>&& r2, O&& result, F op,
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __stl2::transform(
			__stl2::begin(r1), __stl2::end(r1),
			__stl2::begin(r2), __stl2::end(r2),
			__stl2::forward<O>(result), __stl2::ref(op),
			__stl2::ref(proj1),
			__stl2::ref(proj2));
	}
} STL2_CLOSE_NAMESPACE

#endif
