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
#ifndef STL2_DETAIL_ALGORITHM_IS_SORTED_HPP
#define STL2_DETAIL_ALGORITHM_IS_SORTED_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/is_sorted_until.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// is_sorted [is.sorted]
//
STL2_OPEN_NAMESPACE {
	template<ForwardIterator I, Sentinel<I> S, class Comp = less,
		class Proj = identity>
	requires
		IndirectStrictWeakOrder<
			Comp, projected<I, Proj>>
	bool is_sorted(I first, S last, Comp comp = {}, Proj proj = {})
	{
		return last == __stl2::is_sorted_until(std::move(first), last,
			__stl2::ref(comp), __stl2::ref(proj));
	}

	template<ForwardRange Rng, class Comp = less, class Proj = identity>
	requires
		IndirectStrictWeakOrder<
			Comp, projected<iterator_t<Rng>, Proj>>
	bool is_sorted(Rng&& rng, Comp comp = {}, Proj proj = {})
	{
		return end(rng) ==
			__stl2::is_sorted_until(begin(rng), end(rng),
				__stl2::ref(comp), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
