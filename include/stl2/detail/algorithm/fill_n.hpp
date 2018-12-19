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
#ifndef STL2_DETAIL_ALGORITHM_FILL_N_HPP
#define STL2_DETAIL_ALGORITHM_FILL_N_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>

///////////////////////////////////////////////////////////////////////////
// fill_n [alg.fill]
//
STL2_OPEN_NAMESPACE {
	struct __fill_n_fn : private __niebloid {
		template<class T, OutputIterator<const T&> O>
		constexpr O operator()(O first, iter_difference_t<O> n, const T& value) const
		{
			for (; n > 0; --n, (void)++first) {
				*first = value;
			}
			return first;
		}
	};

	inline constexpr __fill_n_fn fill_n {};
} STL2_CLOSE_NAMESPACE

#endif
