// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_MAX_ELEMENT_HPP
#define STL2_DETAIL_ALGORITHM_MAX_ELEMENT_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// max_element [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	struct __max_element_fn : private __niebloid {
		template<forward_iterator I, sentinel_for<I> S, class Proj = identity,
			indirect_strict_weak_order<projected<I, Proj>> Comp = less>
		constexpr I
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
			if (first != last) {
				for (auto i = next(first); i != last; ++i) {
					if (!__stl2::invoke(comp,
							__stl2::invoke(proj, *i),
							__stl2::invoke(proj, *first))) {
						first = i;
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

	inline constexpr __max_element_fn max_element{};
} STL2_CLOSE_NAMESPACE

#endif
