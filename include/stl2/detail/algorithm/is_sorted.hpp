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

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/is_sorted_until.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// is_sorted [is.sorted]
//
STL2_OPEN_NAMESPACE {
	struct __is_sorted_fn {
		template<ForwardIterator I, Sentinel<I> S, class Proj = identity,
			IndirectStrictWeakOrder<projected<I, Proj>> Comp = ranges::less<>>
		constexpr bool operator()(I first, S last, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return last == __stl2::is_sorted_until(std::move(first), last,
				std::ref(comp), std::ref(proj));
		}

		template<ForwardRange R, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<R>, Proj>> Comp = ranges::less<>>
		constexpr bool operator()(R&& r, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return __stl2::end(r) == __stl2::is_sorted_until(__stl2::begin(r), __stl2::end(r),
				std::ref(comp), std::ref(proj));
		}
	};

	inline constexpr __is_sorted_fn is_sorted {};
} STL2_CLOSE_NAMESPACE

#endif
