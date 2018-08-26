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
#ifndef STL2_DETAIL_ALGORITHM_BINARY_SEARCH_HPP
#define STL2_DETAIL_ALGORITHM_BINARY_SEARCH_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/lower_bound.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// binary_search [binary.search]
//
STL2_OPEN_NAMESPACE {
	struct __binary_search_fn {
		template<ForwardIterator I, Sentinel<I> S, class T, class Proj = identity,
			IndirectStrictWeakOrder<const T*, projected<I, Proj>> Comp = ranges::less<>>
		constexpr bool operator()(I first, S last, const T& value, Comp comp = Comp{},
			Proj proj = Proj{}) const
		{
			auto result = __stl2::lower_bound(std::move(first), last, value,
				std::ref(comp), std::ref(proj));
			return result != last && !__stl2::invoke(comp, value, __stl2::invoke(proj, *result));
		}

		template<ForwardRange R, class T, class Proj = identity,
			IndirectStrictWeakOrder<const T*, projected<iterator_t<R>, Proj>> Comp = ranges::less<>>
		constexpr bool operator()(R&& r, const T& value, Comp comp = Comp{},
			Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(r), __stl2::end(r), value, std::ref(comp), std::ref(proj));
		}
	};

	inline constexpr __binary_search_fn binary_search {};
} STL2_CLOSE_NAMESPACE

#endif
