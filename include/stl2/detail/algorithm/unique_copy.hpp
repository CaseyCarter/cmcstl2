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
#include <stl2/detail/algorithm/return_types.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// unique_copy [alg.unique]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	struct unique_copy_result : __in_out_result<I, O> {};

	class __unique_copy_fn {
		template <class I, class S, class O, class R, class Proj>
		requires IndirectlyCopyableStorable<I, O>
		static constexpr unique_copy_result<I, O>
		impl(ext::priority_tag<0>, I first, S last, O result, R comp, Proj proj)
		{
			if (first != last) {
				iter_value_t<I> saved = *first;
				*result = saved;
				++result;
				while (++first != last) {
					iter_reference_t<I>&& v = *first;
					if (!__stl2::invoke(comp,
						__stl2::invoke(proj, std::forward<iter_reference_t<I>>(v)),
						__stl2::invoke(proj, saved))) {
						saved = std::forward<iter_reference_t<I>>(v);
						*result = saved;
						++result;
					}
				}
			}
			return {std::move(first), std::move(result)};
		}

		template <class I, class S, InputIterator O, class R, class Proj>
		requires Same<iter_value_t<I>, iter_value_t<O>>
		static constexpr unique_copy_result<I, O>
		impl(ext::priority_tag<1>, I first, S last, O result, R comp, Proj proj)
		{
			if (first != last) {
				*result = *first;
				while (++first != last) {
					iter_reference_t<I>&& v = *first;
					if (!__stl2::invoke(comp,
						__stl2::invoke(proj, std::forward<iter_reference_t<I>>(v)),
						__stl2::invoke(proj, *result))) {
						*++result = std::forward<iter_reference_t<I>>(v);
					}
				}
				++result;
			}
			return {std::move(first), std::move(result)};
		}

		template <ForwardIterator I, class S, class O, class R, class Proj>
		static constexpr unique_copy_result<I, O>
		impl(ext::priority_tag<2>, I first, S const last, O result, R comp, Proj proj)
		{
			if (first != last) {
				*result = *first;
				++result;
				auto m = first;
				while (++first != last) {
					iter_reference_t<I>&& v = *first;
					if (!__stl2::invoke(comp,
						__stl2::invoke(proj, std::forward<iter_reference_t<I>>(v)),
						__stl2::invoke(proj, *m))) {
						*result = std::forward<iter_reference_t<I>>(v);
						++result;
						m = first;
					}
				}
			}
			return {std::move(first), std::move(result)};
		}
	public:
		template<InputIterator I, Sentinel<I> S, WeaklyIncrementable O,
			class Proj = identity, IndirectRelation<projected<I, Proj>> R = ranges::equal_to<>>
		requires IndirectlyCopyable<I, O> &&
			(ForwardIterator<I> ||
			(InputIterator<O> && Same<iter_value_t<I>, iter_value_t<O>>) ||
			IndirectlyCopyableStorable<I, O>)
		constexpr unique_copy_result<I, O>
		operator()(I first, S last, O result, R comp = R{}, Proj proj = Proj{}) const
		{
			return __unique_copy_fn::impl(
				ext::priority_tag<2>{},
				std::move(first), std::move(last),
				std::move(result),
				std::ref(comp),
				std::ref(proj));
		}

		template<InputRange Rng, WeaklyIncrementable O, class Proj = identity,
			IndirectRelation<projected<iterator_t<Rng>, Proj>> R = ranges::equal_to<>>
		requires IndirectlyCopyable<iterator_t<Rng>, O> &&
			(ForwardIterator<iterator_t<Rng>> ||
			(InputIterator<O> && Same<iter_value_t<iterator_t<Rng>>, iter_value_t<O>>) ||
			IndirectlyCopyableStorable<iterator_t<Rng>, O>)
		constexpr unique_copy_result<safe_iterator_t<Rng>, O>
		operator()(Rng&& rng, O result, R comp = R{}, Proj proj = Proj{}) const
		{
			return __unique_copy_fn::impl(
				ext::priority_tag<2>{},
				__stl2::begin(rng), __stl2::end(rng),
				std::move(result),
				std::ref(comp),
				std::ref(proj));
		}
	};

	inline constexpr __unique_copy_fn unique_copy {};
} STL2_CLOSE_NAMESPACE

#endif
