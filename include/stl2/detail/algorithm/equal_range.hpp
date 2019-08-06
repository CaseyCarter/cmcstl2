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
#ifndef STL2_DETAIL_ALGORITHM_EQUAL_RANGE_HPP
#define STL2_DETAIL_ALGORITHM_EQUAL_RANGE_HPP

#include <stl2/detail/algorithm/lower_bound.hpp>
#include <stl2/detail/algorithm/upper_bound.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/view/subrange.hpp>

///////////////////////////////////////////////////////////////////////////
// equal_range [equal.range]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		struct __equal_range_n_fn {
			template<forward_iterator I, class T, class Comp = less, class Proj = identity>
			requires indirect_strict_weak_order<Comp, const T*, projected<I, Proj>>
			constexpr subrange<I>
			operator()(I first, iter_difference_t<I> dist, const T& value,
				Comp comp = {}, Proj proj = {}) const {
				if (0 < dist) {
					do {
						auto half = dist / 2;
						auto middle = next(first, half);
						auto&& v = *middle;
						auto&& pv = __stl2::invoke(proj, std::forward<decltype(v)>(v));
						if (__stl2::invoke(comp, pv, value)) {
							first = std::move(middle);
							++first;
							dist -= half + 1;
						} else if (__stl2::invoke(comp, value, pv)) {
							dist = half;
						} else {
							return {
								ext::lower_bound_n(
									std::move(first), half, value,
									__stl2::ref(comp), __stl2::ref(proj)),
								ext::upper_bound_n(next(middle),
									dist - (half + 1), value,
									__stl2::ref(comp), __stl2::ref(proj))
							};
						}
					} while (0 != dist);
				}
				return {first, first};
			}
		};

		inline constexpr __equal_range_n_fn equal_range_n {};
	} // namespace ext

	struct __equal_range_fn : private __niebloid {
		template<forward_iterator I, sentinel_for<I> S, class T, class Comp = less,
			class Proj = identity>
		requires indirect_strict_weak_order<Comp, const T*, projected<I, Proj>>
		constexpr subrange<I> operator()(I first, S last, const T& value,
			Comp comp = {}, Proj proj = {}) const
		{
			if constexpr (sized_sentinel_for<S, I>) {
				auto len = distance(first, std::move(last));
				return ext::equal_range_n(std::move(first), len, value,
					__stl2::ref(comp), __stl2::ref(proj));
			} else {
				// Probe exponentially for either end-of-range, an iterator that
				// is past the equal range (i.e., denotes an element greater
				// than value), or is in the equal range (denotes an element equal
				// to value).
				auto dist = iter_difference_t<I>{1};
				while (true) {
					auto mid = first;
					auto d = advance(mid, dist, last);
					STL2_EXPECT(d >= 0);
					if (d || mid == last) {
						// at the end of the input range
						return ext::equal_range_n(
							std::move(first), dist - d, value,
							__stl2::ref(comp), __stl2::ref(proj));
					}
					auto&& v = *mid;
					auto&& pv = __stl2::invoke(proj, std::forward<decltype(v)>(v));
					// if value < *mid, mid is after the target range.
					if (__stl2::invoke(comp, value, pv)) {
						return ext::equal_range_n(
							std::move(first), dist, value,
							__stl2::ref(comp), __stl2::ref(proj));
					} else if (!__stl2::invoke(comp, pv, value)) {
						// *mid == value: the lower bound is <= mid, and the upper bound is > mid.
						return {
							ext::lower_bound_n(std::move(first), dist, value,
								__stl2::ref(comp), __stl2::ref(proj)),
							upper_bound(std::move(mid), std::move(last),
								value, __stl2::ref(comp), __stl2::ref(proj))
						};
					}
					// *mid < value, mid is before the target range.
					first = std::move(mid);
					++first;
					dist *= 2;
				}
			}
		}

		template<forward_range Rng, class T, class Comp = less, class Proj = identity>
		requires indirect_strict_weak_order<Comp, const T*, projected<iterator_t<Rng>, Proj>>
		constexpr safe_subrange_t<Rng>
		operator()(Rng&& rng, const T& value, Comp comp = {}, Proj proj = {}) const {
			if constexpr (sized_range<Rng>) {
				return ext::equal_range_n(begin(rng), size(rng), value, __stl2::ref(comp),
					__stl2::ref(proj));
			} else {
				return (*this)(begin(rng), end(rng), value, __stl2::ref(comp),
					__stl2::ref(proj));
			}
		}
	};

	inline constexpr __equal_range_fn equal_range {};
} STL2_CLOSE_NAMESPACE

#endif
