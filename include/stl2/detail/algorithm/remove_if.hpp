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
	template<ForwardIterator I, Sentinel<I> S, class Pred, class Proj = identity>
	requires
		Permutable<I> &&
		IndirectUnaryPredicate<
			Pred, projected<I, Proj>>
	I remove_if(I first, S last, Pred pred, Proj proj = {})
	{
		first = __stl2::find_if(std::move(first), last,
			__stl2::ref(pred), __stl2::ref(proj));
		if (first != last) {
			for (auto m = next(first); m != last; ++m) {
				if (!__stl2::invoke(pred, __stl2::invoke(proj, *m))) {
					*first = iter_move(m);
					++first;
				}
			}
		}
		return first;
	}

	template<ForwardRange Rng, class Pred, class Proj = identity>
	requires
		Permutable<iterator_t<Rng>> &&
		IndirectUnaryPredicate<
			Pred, projected<iterator_t<Rng>, Proj>>
	safe_iterator_t<Rng>
	remove_if(Rng&& rng, Pred pred, Proj proj = {})
	{
		return __stl2::remove_if(
			begin(rng), end(rng),
			__stl2::ref(pred), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
