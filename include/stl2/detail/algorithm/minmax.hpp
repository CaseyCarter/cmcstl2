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
#ifndef STL2_DETAIL_ALGORITHM_MINMAX_HPP
#define STL2_DETAIL_ALGORITHM_MINMAX_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// minmax [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	namespace __minmax {
		template <InputRange Rng, class Comp = less<>, class Proj = identity>
		requires
			models::Copyable<value_type_t<iterator_t<Rng>>> &&
			models::IndirectInvocableStrictWeakOrder<
				Comp, projected<iterator_t<Rng>, Proj>>
		constexpr tagged_pair<tag::min(value_type_t<iterator_t<Rng>>),
			tag::max(value_type_t<iterator_t<Rng>>)>
		impl(Rng&& rng, Comp comp = Comp{}, Proj proj = Proj{})
		{
			using V = value_type_t<iterator_t<Rng>>;
			auto first = __stl2::begin(rng);
			auto last = __stl2::end(rng);
			STL2_EXPECT(first != last);
			auto result = tagged_pair<tag::min(V), tag::max(V)>{
				*first, *first
			};
			if (++first != last) {
				{
					auto&& tmp = *first;
					if (__stl2::invoke(comp, __stl2::invoke(proj, tmp), __stl2::invoke(proj, result.first))) {
						result.first = (decltype(tmp)&&)tmp;
					} else {
						result.second = (decltype(tmp)&&)tmp;
					}
				}
				while (++first != last) {
					auto tmp1 = V{*first};
					if (++first == last) {
						if (__stl2::invoke(comp, __stl2::invoke(proj, tmp1), __stl2::invoke(proj, result.first))) {
							result.first = __stl2::move(tmp1);
						} else if (!__stl2::invoke(comp, __stl2::invoke(proj, tmp1), __stl2::invoke(proj, result.second))) {
							result.second = __stl2::move(tmp1);
						}
						break;
					}

					auto&& tmp2 = *first;
					if (__stl2::invoke(comp, __stl2::invoke(proj, tmp2), __stl2::invoke(proj, tmp1))) {
						if (__stl2::invoke(comp, __stl2::invoke(proj, tmp2), __stl2::invoke(proj, result.first))) {
							result.first = (decltype(tmp2)&&)tmp2;
						}
						if (!__stl2::invoke(comp, __stl2::invoke(proj, tmp1), __stl2::invoke(proj, result.second))) {
							result.second = __stl2::move(tmp1);
						}
					} else {
						if (__stl2::invoke(comp, __stl2::invoke(proj, tmp1), __stl2::invoke(proj, result.first))) {
							result.first = __stl2::move(tmp1);
						}
						if (!__stl2::invoke(comp, __stl2::invoke(proj, tmp2), __stl2::invoke(proj, result.second))) {
							result.second = (decltype(tmp2)&&)tmp2;
						}
					}
				}
			}
			return result;
		}
	}

	template <class T, class Comp = less<>, class Proj = identity>
	requires
		models::IndirectInvocableStrictWeakOrder<
			Comp, projected<const T*, Proj>>
	constexpr tagged_pair<tag::min(const T&), tag::max(const T&)>
	minmax(const T& a, const T& b, Comp comp = Comp{}, Proj proj = Proj{})
	{
		if (__stl2::invoke(comp, __stl2::invoke(proj, b), __stl2::invoke(proj, a))) {
			return {b, a};
		} else {
			return {a, b};
		}
	}

	template <InputRange Rng, class Comp = less<>, class Proj = identity>
	requires
		models::Copyable<value_type_t<iterator_t<Rng>>> &&
		models::IndirectInvocableStrictWeakOrder<
			Comp, projected<iterator_t<Rng>, Proj>>
	STL2_CONSTEXPR_EXT tagged_pair<tag::min(value_type_t<iterator_t<Rng>>),
		tag::max(value_type_t<iterator_t<Rng>>)>
	minmax(Rng&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __minmax::impl(rng, __stl2::ref(comp), __stl2::ref(proj));
	}

	template <Copyable T, class Comp = less<>, class Proj = identity>
	requires
		models::IndirectInvocableStrictWeakOrder<
			Comp, projected<const T*, Proj>>
	constexpr tagged_pair<tag::min(T), tag::max(T)>
	minmax(std::initializer_list<T>&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __minmax::impl(rng, __stl2::ref(comp), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
