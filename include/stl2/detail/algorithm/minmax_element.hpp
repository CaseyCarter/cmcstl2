// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright LLVM Contributors
//  Copyright Eric Niebler
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 AND Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_MINMAX_ELEMENT_HPP
#define STL2_DETAIL_ALGORITHM_MINMAX_ELEMENT_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// minmax_element [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	struct __minmax_element_fn : private __niebloid {
		template<forward_iterator I, sentinel_for<I> S, class Proj = identity,
			indirect_strict_weak_order<projected<I, Proj>> Comp = less>
		constexpr minmax_result<I>
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const
		{
			minmax_result<I> result{first, first};
			if (first == last || ++first == last) return result;

			auto pred = [&](auto&& lhs, auto&& rhs) -> bool {
				return __stl2::invoke(comp,
					__stl2::invoke(proj, static_cast<decltype(lhs)>(lhs)),
					__stl2::invoke(proj, static_cast<decltype(rhs)>(rhs)));
			};
			if (pred(*first, *result.max)) result.min = first;
			else result.max = first;

			while (true) {
				if (++first == last) return result;

				I tmp = first;
				if (++first == last) {
					if (pred(*tmp, *result.min)) result.min = tmp;
					else if (!pred(*tmp, *result.max)) result.max = tmp;
					return result;
				}

				if (pred(*first, *tmp)) {
					if (pred(*first, *result.min)) result.min = first;
					if (!pred(*tmp, *result.max)) result.max = tmp;
				} else {
					if (pred(*tmp, *result.min)) result.min = tmp;
					if (!pred(*first, *result.max)) result.max = first;
				}
			}
		}

		template<forward_range R, class Proj = identity,
			indirect_strict_weak_order<projected<iterator_t<R>, Proj>> Comp = less>
		constexpr minmax_result<safe_iterator_t<R>>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			return (*this)(begin(r), end(r), __stl2::ref(comp), __stl2::ref(proj));
		}
	};

	inline constexpr __minmax_element_fn minmax_element{};
} STL2_CLOSE_NAMESPACE

#endif
