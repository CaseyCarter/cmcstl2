// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_REPLACE_IF_HPP
#define STL2_DETAIL_ALGORITHM_REPLACE_IF_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// replace_if [alg.replace]
//
STL2_OPEN_NAMESPACE {
	struct __replace_if_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, class T, class Proj = identity,
			indirect_unary_predicate<projected<I, Proj>> Pred>
		requires writable<I, const T&>
		constexpr I operator()(I first, S last, Pred pred, const T& new_value,
			Proj proj = {}) const
		{
			for (; first != last; ++first) {
				if (__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
					*first = new_value;
				}
			}
			return first;
		}

		template<input_range R, class T, class Proj = identity,
			indirect_unary_predicate<projected<iterator_t<R>, Proj>> Pred>
		requires writable<iterator_t<R>, const T&>
		constexpr safe_iterator_t<R>
		operator()(R&& r, Pred pred, const T& new_value, Proj proj = {}) const {
			return (*this)(begin(r), end(r), __stl2::ref(pred), new_value,
				__stl2::ref(proj));
		}
	};

	inline constexpr __replace_if_fn replace_if{};
} STL2_CLOSE_NAMESPACE

#endif
