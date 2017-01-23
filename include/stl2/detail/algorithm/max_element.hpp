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
#ifndef STL2_DETAIL_ALGORITHM_MAX_ELEMENT_HPP
#define STL2_DETAIL_ALGORITHM_MAX_ELEMENT_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// max_element [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	template <ForwardIterator I, Sentinel<I> S, class Comp = less<>, class Proj = identity>
	requires
		models::IndirectStrictWeakOrder<
			Comp, projected<I, Proj>>
	I max_element(I first, S last, Comp comp = Comp{}, Proj proj = Proj{})
	{
		if (first != last) {
			for (auto i = __stl2::next(first); i != last; ++i) {
				if (!__stl2::invoke(comp, __stl2::invoke(proj, *i), __stl2::invoke(proj, *first))) {
					first = i;
				}
			}
		}
		return first;
	}

	template <ForwardRange Rng, class Comp = less<>, class Proj = identity>
	requires
		models::IndirectStrictWeakOrder<
			Comp, projected<iterator_t<Rng>, Proj>>
	safe_iterator_t<Rng>
	max_element(Rng&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __stl2::max_element(__stl2::begin(rng), __stl2::end(rng),
			__stl2::ref(comp), __stl2::ref(proj));
	}

	// Extension
	template <class E, class Comp = less<>, class Proj = identity>
	requires
		models::IndirectStrictWeakOrder<
			Comp, projected<const E*, Proj>>
	dangling<const E*>
	max_element(std::initializer_list<E>&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __stl2::max_element(__stl2::begin(rng), __stl2::end(rng),
			__stl2::ref(comp), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
