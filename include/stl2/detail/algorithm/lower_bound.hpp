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
	namespace ext {
		struct __lower_bound_n_fn : private __niebloid {
			template<class I, class T, class Comp = less, class Proj = identity>
			requires ForwardIterator<__f<I>> &&
				IndirectStrictWeakOrder<Comp, const T*, projected<__f<I>, Proj>>
			constexpr __f<I> operator()(I&& first, iter_difference_t<__f<I>> n,
				const T& value, Comp comp = {}, Proj proj = {}) const {
				auto pred = [&](auto&& i) -> bool {
					return __stl2::invoke(comp, i, value);
				};
				return __stl2::ext::partition_point_n(
					std::forward<I>(first), n, pred, std::ref(proj));
			}
		};

		inline constexpr __lower_bound_n_fn lower_bound_n {};
	}

	struct __lower_bound_fn : private __niebloid {
		template<class I, class S, class T, class Comp = less, class Proj = identity>
		requires ForwardIterator<__f<I>> && Sentinel<__f<S>, __f<I>> &&
			IndirectStrictWeakOrder<Comp, const T*, projected<__f<I>, Proj>>
		constexpr __f<I> operator()(I&& first, S&& last, const T& value,
			Comp comp = {}, Proj proj = {}) const {
			if constexpr (SizedSentinel<__f<S>, __f<I>>) {
				auto first_ = std::forward<I>(first);
				auto n = __stl2::distance(first_, std::forward<S>(last));
				return __stl2::ext::lower_bound_n(std::move(first_), n, value,
					std::ref(comp), std::ref(proj));
			} else {
				auto pred = [&](auto&& i) -> bool {
					return __stl2::invoke(comp, i, value);
				};
				return __stl2::partition_point(
					std::forward<I>(first), std::forward<S>(last), pred, std::ref(proj));
			}
		}

		template<ForwardRange Rng, class T, class Comp = less, class Proj = identity>
		requires
			IndirectStrictWeakOrder<Comp, const T*, projected<iterator_t<Rng>, Proj>>
		constexpr safe_iterator_t<Rng>
		operator()(Rng&& rng, const T& value, Comp comp = {}, Proj proj = {}) const {
			if constexpr (SizedRange<Rng>) {
				return __stl2::ext::lower_bound_n(
					__stl2::begin(rng), __stl2::distance(rng), value,
					std::ref(comp), std::ref(proj));
			} else {
				return (*this)(__stl2::begin(rng), __stl2::end(rng), value,
					std::ref(comp), std::ref(proj));
			}
		}
	};

	inline constexpr __lower_bound_fn lower_bound {};
} STL2_CLOSE_NAMESPACE

#endif
