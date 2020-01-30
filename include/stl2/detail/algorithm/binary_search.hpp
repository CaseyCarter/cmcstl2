// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_BINARY_SEARCH_HPP
#define STL2_DETAIL_ALGORITHM_BINARY_SEARCH_HPP

#include <stl2/detail/algorithm/lower_bound.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// binary_search [binary.search]
//
STL2_OPEN_NAMESPACE {
	struct __binary_search_fn : private __niebloid {
		template<forward_iterator I, sentinel_for<I> S, class T, class Proj = identity,
			indirect_strict_weak_order<const T*, projected<I, Proj>> Comp = less>
		constexpr bool operator()(I first, S last, const T& value,
			Comp comp = {}, Proj proj = {}) const
		{
			auto result = lower_bound(std::move(first), last, value,
				__stl2::ref(comp), __stl2::ref(proj));
			return bool(result != last) &&
				!bool(__stl2::invoke(comp, value, __stl2::invoke(proj, *result)));
		}

		template<forward_range R, class T, class Proj = identity,
			indirect_strict_weak_order<const T*,
				projected<iterator_t<R>, Proj>> Comp = less>
		constexpr bool operator()(R&& r, const T& value, Comp comp = {},
			Proj proj = {}) const
		{
			return (*this)(begin(r), end(r), value, static_cast<Comp&&>(comp),
				static_cast<Proj&&>(proj));
		}
	};

	inline constexpr __binary_search_fn binary_search{};
} STL2_CLOSE_NAMESPACE

#endif
