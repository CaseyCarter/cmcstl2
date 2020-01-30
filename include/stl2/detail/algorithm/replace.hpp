// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_REPLACE_HPP
#define STL2_DETAIL_ALGORITHM_REPLACE_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// replace [alg.replace]
//
STL2_OPEN_NAMESPACE {
	struct __replace_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, class T1, class T2,
			class Proj = identity>
		requires writable<I, const T2&> &&
			indirect_relation<equal_to, projected<I, Proj>, const T1*>
		constexpr I operator()(I first, S last, const T1& old_value,
			const T2& new_value, Proj proj = {}) const
		{
			for (; first != last; ++first) {
				if (__stl2::invoke(proj, *first) == old_value) {
					*first = new_value;
				}
			}
			return first;
		}

		template<input_range R, class T1, class T2, class Proj = identity>
		requires writable<iterator_t<R>, const T2&> &&
			indirect_relation<equal_to, projected<iterator_t<R>, Proj>, const T1*>
		constexpr safe_iterator_t<R> operator()(R&& r, const T1& old_value,
			const T2& new_value, Proj proj = {}) const
		{
			return (*this)(begin(r), end(r), old_value, new_value,
				__stl2::ref(proj));
		}
	};

	inline constexpr __replace_fn replace{};
} STL2_CLOSE_NAMESPACE

#endif
