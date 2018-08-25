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

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>

///////////////////////////////////////////////////////////////////////////
// fill [alg.fill]
STL2_OPEN_NAMESPACE {
	struct __fill_fn {
		template<class T, OutputIterator<const T&> O, Sentinel<O> S>
		constexpr O operator()(O first, S last, const T& value) const
		{
			for (; first != last; ++first) {
				*first = value;
			}
			return first;
		}

		template<class T, OutputRange<const T&> Rng>
		constexpr safe_iterator_t<Rng> operator()(Rng&& rng, const T& value) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), value);
		}
	};

	inline constexpr __fill_fn fill {};
} STL2_CLOSE_NAMESPACE

#endif
