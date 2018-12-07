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
#ifndef STL2_DETAIL_ALGORITHM_BINARY_SEARCH_HPP
#define STL2_DETAIL_ALGORITHM_BINARY_SEARCH_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/lower_bound.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// binary_search [binary.search]
//
STL2_OPEN_NAMESPACE {
	template<ForwardIterator I, Sentinel<I> S, class T,
		class Comp = less, class Proj = identity>
	requires
		IndirectStrictWeakOrder<
			Comp, const T*, projected<I, Proj>>
	bool binary_search(I first, S last, const T& value, Comp comp = {},
		Proj proj = {})
	{
		auto result = __stl2::lower_bound(__stl2::move(first), last, value,
			std::ref(comp), std::ref(proj));
		return result != last && !__stl2::invoke(comp, value, __stl2::invoke(proj, *result));
	}

	template<ForwardRange Rng, class T, class Comp = less, class Proj = identity>
	requires
		IndirectStrictWeakOrder<
			Comp, const T*, projected<iterator_t<Rng>, Proj>>
	bool binary_search(Rng&& rng, const T& value, Comp comp = {}, Proj proj = {})
	{
		return __stl2::binary_search(
			__stl2::begin(rng), __stl2::end(rng), value, std::ref(comp), std::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
