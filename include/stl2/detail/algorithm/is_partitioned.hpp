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
#ifndef STL2_DETAIL_ALGORITHM_IS_PARTITIONED_HPP
#define STL2_DETAIL_ALGORITHM_IS_PARTITIONED_HPP

#include <stl2/detail/algorithm/find_if_not.hpp>
#include <stl2/detail/algorithm/none_of.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// is_partitioned [alg.partitions]
//
STL2_OPEN_NAMESPACE {
	struct __is_partitioned_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, class Proj = identity,
			indirect_unary_predicate<projected<I, Proj>> Pred>
		constexpr bool
		operator()(I first, S last, Pred pred, Proj proj = {}) const {
			first = find_if_not(std::move(first), last,
				__stl2::ref(pred), __stl2::ref(proj));
			return none_of(std::move(first), std::move(last),
				__stl2::ref(pred), __stl2::ref(proj));
		}

		template<input_range Rng, class Proj = identity,
			indirect_unary_predicate<projected<iterator_t<Rng>, Proj>> Pred>
		constexpr bool
		operator()(Rng&& rng, Pred pred, Proj proj = {}) const {
			return (*this)(begin(rng), end(rng), __stl2::ref(pred),
				__stl2::ref(proj));
		}
	};

	inline constexpr __is_partitioned_fn is_partitioned;
} STL2_CLOSE_NAMESPACE

#endif
