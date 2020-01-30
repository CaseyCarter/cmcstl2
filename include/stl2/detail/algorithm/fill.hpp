// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_FILL_HPP
#define STL2_DETAIL_ALGORITHM_FILL_HPP

#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// fill [alg.fill]
STL2_OPEN_NAMESPACE {
	struct __fill_fn : private __niebloid {
		template<class T, output_iterator<const T&> O, sentinel_for<O> S>
		constexpr O operator()(O first, S last, const T& value) const {
			for (; first != last; ++first) {
				*first = value;
			}
			return first;
		}

		template<class T, output_range<const T&> R>
		constexpr safe_iterator_t<R> operator()(R&& r, const T& value) const {
			return (*this)(begin(r), end(r), value);
		}
	};

	inline constexpr __fill_fn fill{};
} STL2_CLOSE_NAMESPACE

#endif
