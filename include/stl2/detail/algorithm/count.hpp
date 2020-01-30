// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_COUNT_HPP
#define STL2_DETAIL_ALGORITHM_COUNT_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// count [alg.count]
//
STL2_OPEN_NAMESPACE {
	struct __count_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, class T, class Proj = identity>
		requires indirect_relation<equal_to, projected<I, Proj>, const T*>
		constexpr iter_difference_t<I>
		operator()(I first, S last, const T& value, Proj proj = {}) const {
			iter_difference_t<I> n = 0;
			for (; first != last; ++first) {
				if (__stl2::invoke(proj, *first) == value) {
					++n;
				}
			}
			return n;
		}

		template<input_range R, class T, class Proj = identity>
		requires indirect_relation<equal_to, projected<iterator_t<R>, Proj>, const T*>
		constexpr iter_difference_t<iterator_t<R>>
		operator()(R&& r, const T& value, Proj proj = {}) const {
			return (*this)(begin(r), end(r), value, __stl2::ref(proj));
		}
	};

	inline constexpr __count_fn count{};
} STL2_CLOSE_NAMESPACE

#endif
