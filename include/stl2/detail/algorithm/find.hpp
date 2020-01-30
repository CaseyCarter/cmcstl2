// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_FIND_HPP
#define STL2_DETAIL_ALGORITHM_FIND_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// find [alg.find]
//
STL2_OPEN_NAMESPACE {
	struct __find_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, class T, class Proj = identity>
		requires indirect_relation<equal_to, projected<I, Proj>, const T*>
		constexpr I
		operator()(I first, S last, const T& value, Proj proj = {}) const {
			for (; first != last; ++first) {
				if (__stl2::invoke(proj, *first) == value) {
					break;
				}
			}
			return first;
		}

		template<input_range R, class T, class Proj = identity>
		requires indirect_relation<equal_to, projected<iterator_t<R>, Proj>, const T*>
		constexpr safe_iterator_t<R>
		operator()(R&& r, const T& value, Proj proj = {}) const {
			return (*this)(begin(r), end(r), value, __stl2::ref(proj));
		}
	};

	inline constexpr __find_fn find{};
} STL2_CLOSE_NAMESPACE

#endif
