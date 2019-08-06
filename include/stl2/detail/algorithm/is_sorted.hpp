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
#ifndef STL2_DETAIL_ALGORITHM_IS_SORTED_HPP
#define STL2_DETAIL_ALGORITHM_IS_SORTED_HPP

#include <stl2/detail/algorithm/is_sorted_until.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// is_sorted [is.sorted]
//
STL2_OPEN_NAMESPACE {
	struct __is_sorted_fn : private __niebloid {
		template<forward_iterator I, sentinel_for<I> S, class Proj = identity,
			indirect_strict_weak_order<projected<I, Proj>> Comp = less>
		constexpr bool
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
			return last == is_sorted_until(std::move(first), last,
				__stl2::ref(comp), __stl2::ref(proj));
		}

		template<forward_range R, class Proj = identity,
			indirect_strict_weak_order<projected<iterator_t<R>, Proj>> Comp = less>
		constexpr bool operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			return end(r) == is_sorted_until(begin(r), end(r),
				__stl2::ref(comp), __stl2::ref(proj));
		}
	};

	inline constexpr __is_sorted_fn is_sorted {};
} STL2_CLOSE_NAMESPACE

#endif
