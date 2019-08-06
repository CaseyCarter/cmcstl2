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

#include <stl2/detail/algorithm/is_heap_until.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// is_heap [is.heap]
//
STL2_OPEN_NAMESPACE {
	struct __is_heap_fn : private __niebloid {
		template<random_access_iterator I, sentinel_for<I> S, class Proj = identity,
			indirect_strict_weak_order<projected<I, Proj>> Comp = less>
		constexpr bool
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
			return last == is_heap_until(std::move(first), last,
				__stl2::ref(comp), __stl2::ref(proj));
		}

		template<random_access_range Rng, class Proj = identity,
			indirect_strict_weak_order<projected<iterator_t<Rng>, Proj>> Comp = less>
		constexpr bool
		operator()(Rng&& rng, Comp comp = {}, Proj proj = {}) const {
			return end(rng) ==
				is_heap_until(rng, __stl2::ref(comp), __stl2::ref(proj));
		}
	};

	inline constexpr __is_heap_fn is_heap {};
} STL2_CLOSE_NAMESPACE

#endif
