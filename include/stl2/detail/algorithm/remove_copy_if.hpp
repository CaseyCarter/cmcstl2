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
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// remove_copy_if [alg.remove]
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O, class Pred,
		class Proj = identity>
	requires
		models::IndirectlyCopyable<I, O> &&
		models::IndirectInvocablePredicate<
			Pred, projected<I, Proj>>
	tagged_pair<tag::in(I), tag::out(O)>
	remove_copy_if(I first, S last, O result, Pred pred, Proj proj = Proj{})
	{
		for (; first != last; ++first) {
			reference_t<I>&& v = *first;
			if (!__stl2::invoke(pred, __stl2::invoke(proj, v))) {
				*result = __stl2::forward<reference_t<I>>(v);
				++result;
			}
		}
		return {__stl2::move(first), __stl2::move(result)};
	}

	template <InputRange Rng, class O, class Pred, class Proj = identity>
	requires
		models::WeaklyIncrementable<__f<O>> &&
		models::IndirectlyCopyable<iterator_t<Rng>, __f<O>> &&
		models::IndirectInvocablePredicate<
			Pred, projected<iterator_t<Rng>, Proj>>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
	remove_copy_if(Rng&& rng, O&& result, Pred pred, Proj proj = Proj{})
	{
		return __stl2::remove_copy_if(
			__stl2::begin(rng), __stl2::end(rng), __stl2::forward<O>(result),
			__stl2::ref(pred), __stl2::ref(proj));
	}

	// Extension
	template <class E, class O, class Pred, class Proj = identity>
	requires
		models::WeaklyIncrementable<__f<O>> &&
		models::IndirectlyCopyable<const E*, __f<O>> &&
		models::IndirectInvocablePredicate<
			Pred, projected<const E*, Proj>>
	tagged_pair<tag::in(dangling<const E*>), tag::out(__f<O>)>
	remove_copy_if(std::initializer_list<E>&& rng, O&& result,
								 Pred pred, Proj proj = Proj{})
	{
		return __stl2::remove_copy_if(
			__stl2::begin(rng), __stl2::end(rng), __stl2::forward<O>(result),
			__stl2::ref(pred), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
