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
#ifndef STL2_DETAIL_ALGORITHM_LOWER_BOUND_HPP
#define STL2_DETAIL_ALGORITHM_LOWER_BOUND_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/partition_point.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// lower_bound [lower.bound]
//
STL2_OPEN_NAMESPACE {
	template <class C, class T>
	struct __lower_bound_fn {
		decay_t<C> comp_;
		const T& value_;

		constexpr __lower_bound_fn(C comp, const T& value) :
			comp_(__stl2::ref(comp)), value_(value) {}

		constexpr bool operator()(auto&& i) const {
			return __stl2::invoke(comp_, i, value_);
		}
	};

	namespace ext {
		template <class I, class T, class Comp = less<>, class Proj = identity>
		requires
			ForwardIterator<__f<I>> &&
			IndirectStrictWeakOrder<
				Comp, const T*, projected<__f<I>, Proj>>
		__f<I> lower_bound_n(I&& first, difference_type_t<__f<I>> n,
			const T& value, Comp comp = Comp{}, Proj proj = Proj{})
		{
			return __stl2::ext::partition_point_n(
				__stl2::forward<I>(first), n,
				__lower_bound_fn<Comp, T>{__stl2::ref(comp), value},
				__stl2::ref(proj));
		}
	}

	template <class I, class S, class T, class Comp = less<>, class Proj = identity>
	requires
		ForwardIterator<__f<I>> &&
		Sentinel<__f<S>, __f<I>> &&
		IndirectStrictWeakOrder<
			Comp, const T*, projected<__f<I>, Proj>>
	__f<I> lower_bound(I&& first, S&& last, const T& value,
		Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __stl2::partition_point(
			__stl2::forward<I>(first), __stl2::forward<S>(last),
			__lower_bound_fn<Comp, T>{__stl2::ref(comp), value},
			__stl2::ref(proj));
	}

	template <class I, class S, class T, class Comp = less<>, class Proj = identity>
	requires
		SizedSentinel<__f<S>, __f<I>> &&
		ForwardIterator<__f<I>> &&
		Sentinel<__f<S>, __f<I>> &&
		IndirectStrictWeakOrder<
			Comp, const T*, projected<__f<I>, Proj>>
	__f<I> lower_bound(I&& first_, S&& last, const T& value,
		Comp comp = Comp{}, Proj proj = Proj{})
	{
		auto first = __stl2::forward<I>(first_);
		auto n = __stl2::distance(first, __stl2::forward<S>(last));
		return __stl2::ext::lower_bound_n(__stl2::move(first), n, value,
			__stl2::ref(comp), __stl2::ref(proj));
	}

	template <ForwardRange Rng, class T, class Comp = less<>, class Proj = identity>
	requires
		IndirectStrictWeakOrder<
			Comp, const T*, projected<iterator_t<Rng>, Proj>>
	safe_iterator_t<Rng>
	lower_bound(Rng&& rng, const T& value, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __stl2::lower_bound(__stl2::begin(rng), __stl2::end(rng), value,
			__stl2::ref(comp), __stl2::ref(proj));
	}

	template <ForwardRange Rng, class T, class Comp = less<>, class Proj = identity>
	requires
		SizedRange<Rng> &&
		IndirectStrictWeakOrder<
			Comp, const T*, projected<iterator_t<Rng>, Proj>>
	safe_iterator_t<Rng>
	lower_bound(Rng&& rng, const T& value, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __stl2::ext::lower_bound_n(
			__stl2::begin(rng), __stl2::distance(rng), value,
			__stl2::ref(comp), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
