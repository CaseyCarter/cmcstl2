// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_FOR_EACH_HPP
#define STL2_DETAIL_ALGORITHM_FOR_EACH_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// for_each [alg.foreach]
//
STL2_OPEN_NAMESPACE {
	template<class I, class F>
	using for_each_result = __in_fun_result<I, F>;

	struct __for_each_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, class Proj = identity,
			indirect_unary_invocable<projected<I, Proj>> F>
		constexpr for_each_result<I, F>
		operator()(I first, S last, F fun, Proj proj = {}) const {
			for (; first != last; ++first) {
				__stl2::invoke(fun, __stl2::invoke(proj, *first));
			}
			return {std::move(first), std::move(fun)};
		}

		template<input_range R, class Proj = identity,
			indirect_unary_invocable<projected<iterator_t<R>, Proj>> F>
		constexpr for_each_result<safe_iterator_t<R>, F>
		operator()(R&& r, F fun, Proj proj = {}) const {
			return (*this)(begin(r), end(r), std::move(fun), std::move(proj));
		}
	};

	inline constexpr __for_each_fn for_each{};
} STL2_CLOSE_NAMESPACE

#endif
