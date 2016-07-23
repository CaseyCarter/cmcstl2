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
#ifndef STL2_DETAIL_ALGORITHM_NEXT_PERMUTATION_HPP
#define STL2_DETAIL_ALGORITHM_NEXT_PERMUTATION_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/reverse.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// next_permutation [alg.permutation.generators]
//
STL2_OPEN_NAMESPACE {
	template <BidirectionalIterator I, Sentinel<I> S,
		class Comp = less<>, class Proj = identity>
	requires
		models::Sortable<I, __f<Comp>, __f<Proj>>
	bool next_permutation(I first, S last,
		Comp&& comp_ = Comp{}, Proj&& proj_ = Proj{})
	{
		if (first == last) {
			return false;
		}
		auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
		auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
		I end = __stl2::next(first, __stl2::move(last)), i = end;
		if (first == --i) {
			return false;
		}
		while (true) {
			I ip1 = i;
			if (comp(proj(*--i), proj(*ip1))) {
				I j = end;
				while (!comp(proj(*i), proj(*--j))) {
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
	bool next_permutation(Rng&& rng, Comp&& comp = Comp{}, Proj&& proj = Proj{})
	{
		return __stl2::next_permutation(__stl2::begin(rng), __stl2::end(rng),
			__stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
