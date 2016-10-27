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
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
#ifndef STL2_DETAIL_ALGORITHM_PREV_PERMUTATION_HPP
#define STL2_DETAIL_ALGORITHM_PREV_PERMUTATION_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/reverse.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// prev_permutation [alg.permutation.generators]
//
STL2_OPEN_NAMESPACE {
	template <BidirectionalIterator I, Sentinel<I> S, class Comp = less<>,
		class Proj = identity>
	requires
		models::Sortable<I, __f<Comp>, __f<Proj>>
	bool prev_permutation(I first, S last, Comp&& comp = Comp{},
		Proj&& proj = Proj{})
	{
		if (first == last) {
			return false;
		}
		I end = __stl2::next(first, __stl2::move(last)), i = end;
		if (first == --i) {
			return false;
		}
		while (true) {
			I ip1 = i;
			if (__stl2::invoke(comp, __stl2::invoke(proj, *ip1), __stl2::invoke(proj, *--i))) {
				I j = end;
				while (!__stl2::invoke(comp, __stl2::invoke(proj, *--j), __stl2::invoke(proj, *i))) {
					;
				}
				__stl2::iter_swap(i, j);
				__stl2::reverse(ip1, end);
				return true;
			}
			if (i == first) {
				__stl2::reverse(first, end);
				return false;
			}
		}
	}

	template <BidirectionalRange Rng, class Comp = less<>, class Proj = identity>
	requires
		models::Sortable<iterator_t<Rng>, __f<Comp>, __f<Proj>>
	bool prev_permutation(Rng&& rng, Comp&& comp = Comp{}, Proj&& proj = Proj{})
	{
		return __stl2::prev_permutation(__stl2::begin(rng), __stl2::end(rng),
			__stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
