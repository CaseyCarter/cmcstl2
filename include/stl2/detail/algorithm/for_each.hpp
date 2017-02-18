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
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// for_each [alg.for_each]
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I, Sentinel<I> S, class F, class Proj = identity>
	requires
		IndirectInvocable<
			F, projected<I, Proj>>()
	tagged_pair<tag::in(I), tag::fun(F)>
	for_each(I first, S last, F fun, Proj proj = Proj{})
	{
		for (; first != last; ++first) {
			static_cast<void>(__stl2::invoke(fun, __stl2::invoke(proj, *first)));
		}
		return {__stl2::move(first), __stl2::move(fun)};
	}

	template <InputRange Rng, class F, class Proj = identity>
	requires
		IndirectInvocable<
			F, projected<iterator_t<Rng>, Proj>>()
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::fun(F)>
	for_each(Rng&& rng, F fun, Proj proj = Proj{})
	{
		return {__stl2::for_each(__stl2::begin(rng), __stl2::end(rng),
			__stl2::ref(fun), __stl2::ref(proj)).in(), __stl2::move(fun)};
	}
} STL2_CLOSE_NAMESPACE

#endif
