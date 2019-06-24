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
#ifndef STL2_DETAIL_ALGORITHM_GENERATE_HPP
#define STL2_DETAIL_ALGORITHM_GENERATE_HPP

#include <stl2/detail/concepts/function.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// generate [alg.generate]
//
STL2_OPEN_NAMESPACE {
	struct __generate_fn : private __niebloid {
		template<Iterator O, Sentinel<O> S, CopyConstructible F>
		requires Invocable<F&> && Writable<O, invoke_result_t<F&>>
		constexpr O operator()(O first, S last, F gen) const {
			for (; first != last; ++first) {
				*first = gen();
			}
			return first;
		}

		template<class R, CopyConstructible F>
		requires Invocable<F&> && OutputRange<R, invoke_result_t<F&>>
		constexpr safe_iterator_t<R> operator()(R&& r, F gen) const {
			return (*this)(begin(r), end(r), __stl2::ref(gen));
		}
	};

	inline constexpr __generate_fn generate {};
} STL2_CLOSE_NAMESPACE

#endif
