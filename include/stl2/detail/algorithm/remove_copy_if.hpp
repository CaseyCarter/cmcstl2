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
#ifndef STL2_DETAIL_ALGORITHM_REMOVE_COPY_IF_HPP
#define STL2_DETAIL_ALGORITHM_REMOVE_COPY_IF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// remove_copy_if [alg.remove]
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O, class Pred,
		class Proj = identity>
	requires
		IndirectlyCopyable<I, O> &&
		IndirectUnaryPredicate<
			Pred, projected<I, Proj>>
	tagged_pair<tag::in(I), tag::out(O)>
	remove_copy_if(I first, S last, O result, Pred pred, Proj proj = Proj{})
	{
		for (; first != last; ++first) {
			reference_t<I>&& v = *first;
			if (!__stl2::invoke(pred, __stl2::invoke(proj, v))) {
				*result = std::forward<reference_t<I>>(v);
				++result;
			}
		}
		return {std::move(first), std::move(result)};
	}

	template <InputRange Rng, class O, class Pred, class Proj = identity>
	requires
		WeaklyIncrementable<__f<O>> &&
		IndirectlyCopyable<iterator_t<Rng>, __f<O>> &&
		IndirectUnaryPredicate<
			Pred, projected<iterator_t<Rng>, Proj>>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
	remove_copy_if(Rng&& rng, O&& result, Pred pred, Proj proj = Proj{})
	{
		return __stl2::remove_copy_if(
			__stl2::begin(rng), __stl2::end(rng), std::forward<O>(result),
			std::ref(pred), std::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
