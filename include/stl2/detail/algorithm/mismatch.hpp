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

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// mismatch [mismatch]
//
STL2_OPEN_NAMESPACE {
	template<class I1, class I2>
	using mismatch_result = __in_in_result<I1, I2>;

	struct __mismatch_fn : private __niebloid {
		template<input_iterator I1, sentinel_for<I1> S1, input_iterator I2,
			sentinel_for<I2> S2, class Proj1 = identity, class Proj2 = identity,
			indirect_relation<projected<I1, Proj1>,
				projected<I2, Proj2>> Pred = equal_to>
		constexpr mismatch_result<I1, I2>
		operator()(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = {},
			Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			while (true) {
				if (first1 == last1) break;
				if (first2 == last2) break;
				if (!__stl2::invoke(pred,
						__stl2::invoke(proj1, *first1),
						__stl2::invoke(proj2, *first2))) {
					break;
				}
				++first1;
				++first2;
			}
			return {std::move(first1), std::move(first2)};
		}

		template<input_range R1, input_range R2,
			class Proj1 = identity, class Proj2 = identity,
			indirect_relation<projected<iterator_t<R1>, Proj1>,
				projected<iterator_t<R2>, Proj2>> Pred = equal_to>
		constexpr mismatch_result<safe_iterator_t<R1>, safe_iterator_t<R2>>
		operator()(R1&& r1, R2&& r2, Pred pred = {},
			Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			return (*this)(begin(r1), end(r1), begin(r2), end(r2),
				__stl2::ref(pred), __stl2::ref(proj1), __stl2::ref(proj2));
		}
	};

	inline constexpr __mismatch_fn mismatch;
} STL2_CLOSE_NAMESPACE

#endif
