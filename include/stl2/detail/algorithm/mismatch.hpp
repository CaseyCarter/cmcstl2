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

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/algorithm/return_types.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// mismatch [mismatch]
//
STL2_OPEN_NAMESPACE {
	template <class I1, class I2>
	struct mismatch_result : __in1_in2_result<I1, I2> {};

	struct __mismatch_fn {
		template<InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
			class Proj1 = identity, class Proj2 = identity,
			IndirectRelation<projected<I1, Proj1>, projected<I2, Proj2>> Pred = ranges::equal_to<>>
		constexpr mismatch_result<I1, I2> operator()(I1 first1, S1 last1, I2 first2, S2 last2,
			Pred pred = Pred{}, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
				if (!__stl2::invoke(pred, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2))) {
					break;
				}
			}
			return {std::move(first1), std::move(first2)};
		}

		template<InputRange Rng1, InputRange Rng2, class Proj1 = identity, class Proj2 = identity,
			IndirectRelation<
				projected<iterator_t<Rng1>, Proj1>,
				projected<iterator_t<Rng2>, Proj2>> Pred = ranges::equal_to<>>
		constexpr mismatch_result<safe_iterator_t<Rng1>, safe_iterator_t<Rng2>>
		operator()(Rng1&& rng1, Rng2&& rng2, Pred pred = Pred{},
			Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			return (*this)(
				__stl2::begin(rng1), __stl2::end(rng1),
				__stl2::begin(rng2), __stl2::end(rng2),
				std::ref(pred), std::ref(proj1), std::ref(proj2));
		}
	};

	inline constexpr __mismatch_fn mismatch {};
} STL2_CLOSE_NAMESPACE

#endif
