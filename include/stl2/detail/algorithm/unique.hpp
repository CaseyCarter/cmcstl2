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
	template<ForwardIterator I, Sentinel<I> S,
		class R = equal_to, class Proj = identity>
	requires
		Permutable<I> &&
		IndirectRelation<__f<R>, projected<I, Proj>>
	I unique(I first, S last, R comp = {}, Proj proj = {})
	{
		first = __stl2::adjacent_find(
			std::move(first), last, __stl2::ref(comp), __stl2::ref(proj));
		if (first != last) {
			for (auto m = next(first, 2); m != last; ++m) {
				if (!__stl2::invoke(comp, __stl2::invoke(proj, *first), __stl2::invoke(proj, *m))) {
					*++first = __stl2::iter_move(m);
				}
			}
			++first;
		}
		return first;
	}

	template<ForwardRange Rng, class R = equal_to, class Proj = identity>
	requires
		Permutable<iterator_t<Rng>> &&
		IndirectRelation<
			__f<R>, projected<iterator_t<Rng>, Proj>>
	safe_iterator_t<Rng>
	unique(Rng&& rng, R comp = {}, Proj proj = {})
	{
		return __stl2::unique(begin(rng), end(rng),
			__stl2::ref(comp), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
