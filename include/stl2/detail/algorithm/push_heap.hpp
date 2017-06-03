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
#ifndef STL2_DETAIL_ALGORITHM_PUSH_HEAP_HPP
#define STL2_DETAIL_ALGORITHM_PUSH_HEAP_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/heap_sift.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// push_heap [push.heap]
//
STL2_OPEN_NAMESPACE {
	template <RandomAccessIterator I, class S, class Comp = less<>, class Proj = identity>
	requires
		Sentinel<__f<S>, I> &&
		Sortable<I, Comp, Proj>
	I push_heap(I first, S&& last, Comp comp = Comp{}, Proj proj = Proj{})
	{
		auto n = __stl2::distance(first, __stl2::forward<S>(last));
		detail::sift_up_n(first, n, __stl2::ref(comp), __stl2::ref(proj));
		return first + n;
	}

	template <RandomAccessRange Rng, class Comp = less<>, class Proj = identity>
	requires
		Sortable<iterator_t<Rng>, Comp, Proj>
	safe_iterator_t<Rng>
	push_heap(Rng&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		auto n = __stl2::distance(rng);
		detail::sift_up_n(__stl2::begin(rng), n, __stl2::ref(comp),
			__stl2::ref(proj));
		return __stl2::begin(rng) + n;
	}
} STL2_CLOSE_NAMESPACE

#endif
