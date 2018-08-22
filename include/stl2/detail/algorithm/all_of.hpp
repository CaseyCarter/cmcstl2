// cmcstl2 - A concept-enabled C++ standard library
//
// Algorithm adapted (stolen) from Range v3 library
//
//  Copyright Andrew Sutton 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_ALL_OF_HPP
#define STL2_DETAIL_ALGORITHM_ALL_OF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/find_if_not.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// all_of [alg.all_of]
//
STL2_OPEN_NAMESPACE {
	struct __all_of_fn {
		template<InputIterator I, Sentinel<I> S, class Proj = identity,
			IndirectUnaryPredicate<projected<I, Proj>> Pred>
		constexpr bool operator()(I first, S last, Pred pred, Proj proj = Proj{}) const
		{
			return __stl2::find_if_not(std::move(first), std::move(last), std::ref(pred),
				std::ref(proj)) == last;
		}

		template<InputRange Rng, class Proj = identity,
			IndirectUnaryPredicate<projected<iterator_t<Rng>, Proj>> Pred>
		constexpr bool operator()(Rng&& rng, Pred pred, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), std::ref(pred), std::ref(proj));
		}
	};

	inline constexpr __all_of_fn all_of {};
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_ALGORITHM_ALL_OF_HPP
