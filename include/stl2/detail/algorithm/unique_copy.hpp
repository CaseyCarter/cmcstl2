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
#ifndef STL2_DETAIL_ALGORITHM_UNIQUE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_UNIQUE_COPY_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// unique_copy [alg.unique]
//
STL2_OPEN_NAMESPACE {
	template <class I, class S, class O, class R, class Proj>
	requires IndirectlyCopyableStorable<I, O>
	tagged_pair<tag::in(I), tag::out(O)>
	__unique_copy(ext::priority_tag<0>, I first, S last, O result, R comp, Proj proj)
	{
		if (first != last) {
			value_type_t<I> saved = *first;
			*result = saved;
			++result;
			while (++first != last) {
				reference_t<I>&& v = *first;
				if (!__stl2::invoke(comp,
					__stl2::invoke(proj, std::forward<reference_t<I>>(v)),
					__stl2::invoke(proj, saved))) {
					saved = std::forward<reference_t<I>>(v);
					*result = saved;
					++result;
				}
			}
		}
		return {std::move(first), std::move(result)};
	}

	template <class I, class S, InputIterator O, class R, class Proj>
	requires Same<value_type_t<I>, value_type_t<O>>
	tagged_pair<tag::in(I), tag::out(O)>
	__unique_copy(ext::priority_tag<1>, I first, S last, O result, R comp, Proj proj)
	{
		if (first != last) {
			*result = *first;
			while (++first != last) {
				reference_t<I>&& v = *first;
				if (!__stl2::invoke(comp,
					__stl2::invoke(proj, std::forward<reference_t<I>>(v)),
					__stl2::invoke(proj, *result))) {
					*++result = std::forward<reference_t<I>>(v);
				}
			}
			++result;
		}
		return {std::move(first), std::move(result)};
	}

	template <ForwardIterator I, class S, class O, class R, class Proj>
	tagged_pair<tag::in(I), tag::out(O)>
	__unique_copy(ext::priority_tag<2>, I first, S const last, O result, R comp, Proj proj)
	{
		if (first != last) {
			*result = *first;
			++result;
			auto m = first;
			while (++first != last) {
				reference_t<I>&& v = *first;
				if (!__stl2::invoke(comp,
					__stl2::invoke(proj, std::forward<reference_t<I>>(v)),
					__stl2::invoke(proj, *m))) {
					*result = std::forward<reference_t<I>>(v);
					++result;
					m = first;
				}
			}
		}
		return {std::move(first), std::move(result)};
	}

	template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O,
		class R = equal_to<>, class Proj = identity>
	requires
		IndirectlyCopyable<I, O> &&
		IndirectRelation<R, projected<I, Proj>> &&
		(ForwardIterator<I> ||
		 InputIterator<O> && Same<value_type_t<I>, value_type_t<O>> ||
		 IndirectlyCopyableStorable<I, O>)
	tagged_pair<tag::in(I), tag::out(O)>
	unique_copy(I first, S last, O result, R comp = R{},
		Proj proj = Proj{})
	{
		return __stl2::__unique_copy(
			ext::priority_tag<2>{},
			std::move(first), std::move(last),
			std::move(result),
			std::ref(comp),
			std::ref(proj));
	}

	template <InputRange Rng, WeaklyIncrementable O, class R = equal_to<>,
		class Proj = identity>
	requires
		IndirectlyCopyable<iterator_t<Rng>, O> &&
		IndirectRelation<R, projected<iterator_t<Rng>, Proj>> &&
		(ForwardIterator<iterator_t<Rng>> ||
		 InputIterator<O> && Same<value_type_t<iterator_t<Rng>>, value_type_t<O>> ||
		 IndirectlyCopyableStorable<iterator_t<Rng>, O>)
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
	unique_copy(Rng&& rng, O result, R comp = R{}, Proj proj = Proj{})
	{
		return __stl2::__unique_copy(
			ext::priority_tag<2>{},
			__stl2::begin(rng), __stl2::end(rng),
			std::move(result),
			std::ref(comp),
			std::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
