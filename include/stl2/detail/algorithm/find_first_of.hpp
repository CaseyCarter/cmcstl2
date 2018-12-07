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
#ifndef STL2_DETAIL_ALGORITHM_FIND_FIRST_OF_HPP
#define STL2_DETAIL_ALGORITHM_FIND_FIRST_OF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// find_first_of [alg.find.first.of]
//
STL2_OPEN_NAMESPACE {
	struct __find_first_of_fn : private __niebloid {
		template<InputIterator I1, Sentinel<I1> S1, ForwardIterator I2, Sentinel<I2> S2,
			class Proj1 = identity, class Proj2 = identity,
			IndirectRelation<projected<I1, Proj1>,	projected<I2, Proj2>> Pred = equal_to>
		constexpr I1 operator()(I1 first1, S1 last1, I2 first2, S2 last2,
			Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			for (; first1 != last1; ++first1) {
				for (auto pos = first2; pos != last2; ++pos) {
					if (__stl2::invoke(pred, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *pos))) {
						return first1;
					}
				}
			}
			return first1;
		}

		template<InputRange R1, ForwardRange R2, class Pred = equal_to,
			class Proj1 = identity, class Proj2 = identity>
		requires
			IndirectRelation<Pred,
				projected<iterator_t<R1>, Proj1>,
				projected<iterator_t<R2>, Proj2>>
		constexpr safe_iterator_t<R1>
		operator()(R1&& r1, R2&& r2, Pred pred = {},
			Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			return (*this)(
				__stl2::begin(r1), __stl2::end(r1),
				__stl2::begin(r2), __stl2::end(r2),
				std::ref(pred), std::ref(proj1),
				std::ref(proj2));
		}
	};

	inline constexpr __find_first_of_fn find_first_of {};
} STL2_CLOSE_NAMESPACE

#endif
