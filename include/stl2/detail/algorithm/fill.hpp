// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_FILL_HPP
#define STL2_DETAIL_ALGORITHM_FILL_HPP

#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// fill [alg.fill]
STL2_OPEN_NAMESPACE {
	struct __fill_fn : private __niebloid {
		template<class T, OutputIterator<const T&> O, Sentinel<O> S>
		constexpr O operator()(O first, S last, const T& value) const {
			for (; first != last; ++first) {
				*first = value;
			}
			return first;
		}

		template<class T, OutputRange<const T&> R>
		constexpr safe_iterator_t<R> operator()(R&& r, const T& value) const {
			return (*this)(begin(r), end(r), value);
		}
	};

	inline constexpr __fill_fn fill {};
} STL2_CLOSE_NAMESPACE

#endif
