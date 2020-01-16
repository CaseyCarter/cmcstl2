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
#ifndef STL2_DETAIL_ALGORITHM_FIND_IF_NOT_HPP
#define STL2_DETAIL_ALGORITHM_FIND_IF_NOT_HPP

#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// find_if_not [alg.find]
//
STL2_OPEN_NAMESPACE {
	struct __find_if_not_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, class Proj = identity,
			indirect_unary_predicate<projected<I, Proj>> Pred>
		constexpr I
		operator()(I first, S last, Pred pred, Proj proj = {}) const {
			return find_if(std::move(first), std::move(last),
				__stl2::not_fn(__stl2::ref(pred)), __stl2::ref(proj));
		}

		template<input_range R, class Proj = identity,
			indirect_unary_predicate<projected<iterator_t<R>, Proj>> Pred>
		constexpr safe_iterator_t<R>
		operator()(R&& r, Pred pred, Proj proj = {}) const {
			return find_if(begin(r), end(r),
				__stl2::not_fn(__stl2::ref(pred)), __stl2::ref(proj));
		}
	};

	inline constexpr __find_if_not_fn find_if_not{};
} STL2_CLOSE_NAMESPACE

#endif
