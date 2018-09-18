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
#ifndef STL2_DETAIL_ALGORITHM_FOR_EACH_HPP
#define STL2_DETAIL_ALGORITHM_FOR_EACH_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// for_each [alg.for_each]
//
STL2_OPEN_NAMESPACE {
	struct __for_each_fn {
		template<InputIterator I, Sentinel<I> S, class Proj = identity,
			IndirectUnaryInvocable<projected<I, Proj>> F>
		constexpr tagged_pair<tag::in(I), tag::fun(F)>
		operator()(I first, S last, F fun, Proj proj = Proj{}) const
		{
			for (; first != last; ++first) {
				static_cast<void>(__stl2::invoke(fun, __stl2::invoke(proj, *first)));
			}
			return {std::move(first), std::move(fun)};
		}

		template<InputRange R, class Proj = identity,
			IndirectUnaryInvocable<projected<iterator_t<R>, Proj>> F>
		constexpr tagged_pair<tag::in(safe_iterator_t<R>), tag::fun(F)>
		operator()(R&& r, F fun, Proj proj = Proj{}) const
		{
			return {(*this)(__stl2::begin(r), __stl2::end(r),
				std::ref(fun), std::ref(proj)).in(), std::move(fun)};
		}
	};

	inline constexpr __for_each_fn for_each {};
} STL2_CLOSE_NAMESPACE

#endif
