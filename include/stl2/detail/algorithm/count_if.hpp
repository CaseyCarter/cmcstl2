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
#ifndef STL2_DETAIL_ALGORITHM_COUNT_IF_HPP
#define STL2_DETAIL_ALGORITHM_COUNT_IF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// count_if [alg.count]
//
STL2_OPEN_NAMESPACE {
	struct __count_if_fn {
		template<InputIterator I, Sentinel<I> S, class Proj = identity,
			IndirectUnaryPredicate<projected<I, Proj>> Pred>
		constexpr iter_difference_t<I>
		operator()(I first, S last, Pred pred, Proj proj = Proj{}) const
		{
			auto n = iter_difference_t<I>{0};
			for (; first != last; ++first) {
				if (__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
					++n;
				}
			}
			return n;
		}

		template<InputRange Rng, class Proj = identity,
			IndirectUnaryPredicate<projected<iterator_t<Rng>, Proj>> Pred>
		constexpr iter_difference_t<iterator_t<Rng>>
		operator()(Rng&& rng, Pred pred, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), std::ref(pred), std::ref(proj));
		}
	};

	inline constexpr __count_if_fn count_if {};
} STL2_CLOSE_NAMESPACE

#endif
