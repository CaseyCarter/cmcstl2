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
#ifndef STL2_DETAIL_ALGORITHM_INCLUDES_HPP
#define STL2_DETAIL_ALGORITHM_INCLUDES_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// includes [includes]
//
STL2_OPEN_NAMESPACE {
	struct __includes_fn : private  __niebloid {
		template<input_iterator I1, sentinel_for<I1> S1, input_iterator I2,
			sentinel_for<I2> S2, class Proj1 = identity, class Proj2 = identity,
			indirect_strict_weak_order<projected<I1, Proj1>,
				projected<I2, Proj2>> Comp = less>
		constexpr bool operator()(I1 first1, S1 last1, I2 first2, S2 last2,
			Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			while (true) {
				if (first2 == last2) return true;
				if (first1 == last1) return false;
				if (__stl2::invoke(comp,
						__stl2::invoke(proj2, *first2),
						__stl2::invoke(proj1, *first1))) {
					return false;
				}
				if (!__stl2::invoke(comp,
						__stl2::invoke(proj1, *first1),
						__stl2::invoke(proj2, *first2))) {
					++first2;
				}
				++first1;
			}
		}

		template<input_range R1, input_range R2, class Proj1 = identity,
			class Proj2 = identity,
			indirect_strict_weak_order<projected<iterator_t<R1>, Proj1>,
				projected<iterator_t<R2>, Proj2>> Comp = less>
		constexpr bool operator()(R1&& r1, R2&& r2, Comp comp = {},
			Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			return (*this)(begin(r1), end(r1), begin(r2), end(r2),
				__stl2::ref(comp), __stl2::ref(proj1), __stl2::ref(proj2));
		}
	};

	inline constexpr __includes_fn includes{};
} STL2_CLOSE_NAMESPACE

#endif
