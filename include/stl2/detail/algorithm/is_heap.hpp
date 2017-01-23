// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
#ifndef STL2_DETAIL_ALGORITHM_IS_HEAP_HPP
#define STL2_DETAIL_ALGORITHM_IS_HEAP_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/is_heap_until.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// is_heap [is.heap]
//
STL2_OPEN_NAMESPACE {
	template <RandomAccessIterator I, Sentinel<I> S,
		class Comp = less<>, class Proj = identity>
	requires
		models::IndirectStrictWeakOrder<
			Comp, projected<I, Proj>>
	bool is_heap(I first, S last, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return last == __stl2::is_heap_until(__stl2::move(first), last,
			__stl2::ref(comp), __stl2::ref(proj));
	}

	template <RandomAccessRange Rng, class Comp = less<>, class Proj = identity>
	requires
		models::IndirectStrictWeakOrder<
			Comp, projected<iterator_t<Rng>, Proj>>
	bool is_heap(Rng&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __stl2::end(rng) ==
			__stl2::is_heap_until(rng, __stl2::ref(comp),
				__stl2::ref(proj));
	}

	// Extension
	template <class E, class Comp = less<>, class Proj = identity>
	requires
		models::IndirectStrictWeakOrder<
			Comp, projected<const E*, Proj>>
	bool is_heap(std::initializer_list<E>&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __stl2::end(rng) ==
			__stl2::is_heap_until(rng, __stl2::ref(comp),
				__stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
