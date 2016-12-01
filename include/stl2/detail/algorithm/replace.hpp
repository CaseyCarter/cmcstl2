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
#ifndef STL2_DETAIL_ALGORITHM_REPLACE_HPP
#define STL2_DETAIL_ALGORITHM_REPLACE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// replace [alg.replace]
//
STL2_OPEN_NAMESPACE {
	// Extension: Relax to InputIterator
	template <InputIterator I, Sentinel<I> S, class T1, class T2,
		class Proj = identity>
	requires
		models::Writable<I, const T2&> &&
		models::IndirectCallableRelation<
			equal_to<>, projected<I, Proj>, const T1*>
	I replace(I first, S last, const T1& old_value, const T2& new_value,
		Proj proj = Proj{})
	{
		if (first != last) {
			do {
				if (__stl2::invoke(proj, *first) == old_value) {
					*first = new_value;
				}
			} while (++first != last);
		}
		return first;
	}

	// Extension: Relax to InputRange
	template <InputRange Rng, class T1, class T2, class Proj = identity>
	requires
		models::Writable<iterator_t<Rng>, const T2&> &&
		models::IndirectCallableRelation<
			equal_to<>, projected<iterator_t<Rng>, Proj>, const T1*>
	safe_iterator_t<Rng>
	replace(Rng&& rng, const T1& old_value, const T2& new_value,
		Proj proj = Proj{})
	{
		return __stl2::replace(
			__stl2::begin(rng), __stl2::end(rng),
			old_value, new_value, __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
