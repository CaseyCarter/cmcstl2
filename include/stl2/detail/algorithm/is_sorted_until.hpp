// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_IS_SORTED_UNTIL_HPP
#define STL2_DETAIL_ALGORITHM_IS_SORTED_UNTIL_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// is_sorted_until [is.sorted]
//
STL2_OPEN_NAMESPACE {
	struct __is_sorted_until_fn : private __niebloid {
		template<forward_iterator I, sentinel_for<I> S, class Proj = identity,
			indirect_strict_weak_order<projected<I, Proj>> Comp = less>
		constexpr I
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
			if (first != last) {
				while (true) {
					auto prev = first;
					if (++first == last || __stl2::invoke(comp,
							__stl2::invoke(proj, *first),
							__stl2::invoke(proj, *prev))) {
						break;
					}
				}
			}
			return first;
		}

		template<forward_range R, class Proj = identity,
			indirect_strict_weak_order<projected<iterator_t<R>, Proj>> Comp = less>
		constexpr safe_iterator_t<R>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			return (*this)(begin(r), end(r), __stl2::ref(comp),
				__stl2::ref(proj));
		}
	};

	inline constexpr __is_sorted_until_fn is_sorted_until{};
} STL2_CLOSE_NAMESPACE

#endif
