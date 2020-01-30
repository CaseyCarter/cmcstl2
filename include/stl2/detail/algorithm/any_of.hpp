// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Andrew Sutton
//  Copyright Gonzalo Brito Gadeschi
//  Copyright Casey Carter
//
// SPDX-License-Identifier: BSL-1.0
//
#ifndef STL2_DETAIL_ALGORITHM_ANY_OF_HPP
#define STL2_DETAIL_ALGORITHM_ANY_OF_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/concepts.hpp>

////////////////////////////////////////////////////////////////////////////////
// any_of [alg.any_of]
//
STL2_OPEN_NAMESPACE {
	struct __any_of_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, class Proj = identity,
			indirect_unary_predicate<projected<I, Proj>> Pred>
		constexpr bool
		operator()(I first, S last, Pred pred, Proj proj = {}) const {
			for (; first != last; ++first) {
				if (__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
					return true;
				}
			}
			return false;
		}

		template<input_range R, class Proj = identity,
			indirect_unary_predicate<projected<iterator_t<R>, Proj>> Pred>
		constexpr bool operator()(R&& r, Pred pred, Proj proj = {}) const {
			return (*this)(begin(r), end(r), __stl2::ref(pred), __stl2::ref(proj));
		}
	};

	inline constexpr __any_of_fn any_of{};
} STL2_CLOSE_NAMESPACE

#endif
