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
#ifndef STL2_DETAIL_ALGORITHM_COUNT_HPP
#define STL2_DETAIL_ALGORITHM_COUNT_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// count [alg.count]
//
STL2_OPEN_NAMESPACE {
	struct __count_fn : private __niebloid {
		template<InputIterator I, Sentinel<I> S, class T, class Proj = identity>
		requires IndirectRelation<equal_to, projected<I, Proj>, const T*>
		constexpr iter_difference_t<I>
		operator()(I first, S last, const T& value, Proj proj = {}) const {
			iter_difference_t<I> n = 0;
			for (; first != last; ++first) {
				if (__stl2::invoke(proj, *first) == value) {
					++n;
				}
			}
			return n;
		}

		template<InputRange R, class T, class Proj = identity>
		requires IndirectRelation<equal_to, projected<iterator_t<R>, Proj>, const T*>
		constexpr iter_difference_t<iterator_t<R>>
		operator()(R&& r, const T& value, Proj proj = {}) const {
			return (*this)(begin(r), end(r), value, __stl2::ref(proj));
		}
	};

	inline constexpr __count_fn count {};
} STL2_CLOSE_NAMESPACE

#endif
