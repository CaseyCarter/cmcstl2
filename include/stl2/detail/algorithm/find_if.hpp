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
#ifndef STL2_DETAIL_ALGORITHM_FIND_IF_HPP
#define STL2_DETAIL_ALGORITHM_FIND_IF_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// find_if [alg.find]
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I, Sentinel<I> S, class Pred, class Proj = identity>
	requires
		models::IndirectCallablePredicate<
			__f<Pred>, projected<I, __f<Proj>>>
	I find_if(I first, S last, Pred&& pred, Proj&& proj = Proj{})
	{
		for (; first != last; ++first) {
			if (__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
				break;
			}
		}
		return first;
	}

	template <InputRange Rng, class Pred, class Proj = identity>
	requires
		models::IndirectCallablePredicate<
			__f<Pred>, projected<iterator_t<Rng>, __f<Proj>>>
	safe_iterator_t<Rng>
	find_if(Rng&& rng, Pred&& pred, Proj&& proj = Proj{})
	{
		return __stl2::find_if(__stl2::begin(rng), __stl2::end(rng),
			__stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
	}

	// Extension
	template <class E, class Pred, class Proj = identity>
	requires
		models::IndirectCallablePredicate<
			__f<Pred>, projected<const E*, __f<Proj>>>
	dangling<const E*>
	find_if(std::initializer_list<E>&& il, Pred&& pred, Proj&& proj = Proj{})
	{
		return __stl2::find_if(il.begin(), il.end(),
			__stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
