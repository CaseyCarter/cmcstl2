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

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// search_n [alg.search]
//
STL2_OPEN_NAMESPACE {
	struct __search_n_fn : private __niebloid {
		template<ForwardIterator I, Sentinel<I> S, class T,
			class Pred = equal_to, class Proj = identity>
		requires IndirectlyComparable<I, const T*, Pred, Proj>
		constexpr I operator()(I first, S last, iter_difference_t<I> count,
			const T& value, Pred pred = {}, Proj proj = {}) const
		{
			if constexpr (SizedSentinel<S, I>) {
				auto n = static_cast<iter_difference_t<I>>(last - first);
				return sized(std::move(first), std::move(last),
					n, count, value, __stl2::ref(pred), __stl2::ref(proj));
			} else {
				return unsized(std::move(first), std::move(last),
					count, value, __stl2::ref(pred), __stl2::ref(proj));
			}
		}

		template<ForwardRange R, class T, class Pred = equal_to,
			class Proj = identity>
		requires IndirectlyComparable<iterator_t<R>, const T*, Pred, Proj>
		constexpr safe_iterator_t<R>
		operator()(R&& r, iter_difference_t<iterator_t<R>> count,
			const T& value, Pred pred = {}, Proj proj = {}) const
		{
			if constexpr (SizedRange<R>) {
				return sized(begin(r), end(r), distance(r), count, value,
					__stl2::ref(pred), __stl2::ref(proj));
			} else {
				return unsized(begin(r), end(r), count, value,
					__stl2::ref(pred), __stl2::ref(proj));
			}
		}
	private:
		template<ForwardIterator I, Sentinel<I> S, class T, class Pred,
			class Proj>
		requires IndirectlyComparable<I, const T*, Pred, Proj>
		static constexpr I unsized(I first, S last, iter_difference_t<I> count,
			const T& value, Pred pred, Proj proj)
		{
			if (count <= 0) {
				return first;
			}

			for (; first != last; ++first) {
				if (__stl2::invoke(pred, __stl2::invoke(proj, *first), value)) {
					auto saved = first;
					iter_difference_t<I> n = 0;
					do {
						if (++n == count) {
							return saved;
						}
						if (++first == last) {
							return first;
						}
					} while(__stl2::invoke(pred, __stl2::invoke(proj, *first), value));
				}
			}

			return first;
		}

		template<ForwardIterator I, Sentinel<I> S, class T, class Pred, class Proj>
		requires IndirectlyComparable<I, const T*, Pred, Proj>
		static constexpr I sized(I first_, S last, iter_difference_t<I> d_,
			iter_difference_t<I> count, const T& value, Pred pred, Proj proj)
		{
			if (count <= 0) {
				return first_;
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
							return ext::recounted(first_, std::move(saved), d_ - d);
						}
					} while (__stl2::invoke(pred, __stl2::invoke(proj, *++first), value));
					d -= n;
				}
			}

			return next(ext::recounted(first_, std::move(first), d_ - d),
				std::move(last));
		}
	};

	inline constexpr __search_n_fn search_n {};
} STL2_CLOSE_NAMESPACE

#endif
