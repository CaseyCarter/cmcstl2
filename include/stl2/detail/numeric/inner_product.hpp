// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2013-2017 (range-v3/include/numeric/inner_product.hpp)
//  Copyright Casey Carter 2015 -- 2017
//  Copyright Christopher Di Bella 2016 -- 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_NUMERIC_INNER_PRODUCT_HPP
#define STL2_DETAIL_NUMERIC_INNER_PRODUCT_HPP

#include <stl2/functional.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object/semiregular.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/iterator.hpp>

STL2_OPEN_NAMESPACE {
	template <InputIterator I1, Sentinel<I1> S, InputIterator I2, CopyConstructible T,
		class Op1 = plus<>, class Op2 = multiplies<>,
		class Proj1 = identity, class Proj2 = identity,
		class __X = projected<I1, Proj1>, class __Y = projected<I2, Proj2>>
	[[deprecated]] T
	inner_product(I1 first1, S last1, I2 first2, T init, Op1 op1 = Op1{}, Op2 op2 = Op2{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	requires
		Assignable<T&, const T&>() &&
		IndirectRegularInvocable<Op2, __X, __Y>() &&
		RegularInvocable<Op1, T, indirect_result_of_t<Op2&(__X, __Y)>>() &&
		Assignable<T&, result_of_t<Op1&(T, indirect_result_of_t<Op2&(__X, __Y)>)>>()
	{
		for (; first1 != last1; ++first1, (void)++first2) {
			init = __stl2::invoke(op1, __stl2::move(init), __stl2::invoke(op2,
				__stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2)));
		}
		return init;
	}

	template <InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
		CopyConstructible T,
		class Op1 = plus<>, class Op2 = multiplies<>,
		class Proj1 = identity, class Proj2 = identity,
		class __X = projected<I1, Proj1>, class __Y = projected<I2, Proj2>>
	requires
		Assignable<T&, const T&>() &&
		IndirectRegularInvocable<Op2, __X, __Y>() &&
		RegularInvocable<Op1, T, indirect_result_of_t<Op2&(__X, __Y)>>() &&
		Assignable<T&, result_of_t<Op1&(T, indirect_result_of_t<Op2&(__X, __Y)>)>>()
	T inner_product(I1 first1, S1 last1, I2 first2, S2 last2, T init, Op1 op1 = Op1{},
		Op2 op2 = Op2{}, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		for (; first1 != last1 && first2 != last2; ++first1, (void)++first2) {
			init = __stl2::invoke(op1, __stl2::move(init), __stl2::invoke(op2,
				__stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2)));
		}
		return init;
	}

	template <InputRange Rng, InputIterator I2, CopyConstructible T,
		class Op1 = plus<>, class Op2 = multiplies<>,
		class Proj1 = identity, class Proj2 = identity,
		class __X = projected<iterator_t<Rng>, Proj1>, class __Y = projected<I2, Proj2>>
	[[deprecated]] T
	inner_product(Rng&& rng, I2 first2, T init, Op1 op1 = Op1{}, Op2 op2 = Op2{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	requires
		Assignable<T&, const T&>() &&
		IndirectRegularInvocable<Op2, __X, __Y>() &&
		RegularInvocable<Op1, T, indirect_result_of_t<Op2&(__X, __Y)>>() &&
		Assignable<T&, result_of_t<Op1&(T, indirect_result_of_t<Op2&(__X, __Y)>)>>()
	{
		return inner_product(__stl2::begin(rng), __stl2::end(rng), __stl2::move(first2),
			__stl2::move(init), __stl2::ref(op1), __stl2::ref(op2),
			__stl2::ref(proj1), __stl2::ref(proj2));
	}

	template <InputRange Rng1, InputRange Rng2, CopyConstructible T,
		class Op1 = plus<>, class Op2 = multiplies<>,
		class Proj1 = identity, class Proj2 = identity,
		class __X = projected<iterator_t<Rng1>, Proj1>, class __Y = projected<iterator_t<Rng2>, Proj2>>
	requires
		Assignable<T&, const T&>() &&
		IndirectRegularInvocable<Op2, __X, __Y>() &&
		RegularInvocable<Op1, T, indirect_result_of_t<Op2&(__X, __Y)>>() &&
		Assignable<T&, result_of_t<Op1&(T, indirect_result_of_t<Op2&(__X, __Y)>)>>()
	T inner_product(Rng1&& rng1, Rng2&& rng2, T init, Op1 op1 = Op1{}, Op2 op2 = Op2{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return inner_product(__stl2::begin(rng1), __stl2::end(rng1), __stl2::begin(rng2),
			__stl2::end(rng2), __stl2::move(init), __stl2::ref(op1), __stl2::ref(op2),
			__stl2::ref(proj1), __stl2::ref(proj2));
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_INNER_PRODUCT_HPP
