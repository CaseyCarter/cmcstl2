// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_ADJACENT_FIND_HPP
#define STL2_DETAIL_ALGORITHM_ADJACENT_FIND_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// adjacent_find [alg.adjacent.find]
//
STL2_OPEN_NAMESPACE {
	struct __adjacent_find_fn : private __niebloid {
		template<forward_iterator I, sentinel_for<I> S, class Proj = identity,
			indirect_relation<projected<I, Proj>> Pred = equal_to>
		constexpr I
		operator()(I first, S last, Pred pred = {}, Proj proj = {}) const {
			if (first == last) {
				return first;
			}

			auto next = first;
			for (; ++next != last; first = next) {
				if (__stl2::invoke(pred,
						__stl2::invoke(proj, *first),
						__stl2::invoke(proj, *next))) {
					return first;
				}
			}
			return next;
		}

		template<forward_range R, class Proj = identity,
			indirect_relation<projected<iterator_t<R>, Proj>> Pred = equal_to>
		constexpr safe_iterator_t<R>
		operator()(R&& r, Pred pred = {}, Proj proj = {}) const {
			return (*this)(begin(r), end(r), __stl2::ref(pred),
				__stl2::ref(proj));
		}
	};

	inline constexpr __adjacent_find_fn adjacent_find;
} STL2_CLOSE_NAMESPACE

#endif
