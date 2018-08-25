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
#include <stl2/detail/algorithm/return_types.hpp>
#include <stl2/detail/algorithm/for_each.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// for_each [alg.for_each]
//
STL2_OPEN_NAMESPACE {
	template<class I, class F>
	struct for_each_result : __in_fun_result<I ,F> {};

	struct __for_each_fn {
		template<InputIterator I, Sentinel<I> S, class Proj = identity,
			IndirectUnaryInvocable<projected<I, Proj>> Fun>
		constexpr for_each_result<I, Fun> operator()(I first, S last, Fun f, Proj proj = Proj{}) const
		{
			for (; first != last; ++first) {
				static_cast<void>(__stl2::invoke(f, __stl2::invoke(proj, *first)));
			}
			return {std::move(first), std::move(f)};
		}

		template<InputRange Rng, class Proj = identity,
			IndirectUnaryInvocable<projected<iterator_t<Rng>, Proj>> Fun>
		constexpr for_each_result<safe_iterator_t<Rng>, Fun>
		operator()(Rng&& rng, Fun f, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), std::move(f), std::ref(proj));
		}
	};

	inline constexpr __for_each_fn for_each {};
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_ALGORITHM_FOR_EACH_HPP
