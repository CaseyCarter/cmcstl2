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
#ifndef STL2_DETAIL_ALGORITHM_COUNT_IF_HPP
#define STL2_DETAIL_ALGORITHM_COUNT_IF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// count_if [alg.count]
//
STL2_OPEN_NAMESPACE {
	struct __count_if_fn : private __niebloid {
		template<InputIterator I, Sentinel<I> S, class Proj = identity,
			IndirectUnaryPredicate<projected<I, Proj>> Pred>
		constexpr iter_difference_t<I>
		operator()(I first, S last, Pred pred, Proj proj = {}) const {
			auto n = iter_difference_t<I>{0};
			for (; first != last; ++first) {
				if (__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
					++n;
				}
			}
			return n;
		}

		template<InputRange R, class Proj = identity,
			IndirectUnaryPredicate<projected<iterator_t<R>, Proj>> Pred>
		constexpr iter_difference_t<iterator_t<R>>
		operator()(R&& r, Pred pred, Proj proj = {}) const {
			return (*this)(begin(r), end(r),
				std::ref(pred), std::ref(proj));
		}
	};

	inline constexpr __count_if_fn count_if {};
} STL2_CLOSE_NAMESPACE

#endif
