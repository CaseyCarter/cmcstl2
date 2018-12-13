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
#ifndef STL2_DETAIL_ALGORITHM_FIND_IF_HPP
#define STL2_DETAIL_ALGORITHM_FIND_IF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// find_if [alg.find]
//
STL2_OPEN_NAMESPACE {
	struct __find_if_fn : private __niebloid {
		template<InputIterator I, Sentinel<I> S, class Proj = identity,
			IndirectUnaryPredicate<projected<I, Proj>> Pred>
		constexpr I operator()(I first, S last, Pred pred, Proj proj = {}) const
		{
			for (; first != last; ++first) {
				if (__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
					break;
				}
			}
			return first;
		}

		template<InputRange R, class Proj = identity,
			IndirectUnaryPredicate<projected<iterator_t<R>, Proj>> Pred>
		constexpr safe_iterator_t<R> operator()(R&& r, Pred pred, Proj proj = {}) const
		{
			return (*this)(begin(r), end(r),
				__stl2::ref(pred), __stl2::ref(proj));
		}
	};

	inline constexpr __find_if_fn find_if {};
} STL2_CLOSE_NAMESPACE

#endif
