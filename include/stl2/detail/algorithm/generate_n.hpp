// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_GENERATE_N_HPP
#define STL2_DETAIL_ALGORITHM_GENERATE_N_HPP

#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/concepts/function.hpp>

///////////////////////////////////////////////////////////////////////////
// generate_n [alg.generate]
//
STL2_OPEN_NAMESPACE {
	struct __generate_n_fn : private __niebloid {
		template<input_or_output_iterator O, copy_constructible F>
		requires invocable<F&> && writable<O, invoke_result_t<F&>>
		constexpr O operator()(O first, iter_difference_t<O> n, F gen) const {
			for (; n > 0; (void) ++first, --n) {
				*first = gen();
			}
			return first;
		}
	};

	inline constexpr __generate_n_fn generate_n {};
} STL2_CLOSE_NAMESPACE

#endif
