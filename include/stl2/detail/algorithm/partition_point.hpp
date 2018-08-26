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

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// partition_point [alg.partitions]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		struct __partition_point_n_fn {
			template <ForwardIterator I, class Proj = identity, IndirectUnaryPredicate<projected<I, Proj>> Pred>
			constexpr I operator()(I first, iter_difference_t<I> n, Pred pred, Proj proj = Proj{}) const
			{
				STL2_EXPECT(0 <= n);
				while (n != 0) {
					auto const half = n / 2;
					auto middle = __stl2::next(__stl2::ext::uncounted(first), half);
					if (__stl2::invoke(pred, __stl2::invoke(proj, *middle))) {
						first = __stl2::ext::recounted(
											first, std::move(++middle), half + 1);
						n -= half + 1;
					} else {
						n = half;
					}
				}
				return first;
			}
		};

		inline constexpr __partition_point_n_fn partition_point_n {};
	} // namespace ext

	class __partition_point_fn {
		template<ForwardIterator I, Sentinel<I> S, class Proj, class Pred>
		static constexpr I impl(I first, S last, Pred pred, Proj proj = Proj{})
		{
			// Probe exponentially for either end-of-range or an iterator
			// that is past the partition point (i.e., does not satisfy pred).
			auto n = iter_difference_t<I>{1};
			while (true) {
				auto m = first;
				auto d = __stl2::advance(m, n, last);
				if (m == last || !__stl2::invoke(pred, __stl2::invoke(proj, *m))) {
					n -= d;
					return __stl2::ext::partition_point_n(std::move(first), n,
						std::move(pred), std::move(proj));
				}
				first = std::move(m);
				n *= 2;
			}
		}

		template<ForwardIterator I, SizedSentinel<I> S, class Proj, class Pred>
		static constexpr I impl(I first, S last, Pred pred, Proj proj = Proj{})
		{
			auto n = __stl2::distance(first, std::move(last));
			return __stl2::ext::partition_point_n(std::move(first), n, std::ref(pred), std::ref(proj));
		}

		template<ForwardRange R, class Proj, class Pred>
		static constexpr safe_iterator_t<R> impl(R&& r, Pred pred, Proj proj = Proj{})
		{
			return __stl2::__partition_point_fn::impl(__stl2::begin(r), __stl2::end(r),
				std::ref(pred), std::ref(proj));
		}

		template<SizedRange R, class Proj, class Pred>
		static constexpr safe_iterator_t<R> impl(R&& r, Pred pred, Proj proj = Proj{})
		{
			return __stl2::ext::partition_point_n(__stl2::begin(r), __stl2::distance(r),
				std::ref(pred), std::ref(proj));
		}
	public:
		template<ForwardIterator I, Sentinel<I> S, class Proj = identity,
			IndirectUnaryPredicate<projected<I, Proj>> Pred>
		constexpr I operator()(I first, S last, Pred pred, Proj proj = Proj{}) const
		{
			return __partition_point_fn::impl(std::move(first), std::move(last),
				std::ref(pred), std::ref(proj));
		}

		template<ForwardRange R, class Proj = identity,
			IndirectUnaryPredicate<projected<iterator_t<R>, Proj>> Pred>
		constexpr safe_iterator_t<R> operator()(R&& r, Pred pred, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(r), __stl2::end(r), std::ref(pred), std::ref(proj));
		}
	};

	inline constexpr __partition_point_fn partition_point {};
} STL2_CLOSE_NAMESPACE

#endif
