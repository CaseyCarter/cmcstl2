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

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

////////////////////////////////////////////////////////////////////////////////
// transform [alg.transform]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using unary_transform_result = __in_out_result<I, O>;

	template<class I1, class I2, class O>
	using binary_transform_result = __in_in_out_result<I1, I2, O>;

	struct __transform_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, weakly_incrementable O,
			copy_constructible F, class Proj = identity>
		requires writable<O, indirect_result_t<F&, projected<I, Proj>>>
		constexpr unary_transform_result<I, O>
		operator()(I first, S last, O result, F op, Proj proj = {}) const {
			for (; first != last; (void) ++first, (void) ++result) {
				*result = __stl2::invoke(op, __stl2::invoke(proj, *first));
			}
			return {std::move(first), std::move(result)};
		}

		template<input_range R, weakly_incrementable O, copy_constructible F,
			class Proj = identity>
		requires writable<O, indirect_result_t<F&, projected<iterator_t<R>, Proj>>>
		constexpr unary_transform_result<safe_iterator_t<R>, O>
		operator()(R&& r, O result, F op, Proj proj = {}) const {
			return (*this)(begin(r), end(r), std::move(result), __stl2::ref(op),
				__stl2::ref(proj));
		}

		template<input_iterator I1, sentinel_for<I1> S1,
			input_iterator I2, sentinel_for<I2> S2,
			weakly_incrementable O, copy_constructible F,
			class Proj1 = identity, class Proj2 = identity>
		requires writable<O, indirect_result_t<F&,
			projected<I1, Proj1>, projected<I2, Proj2>>>
		constexpr binary_transform_result<I1, I2, O>
		operator()(I1 first1, S1 last1, I2 first2, S2 last2, O result,
			F op, Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			for (; bool(first1 != last1) && bool(first2 != last2);
			     (void) ++first1, (void) ++first2, (void) ++result)
			{
				*result = __stl2::invoke(op, __stl2::invoke(proj1, *first1),
					__stl2::invoke(proj2, *first2));
			}
			return {std::move(first1), std::move(first2), std::move(result)};
		}

		template<input_range R1, input_range R2, weakly_incrementable O,
			copy_constructible F, class Proj1 = identity, class Proj2 = identity>
		requires writable<O, indirect_result_t<F&,
			projected<iterator_t<R1>, Proj1>, projected<iterator_t<R2>, Proj2>>>
		constexpr binary_transform_result<safe_iterator_t<R1>, safe_iterator_t<R2>, O>
		operator()(R1&& r1, R2&& r2, O result, F op, Proj1 proj1 = {},
			Proj2 proj2 = {}) const
		{
			return (*this)(begin(r1), end(r1), begin(r2), end(r2), std::move(result),
				__stl2::ref(op), __stl2::ref(proj1), __stl2::ref(proj2));
		}
	};

	inline constexpr __transform_fn transform{};
} STL2_CLOSE_NAMESPACE

#endif
