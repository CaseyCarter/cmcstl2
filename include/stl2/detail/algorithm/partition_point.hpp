// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
#ifndef STL2_DETAIL_ALGORITHM_PARTITION_POINT_HPP
#define STL2_DETAIL_ALGORITHM_PARTITION_POINT_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// partition_point [alg.partitions]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		struct __partition_point_n_fn {
			template<forward_iterator I, class Proj = identity,
				indirect_unary_predicate<projected<I, Proj>> Pred>
			constexpr I operator()(I first, iter_difference_t<I> n, Pred pred,
				Proj proj = {}) const
			{
				STL2_EXPECT(0 <= n);
				while (n != 0) {
					auto const half = n / 2;
					auto middle = next(ext::uncounted(first), half);
					if (__stl2::invoke(pred, __stl2::invoke(proj, *middle))) {
						first = ext::recounted(first, std::move(++middle), half + 1);
						n -= half + 1;
					} else {
						n = half;
					}
				}
				return first;
			}
		};

		inline constexpr __partition_point_n_fn partition_point_n {};
	}

	struct __partition_point_fn : private __niebloid {
		template<forward_iterator I, sentinel_for<I> S, class Proj = identity,
			indirect_unary_predicate<projected<I, Proj>> Pred>
		constexpr I operator()(I first, S last, Pred pred, Proj proj = {}) const {
			if constexpr (sized_sentinel_for<S, I>) {
				auto n = distance(first, std::move(last));
				return ext::partition_point_n(std::move(first), n,
					__stl2::ref(pred), __stl2::ref(proj));
			} else {
				// Probe exponentially for either end-of-range or an iterator
				// that is past the partition point (i.e., does not satisfy pred).
				auto n = iter_difference_t<I>{1};
				while (true) {
					auto m = first;
					auto d = advance(m, n, last);
					if (bool(m == last) || !bool(__stl2::invoke(pred, __stl2::invoke(proj, *m)))) {
						n -= d;
						return ext::partition_point_n(std::move(first), n,
							std::move(pred), std::move(proj));
					}
					first = std::move(m);
					n *= 2;
				}
			}
		}

		template<forward_range R, class Proj = identity,
			indirect_unary_predicate<projected<iterator_t<R>, Proj>> Pred>
		constexpr safe_iterator_t<R>
		operator()(R&& r, Pred pred, Proj proj = {}) const {
			if constexpr (sized_range<R>) {
				return ext::partition_point_n(begin(r), distance(r),
					__stl2::ref(pred), __stl2::ref(proj));
			} else {
				return (*this)(begin(r), end(r), __stl2::ref(pred),
					__stl2::ref(proj));
			}
		}
	};

	inline constexpr __partition_point_fn partition_point {};
} STL2_CLOSE_NAMESPACE

#endif
