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

#include <stl2/detail/algorithm/partition_point.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// upper_bound [upper.bound]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		struct __upper_bound_n_fn {
			template<class I, class T, class Comp = less, class Proj = identity>
			requires forward_iterator<__f<I>> &&
				indirect_strict_weak_order<Comp, const T*, projected<__f<I>, Proj>>
			constexpr __f<I> operator()(I&& first_, iter_difference_t<__f<I>> n,
				const T& value, Comp comp = {}, Proj proj = {}) const
			{
				auto pred = [&](auto&& i) {
					return !__stl2::invoke(comp, value, i);
				};
				return ext::partition_point_n(std::forward<I>(first_), n,
					std::move(pred), __stl2::ref(proj));
			}
		};

		inline constexpr __upper_bound_n_fn upper_bound_n {};
	}

	struct __upper_bound_fn : private __niebloid {
		template<forward_iterator I, sentinel_for<I> S, class T,
			class Proj = identity,
			indirect_strict_weak_order<const T*, projected<I, Proj>> Comp = less>
		constexpr I operator()(I first, S last, const T& value,
			Comp comp = {}, Proj proj = {}) const
		{
			if constexpr (sized_sentinel_for<S, I>) {
				auto n = distance(first, std::move(last));
				return ext::upper_bound_n(std::move(first), n, value,
					__stl2::ref(comp), __stl2::ref(proj));
			} else {
				auto pred = [&](auto&& i) {
					return !__stl2::invoke(comp, value, i);
				};
				return partition_point(std::move(first), std::move(last),
					std::move(pred), __stl2::ref(proj));
			}
		}

		template<forward_range R, class T, class Proj = identity,
			indirect_strict_weak_order<const T*,
				projected<iterator_t<R>, Proj>> Comp = less>
		constexpr safe_iterator_t<R>
		operator()(R&& r, const T& value, Comp comp = {}, Proj proj = {}) const {
			if constexpr (sized_range<R>) {
				return ext::upper_bound_n(begin(r), distance(r), value,
					__stl2::ref(comp), __stl2::ref(proj));
			} else {
				return (*this)(begin(r), end(r), value, __stl2::ref(comp),
					__stl2::ref(proj));
			}
		}
	};

	inline constexpr __upper_bound_fn upper_bound {};
} STL2_CLOSE_NAMESPACE

#endif
