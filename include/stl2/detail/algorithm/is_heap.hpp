// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright LLVM Contributors
//  Copyright Eric Niebler
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 AND Apache-2.0 WITH LLVM-exception)
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

	inline constexpr __is_heap_fn is_heap{};
} STL2_CLOSE_NAMESPACE

#endif
