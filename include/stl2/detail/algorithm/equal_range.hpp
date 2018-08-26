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

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/lower_bound.hpp>
#include <stl2/detail/algorithm/upper_bound.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/view/subrange.hpp>

///////////////////////////////////////////////////////////////////////////
// equal_range [equal.range]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		template <ForwardIterator I, class T, class Proj = identity,
			IndirectStrictWeakOrder<const T*, projected<I, Proj>> Comp = ranges::less<>>
		subrange<I> equal_range_n(I first, iter_difference_t<I> dist, const T& value,
			Comp comp = Comp{}, Proj proj = Proj{})
		{
			if (0 < dist) {
				do {
					auto half = dist / 2;
					auto middle = __stl2::next(first, half);
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
								std::ref(comp), std::ref(proj)),
							ext::upper_bound_n(__stl2::next(middle),
								dist - (half + 1), value,
								std::ref(comp), std::ref(proj))
						};
					}
				} while (0 != dist);
			}
			return {first, first};
		}
	}

	class __equal_range_fn {
		template<ForwardIterator I, Sentinel<I> S, class T, class Proj, class Comp>
		static constexpr subrange<I> impl(I first, S last, const T& value, Comp comp, Proj proj)
		{
			// Probe exponentially for either end-of-range, an iterator that
			// is past the equal range (i.e., denotes an element greater
			// than value), or is in the equal range (denotes an element equal
			// to value).
			auto dist = iter_difference_t<I>{1};
			while (true) {
				auto mid = first;
				auto d = __stl2::advance(mid, dist, last);
				STL2_EXPECT(d >= 0);
				if (d || mid == last) {
					// at the end of the input range
					return ext::equal_range_n(
						std::move(first), dist - d, value,
						std::ref(comp), std::ref(proj));
				}
				auto&& v = *mid;
				auto&& pv = __stl2::invoke(proj, std::forward<decltype(v)>(v));
				// if value < *mid, mid is after the target range.
				if (__stl2::invoke(comp, value, pv)) {
					return ext::equal_range_n(
						std::move(first), dist, value,
						std::ref(comp), std::ref(proj));
				} else if (!__stl2::invoke(comp, pv, value)) {
					// *mid == value: the lower bound is <= mid, and the upper bound is > mid.
					return {
						ext::lower_bound_n(std::move(first), dist, value,
							std::ref(comp), std::ref(proj)),
						__stl2::upper_bound(std::move(mid), std::move(last),
							value, std::ref(comp), std::ref(proj))
					};
				}
				// *mid < value, mid is before the target range.
				first = std::move(mid);
				++first;
				dist *= 2;
			}
		}

		template<ForwardIterator I, SizedSentinel<I> S, class T, class Proj, class Comp>
		static constexpr subrange<I> impl(I first, S last, const T& value, Comp comp, Proj proj)
		{
			auto len = __stl2::distance(first, std::move(last));
			return __stl2::ext::equal_range_n(std::move(first), len, value,
				std::ref(comp), std::ref(proj));
		}

		template<ForwardRange R, class T, class Proj, class Comp>
		static constexpr safe_subrange_t<R> impl(R&& r, const T& value, Comp comp, Proj proj)
		{
			return __equal_range_fn::impl(
				__stl2::begin(r), __stl2::end(r), value,
				std::ref(comp), std::ref(proj));
		}

		template<ForwardRange R, class T, class Proj, class Comp>
		requires SizedRange<R>
		static constexpr safe_subrange_t<R> impl(R&& r, const T& value, Comp comp, Proj proj)
		{
			return __stl2::ext::equal_range_n(
				__stl2::begin(r), __stl2::size(r), value,
				std::ref(comp), std::ref(proj));
		}
	public:
		template<ForwardIterator I, Sentinel<I> S, class T, class Proj = identity,
			IndirectStrictWeakOrder<const T*, projected<I, Proj>> Comp = ranges::less<>>
		constexpr subrange<I>
		operator()(I first, S last, const T& value, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return __equal_range_fn::impl(
				std::move(first), std::move(last), value,
				std::ref(comp), std::ref(proj));
		}

		template<ForwardRange R, class T, class Proj = identity,
			IndirectStrictWeakOrder<const T*, projected<iterator_t<R>, Proj>> Comp = ranges::less<>>
		constexpr safe_subrange_t<R>
		operator()(R&& r, const T& value, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return __equal_range_fn::impl(std::forward<R>(r), value, std::ref(comp), std::ref(proj));
		}
	};

	inline constexpr __equal_range_fn equal_range {};
} STL2_CLOSE_NAMESPACE

#endif
