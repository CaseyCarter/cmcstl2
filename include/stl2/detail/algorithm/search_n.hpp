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
//
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
#ifndef STL2_DETAIL_ALGORITHM_SEARCH_N_HPP
#define STL2_DETAIL_ALGORITHM_SEARCH_N_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>

///////////////////////////////////////////////////////////////////////////
// search_n [alg.search]
//
STL2_OPEN_NAMESPACE {
	class __search_n_fn {
		template <class I, class S, class T, class Pred, class Proj>
		static constexpr subrange<I> unsized(I first, S last, iter_difference_t<I> count,
			const T& value, Pred pred, Proj proj)
		{
			if (count <= 0) {
				return {first, first};
			}

			for (; first != last; ++first) {
				if (__stl2::invoke(pred, __stl2::invoke(proj, *first), value)) {
					auto saved = first;
					iter_difference_t<I> n = 0;
					do {
						if (++n == count) {
							return {saved, __stl2::next(saved, count)};
						}
						if (++first == last) {
							return {first, first};
						}
					} while(__stl2::invoke(pred, __stl2::invoke(proj, *first), value));
				}
			}

			return {first, first};
		}

		template <class I, class S, class T, class Pred, class Proj>
		static constexpr subrange<I> sized(I first_, S last, iter_difference_t<I> d_,
			iter_difference_t<I> count,
			const T& value, Pred pred, Proj proj)
		{
			if (count <= 0) {
				return {first_, first_};
			}

			auto d = d_;
			auto first = ext::uncounted(first_);

			for (; d >= count; ++first, --d) {
				if (__stl2::invoke(pred, __stl2::invoke(proj, *first), value)) {
					// *first matches val, now match elements after here
					auto saved = first;
					iter_difference_t<I> n = 0;
					do {
						if (++n == count) {
							// Pattern exhausted, saved is the answer
							// (works for 1 element pattern)
							return {saved, __stl2::next(saved, count)};
						}
					} while (__stl2::invoke(pred, __stl2::invoke(proj, *++first), value));
					d -= n;
				}
			}

			auto result = __stl2::next(first, std::move(last));
			return {result, result};
		}

		template <class I, class S, class T, class Pred, class Proj>
		static constexpr subrange<I> impl(I first, S last, iter_difference_t<I> count,
			const T& value, Pred pred, Proj proj)
		{
			return __search_n_fn::unsized(std::move(first), std::move(last), count, value,
				std::ref(pred), std::ref(proj));
		}

		template <class I, SizedSentinel<I> S, class T, class Pred, class Proj>
		static constexpr subrange<I> impl(I first, S last, iter_difference_t<I> count,
			const T& value, Pred pred, Proj proj)
		{
			auto n = __stl2::distance(first, last);
			return __search_n_fn::sized(std::move(first), std::move(last), n, count, value,
				std::ref(pred), std::ref(proj));
		}

		template <class R, class T, class Pred, class Proj>
		static constexpr safe_subrange_t<R> impl(R&& r, iter_difference_t<iterator_t<R>> count,
			const T& value, Pred pred, Proj proj)
		{
			return __search_n_fn::unsized(
				__stl2::begin(r), __stl2::end(r), count, value, std::ref(pred), std::ref(proj));
		}

		template <SizedRange R, class T, class Pred, class Proj>
		static constexpr safe_subrange_t<R> impl(R&& r, iter_difference_t<iterator_t<R>> count,
			const T& value, Pred pred, Proj proj)
		{
			return __search_n_fn::sized(
				__stl2::begin(r), __stl2::end(r), __stl2::distance(r), count, value,
				std::ref(pred), std::ref(proj));
		}
	public:
		template<ForwardIterator I, Sentinel<I> S, class T, class Pred = ranges::equal_to<>,
			class Proj = identity>
		requires IndirectlyComparable<I, const T*, Pred, Proj>
		constexpr subrange<I> operator()(I first, S last, iter_difference_t<I> count,
			const T& value, Pred pred = Pred{}, Proj proj = Proj{}) const
		{
			return __search_n_fn::impl(std::move(first), std::move(last), count, value, std::ref(pred),
				std::ref(proj));
		}

		template<ForwardRange R, class T, class Pred = ranges::equal_to<>, class Proj = identity>
		requires IndirectlyComparable<iterator_t<R>, const T*, Pred, Proj>
		constexpr safe_subrange_t<R> operator()(R&& r, iter_difference_t<iterator_t<R>> count,
			const T& value, Pred pred = Pred{}, Proj proj = Proj{}) const
		{
			return __search_n_fn::impl(static_cast<R&&>(r), count, value, std::ref(pred),
				std::ref(proj));
		}
	};

	inline constexpr __search_n_fn search_n {};
} STL2_CLOSE_NAMESPACE

#endif
