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
#ifndef STL2_DETAIL_ALGORITHM_NONE_OF_HPP
#define STL2_DETAIL_ALGORITHM_NONE_OF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// none_of [alg.none_of]
//
STL2_OPEN_NAMESPACE {
	struct __none_of_fn {
		template<InputIterator I, Sentinel<I> S, class Proj = identity,
			IndirectUnaryPredicate<projected<I, Proj>> Pred>
		constexpr bool operator()(I first, S last, Pred pred, Proj proj = Proj{}) const
		{
			for (; first != last; ++first) {
				if (__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
					return false;
				}
			}
			return true;
		}

		template<InputRange R, class Proj = identity,
			IndirectUnaryPredicate<projected<iterator_t<R>, Proj>> Pred>
		constexpr bool operator()(R&& r, Pred pred, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(r), __stl2::end(r),
				std::ref(pred), std::ref(proj));
		}
	};

	inline constexpr __none_of_fn none_of {};
} STL2_CLOSE_NAMESPACE

#endif
