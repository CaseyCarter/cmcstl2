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
#ifndef STL2_DETAIL_ALGORITHM_UPPER_BOUND_HPP
#define STL2_DETAIL_ALGORITHM_UPPER_BOUND_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/partition_point.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// upper_bound [upper.bound]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		struct __upper_bound_n_fn : private __niebloid {
			template<class I, class T, class Comp = less, class Proj = identity>
			requires ForwardIterator<__f<I>> &&
				IndirectStrictWeakOrder<Comp, const T*, projected<__f<I>, Proj>>
			constexpr __f<I> operator()(I&& first, iter_difference_t<__f<I>> n, const T& value,
				Comp comp = {}, Proj proj = {}) const {
				auto pred = [&](auto&& i) { return !__stl2::invoke(comp, value, i); };
				return ext::partition_point_n(std::forward<I>(first), n, pred, __stl2::ref(proj));
			}
		};

		inline constexpr __upper_bound_n_fn upper_bound_n {};
	}

	struct __upper_bound_fn : private __niebloid {
		template<class I, class S, class T, class Comp = less, class Proj = identity>
		requires ForwardIterator<__f<I>> && Sentinel<__f<S>, __f<I>> &&
			IndirectStrictWeakOrder<Comp, const T*, projected<__f<I>, Proj>>
		constexpr __f<I> operator()(I&& first, S&& last, const T& value,
			Comp comp = {}, Proj proj = {}) const {
			auto pred = [&](auto&& i) { return !__stl2::invoke(comp, value, i); };
			return __stl2::partition_point(std::forward<I>(first),
				std::forward<S>(last), pred, __stl2::ref(proj));
		}

		template<class I, class S, class T, class Comp = less, class Proj = identity>
		requires SizedSentinel<__f<S>, __f<I>> && ForwardIterator<__f<I>> &&
			Sentinel<__f<S>, __f<I>> &&
			IndirectStrictWeakOrder<Comp, const T*, projected<__f<I>, Proj>>
		constexpr __f<I> operator()(I&& first_, S&& last, const T& value,
			Comp comp = {}, Proj proj = {}) const  {
			auto first = std::forward<I>(first_);
			auto n = __stl2::distance(first, std::forward<S>(last));
			return ext::upper_bound_n(std::move(first), n, value,
				__stl2::ref(comp), __stl2::ref(proj));
		}

		template<ForwardRange Rng, class T, class Comp = less, class Proj = identity>
		requires
			IndirectStrictWeakOrder<Comp, const T*, projected<iterator_t<Rng>, Proj>>
		constexpr safe_iterator_t<Rng>
		operator()(Rng&& rng, const T& value, Comp comp = {}, Proj proj = {}) const {
			return (*this)(begin(rng), end(rng),
				value, __stl2::ref(comp), __stl2::ref(proj));
		}

		template<ForwardRange Rng, class T, class Comp = less, class Proj = identity>
		requires SizedRange<Rng> &&
			IndirectStrictWeakOrder<Comp, const T*, projected<iterator_t<Rng>, Proj>>
		constexpr safe_iterator_t<Rng>
		operator()(Rng&& rng, const T& value, Comp comp = {}, Proj proj = {}) const {
			return ext::upper_bound_n(begin(rng), __stl2::distance(rng),
				value, __stl2::ref(comp), __stl2::ref(proj));
		}
	};

	inline constexpr __upper_bound_fn upper_bound {};
} STL2_CLOSE_NAMESPACE

#endif
