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
#include <stl2/detail/algorithm/return_types.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// transform [alg.transform]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	struct unary_transform_result : __in_out_result<I, O> {};

	template<class I1, class I2, class O>
	struct binary_transform_result : __in1_in2_out_result<I1, I2, O> {};

	struct __transform_fn {
		template<InputIterator I, Sentinel<I> S, WeaklyIncrementable O, CopyConstructible F,
			class Proj = identity>
		requires Writable<O, indirect_result_t<F&, projected<I, Proj>>>
		constexpr unary_transform_result<I, O>
		operator()(I first, S last, O result, F op, Proj proj = Proj{}) const
		{
			for (; first != last; ++first, ++result) {
				*result = __stl2::invoke(op, __stl2::invoke(proj, *first));
			}
			return {std::move(first), std::move(result)};
		}

		template<InputRange R, WeaklyIncrementable O, CopyConstructible F, class Proj = identity>
		requires Writable<O, indirect_result_t<F&, projected<iterator_t<R>, Proj>>>
		constexpr unary_transform_result<safe_iterator_t<R>, O>
		operator()(R&& r, O result, F op, Proj proj = Proj{}) const
		{
			return (*this)(
				__stl2::begin(r), __stl2::end(r), std::move(result),
				std::ref(op), std::ref(proj));
		}

		template<InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
			WeaklyIncrementable O, CopyConstructible F, class Proj1 = identity, class Proj2 = identity>
		requires Writable<O, indirect_result_t<F&, projected<I1, Proj1>, projected<I2, Proj2>>>
		constexpr binary_transform_result<I1, I2, O>
		operator()(I1 first1, S1 last1, I2 first2, S2 last2, O result, F binary_op,
			Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			for (; first1 != last1 && first2 != last2; ++first1, ++first2, ++result) {
				*result = __stl2::invoke(binary_op, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2));
			}
			return {std::move(first1), std::move(first2), std::move(result)};
		}

		template<InputRange R1, InputRange R2, WeaklyIncrementable O, CopyConstructible F,
			class Proj1 = identity, class Proj2 = identity>
		requires Writable<O, indirect_result_t<F&,
			projected<iterator_t<R1>, Proj1>, projected<iterator_t<R2>, Proj2>>>
		constexpr binary_transform_result<safe_iterator_t<R1>, safe_iterator_t<R2>, O>
		operator()(R1&& r1, R2&& r2, O result, F binary_op, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			return (*this)(
				__stl2::begin(r1), __stl2::end(r1),
				__stl2::begin(r2), __stl2::end(r2),
				std::move(result), std::ref(binary_op),
				std::ref(proj1),
				std::ref(proj2));
		}
	};

	inline constexpr __transform_fn transform {};
} STL2_CLOSE_NAMESPACE

#endif
