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
#ifndef STL2_DETAIL_ALGORITHM_REMOVE_IF_HPP
#define STL2_DETAIL_ALGORITHM_REMOVE_IF_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// remove [alg.remove]
//
STL2_OPEN_NAMESPACE {
	struct __remove_if_fn {
		template<ForwardIterator I, Sentinel<I> S, class Proj = identity,
			IndirectUnaryPredicate<projected<I, Proj>> Pred>
		requires Permutable<I>
		constexpr I operator()(I first, S last, Pred pred, Proj proj = Proj{}) const
		{
			first = __stl2::find_if(std::move(first), last,
				std::ref(pred), std::ref(proj));
			if (first != last) {
				for (auto m = __stl2::next(first); m != last; ++m) {
					if (!__stl2::invoke(pred, __stl2::invoke(proj, *m))) {
						*first = __stl2::iter_move(m);
						++first;
					}
				}
			}
			return first;
		}

		template<ForwardRange Rng, class Proj = identity,
			IndirectUnaryPredicate<projected<iterator_t<Rng>, Proj>> Pred>
		requires Permutable<iterator_t<Rng>>
		constexpr safe_iterator_t<Rng> operator()(Rng&& rng, Pred pred, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), std::ref(pred), std::ref(proj));
		}
	};

	inline constexpr __remove_if_fn remove_if {};
} STL2_CLOSE_NAMESPACE

#endif
