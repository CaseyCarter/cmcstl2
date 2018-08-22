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
#ifndef STL2_DETAIL_ALGORITHM_FIND_IF_NOT_HPP
#define STL2_DETAIL_ALGORITHM_FIND_IF_NOT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// find_if_not [alg.find]
//
STL2_OPEN_NAMESPACE {
	struct __find_if_not_fn {
		template<InputIterator I, Sentinel<I> S, class Proj = identity,
			IndirectUnaryPredicate<projected<I, Proj>> Pred>
		constexpr I operator()(I first, S last, Pred pred, Proj proj = Proj{}) const
		{
			return __stl2::find_if(std::forward<I>(first), std::forward<S>(last),
				__stl2::not_fn(std::ref(pred)), std::ref(proj));
		}

		template<InputRange Rng, class Proj = identity,
			IndirectUnaryPredicate<projected<iterator_t<Rng>, Proj>> Pred>
		constexpr safe_iterator_t<Rng> operator()(Rng&& rng, Pred pred, Proj proj = Proj{}) const
		{
			return __stl2::find_if(__stl2::begin(rng), __stl2::end(rng),
				__stl2::not_fn(std::ref(pred)), std::ref(proj));
		}
	};

	inline constexpr __find_if_not_fn find_if_not {};
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_ALGORITHM_FIND_IF_NOT_HPP
