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
#ifndef STL2_DETAIL_ALGORITHM_UNIQUE_HPP
#define STL2_DETAIL_ALGORITHM_UNIQUE_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/adjacent_find.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// unique [alg.unique]
//
STL2_OPEN_NAMESPACE {
	struct __unique_fn {
		template<ForwardIterator I, Sentinel<I> S, class Proj = identity,
			IndirectRelation<projected<I, Proj>> R = ranges::equal_to<>>
		requires Permutable<I>
		constexpr I operator()(I first, S last, R comp = R{}, Proj proj = Proj{}) const
		{
			first = __stl2::adjacent_find(
			std::move(first), last, std::ref(comp), std::ref(proj));
			if (first != last) {
				for (auto m = __stl2::next(first, 2); m != last; ++m) {
					if (!__stl2::invoke(comp, __stl2::invoke(proj, *first), __stl2::invoke(proj, *m))) {
						*++first = __stl2::iter_move(m);
					}
				}
				++first;
			}
			return first;
		}

		template<ForwardRange Rng, class Proj = identity,
			IndirectRelation<projected<iterator_t<Rng>, Proj>> R = ranges::equal_to<>>
		requires Permutable<iterator_t<Rng>>
		constexpr safe_iterator_t<Rng> operator()(Rng&& rng, R comp = R{}, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), std::ref(comp), std::ref(proj));
		}
	};

	inline constexpr __unique_fn unique {};
} STL2_CLOSE_NAMESPACE

#endif
