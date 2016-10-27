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
#ifndef STL2_DETAIL_ALGORITHM_REPLACE_IF_HPP
#define STL2_DETAIL_ALGORITHM_REPLACE_IF_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// replace_if [alg.replace]
//
STL2_OPEN_NAMESPACE {
	// Extension: Relax to InputIterator
	template<InputIterator I, Sentinel<I> S, class Pred, class T, class Proj = identity>
	requires
		models::Writable<I, const T&> &&
		models::IndirectCallablePredicate<
			Pred, projected<I, Proj>>
	I replace_if(I first, S last, Pred pred, const T& new_value, Proj proj = Proj{})
	{
		if (first != last) {
			do {
				if (__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
					*first = new_value;
				}
			} while (++first != last);
		}
		return first;
	}

	// Extension: Relax to InputRange
	template<InputRange Rng, class Pred, class T, class Proj = identity>
	requires
		models::Writable<iterator_t<Rng>, const T&> &&
		models::IndirectCallablePredicate<
			Pred, projected<iterator_t<Rng>, Proj>>
	safe_iterator_t<Rng> replace_if(Rng&& rng, Pred pred, const T& new_value,
		Proj proj = Proj{})
	{
		return __stl2::replace_if(__stl2::begin(rng), __stl2::end(rng),
			__stl2::ref(pred), new_value, __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
