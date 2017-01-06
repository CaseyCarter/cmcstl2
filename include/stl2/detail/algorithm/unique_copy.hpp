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
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// unique_copy [alg.unique]
//
STL2_OPEN_NAMESPACE {
	template <class I, class S, class O, class Proj, class R>
	tagged_pair<tag::in(I), tag::out(O)>
	__unique_copy(false_type, false_type, I first, S last, O result, R comp,
		Proj proj)
	{
		if (first != last) {
			value_type_t<I> saved = *first;
			*result = saved;
			++result;
			while (++first != last) {
				reference_t<I>&& v = *first;
				if (!__stl2::invoke(comp, __stl2::invoke(proj, v), __stl2::invoke(proj, saved))) {
					saved = __stl2::forward<reference_t<I>>(v);
					*result = saved;
					++result;
				}
			}
		}
		return {__stl2::move(first), __stl2::move(result)};
	}

	template <class I, class S, class O, class Proj, class R>
	tagged_pair<tag::in(I), tag::out(O)>
	__unique_copy(false_type, true_type, I first, S last, O result, R comp,
		Proj proj)
	{
		if (first != last) {
			*result = *first;
			while (++first != last) {
				reference_t<I>&& v = *first;
				if (!__stl2::invoke(comp, __stl2::invoke(proj, v), __stl2::invoke(proj, *result))) {
					*++result = __stl2::forward<reference_t<I>>(v);
				}
			}
			++result;
		}
		return {__stl2::move(first), __stl2::move(result)};
	}

	template <class I, class S, class O, class Proj, class R>
	tagged_pair<tag::in(I), tag::out(O)>
	__unique_copy(true_type, auto, I first, S last, O result, R comp,
		Proj proj)
	{
		if (first != last) {
			*result = *first;
			++result;
			auto m = first;
			while (++first != last) {
				reference_t<I>&& v = *first;
				if (!__stl2::invoke(comp, __stl2::invoke(proj, v), __stl2::invoke(proj, *m))) {
					*result = __stl2::forward<reference_t<I>>(v);
					++result;
					m = first;
				}
			}
		}
		return {__stl2::move(first), __stl2::move(result)};
	}

	template <class I, class O>
	constexpr bool __unique_copy_req =
		models::IndirectlyCopyable<I, O> &&
			(models::ForwardIterator<I> ||
			 models::ForwardIterator<O> ||
			 models::IndirectlyCopyableStorable<I, O>);

	template <class I, class S, class O, class R = equal_to<>,
		class Proj = identity>
	requires
		models::InputIterator<__f<I>> &&
		models::Sentinel<__f<S>, __f<I>> &&
		models::WeaklyIncrementable<__f<O>> &&
		__unique_copy_req<__f<I>, __f<O>> &&
		models::IndirectInvocableRelation<
			__f<R>, projected<__f<I>, Proj>>
	tagged_pair<tag::in(__f<I>), tag::out(__f<O>)>
	unique_copy(I&& first, S&& last, O&& result, R comp = R{},
		Proj proj = Proj{})
	{
		return __stl2::__unique_copy(
			meta::bool_<models::ForwardIterator<__f<I>>>{},
			meta::bool_<models::ForwardIterator<__f<O>>>{},
			__stl2::forward<I>(first), __stl2::forward<S>(last),
			__stl2::forward<O>(result),
			__stl2::ref(comp),
			__stl2::ref(proj));
	}

	template <InputRange Rng, class O, class R = equal_to<>,
		class Proj = identity>
	requires
		models::WeaklyIncrementable<__f<O>> &&
		__unique_copy_req<iterator_t<Rng>, __f<O>> &&
		models::IndirectInvocableRelation<
			__f<R>, projected<iterator_t<Rng>, Proj>>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
	unique_copy(Rng&& rng, O&& result, R comp = R{}, Proj proj = Proj{})
	{
		return __stl2::__unique_copy(
			meta::bool_<models::ForwardIterator<iterator_t<Rng>>>{},
			meta::bool_<models::ForwardIterator<__f<O>>>{},
			__stl2::begin(rng), __stl2::end(rng),
			__stl2::forward<O>(result),
			__stl2::ref(comp),
			__stl2::ref(proj));
	}

	// Extension
	template <class E, class O, class R = equal_to<>, class Proj = identity>
	requires
		models::WeaklyIncrementable<__f<O>> &&
		__unique_copy_req<const E*, __f<O>> &&
		models::IndirectInvocableRelation<
			__f<R>, projected<const E*, Proj>>
	tagged_pair<tag::in(dangling<const E*>), tag::out(__f<O>)>
	unique_copy(std::initializer_list<E>&& rng, O&& result, R comp = R{},
		Proj proj = Proj{})
	{
		return __stl2::__unique_copy(
			true_type{},
			false_type{}, // Don't care.
			__stl2::begin(rng), __stl2::end(rng),
			__stl2::forward<O>(result),
			__stl2::ref(comp),
			__stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
