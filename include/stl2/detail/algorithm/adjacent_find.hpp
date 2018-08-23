// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_ADJACENT_FIND_HPP
#define STL2_DETAIL_ALGORITHM_ADJACENT_FIND_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// adjacent_find [alg.adjacent.find]
//
STL2_OPEN_NAMESPACE {
	struct __adjacent_find_fn {
		template<ForwardIterator I, Sentinel<I> S, class Proj = identity,
			IndirectRelation<projected<I, Proj>> Pred = ranges::equal_to<>>
		constexpr I operator()(I first, S last, Pred pred = Pred{}, Proj proj = Proj{}) const
		{
			if (first == last) {
				return first;
			}

			auto next = first;
			for (; ++next != last; first = next) {
				if (__stl2::invoke(pred, __stl2::invoke(proj, *first), __stl2::invoke(proj, *next))) {
					return first;
				}
			}
			return next;
		}

		template<ForwardRange Rng, class Proj = identity,
			IndirectRelation<projected<iterator_t<Rng>, Proj>> Pred = ranges::equal_to<>>
		constexpr safe_iterator_t<Rng> operator()(Rng&& rng, Pred pred = Pred{}, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), std::ref(pred), std::ref(proj));
		}
	};

	inline constexpr __adjacent_find_fn adjacent_find {};
} STL2_CLOSE_NAMESPACE

#endif
