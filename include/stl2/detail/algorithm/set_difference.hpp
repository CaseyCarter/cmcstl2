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
#ifndef STL2_DETAIL_ALGORITHM_SET_DIFFERENCE_HPP
#define STL2_DETAIL_ALGORITHM_SET_DIFFERENCE_HPP

#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// set_difference [set.difference]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using set_difference_result = __in_out_result<I, O>;

	struct __set_difference_fn : private __niebloid {
		template<input_iterator I1, sentinel_for<I1> S1, input_iterator I2,
			sentinel_for<I2> S2, weakly_incrementable O, class Comp = less,
			class Proj1 = identity, class Proj2 = identity>
		requires mergeable<I1, I2, O, Comp, Proj1, Proj2>
		constexpr set_difference_result<I1, O>
		operator()(I1 first1, S1 last1, I2 first2, S2 last2, O result,
			Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			while (bool(first1 != last1) && bool(first2 != last2)) {
				iter_reference_t<I1>&& v1 = *first1;
				iter_reference_t<I2>&& v2 = *first2;
				auto&& p1 = __stl2::invoke(proj1, v1);
				auto&& p2 = __stl2::invoke(proj2, v2);
				if (__stl2::invoke(comp, p1, p2)) {
					*result = std::forward<iter_reference_t<I1>>(v1);
					++result;
					++first1;
				} else {
					if (!__stl2::invoke(comp, p2, p1)) {
						++first1;
					}
					++first2;
				}
			}
			return copy(std::move(first1), std::move(last1), std::move(result));
		}

		template<input_range R1, input_range R2, weakly_incrementable O,
			class Comp = less, class Proj1 = identity, class Proj2 = identity>
		requires mergeable<iterator_t<R1>, iterator_t<R2>, O, Comp, Proj1, Proj2>
		constexpr set_difference_result<safe_iterator_t<R1>, O>
		operator()(R1&& r1, R2&& r2, O result,
			Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			return (*this)(begin(r1), end(r1), begin(r2), end(r2),
				std::move(result), __stl2::ref(comp), __stl2::ref(proj1),
				__stl2::ref(proj2));
		}
	};

	inline constexpr __set_difference_fn set_difference {};
} STL2_CLOSE_NAMESPACE

#endif
