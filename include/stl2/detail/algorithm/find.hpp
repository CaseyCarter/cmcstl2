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
#ifndef STL2_DETAIL_ALGORITHM_FIND_HPP
#define STL2_DETAIL_ALGORITHM_FIND_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// find [alg.find]
//
STL2_OPEN_NAMESPACE {
	struct __find_fn {
		template<InputIterator I, Sentinel<I> S, class T, class Proj = identity>
		requires IndirectRelation<ranges::equal_to<>, projected<I, Proj>, const T*>
		constexpr I operator()(I first, S last, const T& value, Proj proj = Proj{}) const
		{
			return __stl2::find_if(std::move(first), std::move(last),
				[&value](auto const& x) { return x == value; }, std::ref(proj));
		}

		template<InputRange Rng, class T, class Proj = identity>
		requires IndirectRelation<ranges::equal_to<>, projected<iterator_t<Rng>, Proj>, const T*>
		constexpr safe_iterator_t<Rng> operator()(Rng&& rng, const T& value, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), value, std::ref(proj));
		}
	};

	inline constexpr __find_fn find {};
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_ALGORITHM_FIND_HPP
