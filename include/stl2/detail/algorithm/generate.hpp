// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_GENERATE_HPP
#define STL2_DETAIL_ALGORITHM_GENERATE_HPP

#include <stl2/detail/concepts/function.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// generate [alg.generate]
//
STL2_OPEN_NAMESPACE {
	struct __generate_fn : private __niebloid {
		template<input_or_output_iterator O, sentinel_for<O> S, copy_constructible F>
		requires invocable<F&> && writable<O, invoke_result_t<F&>>
		constexpr O operator()(O first, S last, F gen) const {
			for (; first != last; ++first) {
				*first = gen();
			}
			return first;
		}

		template<class R, copy_constructible F>
		requires invocable<F&> && output_range<R, invoke_result_t<F&>>
		constexpr safe_iterator_t<R> operator()(R&& r, F gen) const {
			return (*this)(begin(r), end(r), __stl2::ref(gen));
		}
	};

	inline constexpr __generate_fn generate{};
} STL2_CLOSE_NAMESPACE

#endif
