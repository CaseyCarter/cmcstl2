// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_FILL_N_HPP
#define STL2_DETAIL_ALGORITHM_FILL_N_HPP

#include <stl2/detail/iterator/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// fill_n [alg.fill]
//
STL2_OPEN_NAMESPACE {
	struct __fill_n_fn : private __niebloid {
		template<class T, output_iterator<const T&> O>
		constexpr O
		operator()(O first, iter_difference_t<O> n, const T& value) const {
			for (; n > 0; --n, (void)++first) {
				*first = value;
			}
			return first;
		}
	};

	inline constexpr __fill_n_fn fill_n{};
} STL2_CLOSE_NAMESPACE

#endif
