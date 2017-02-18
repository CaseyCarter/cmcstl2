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
	namespace __search_n {
		template <ForwardIterator I, Sentinel<I> S, class T, class Pred, class Proj>
		requires
			IndirectlyComparable<I, const T*, Pred, Proj>()
		I unsized(I first, S last, difference_type_t<I> count,
			const T& value, Pred pred, Proj proj)
		{
			if (count <= 0) {
				return first;
			}

			for (; first != last; ++first) {
				if (__stl2::invoke(pred, __stl2::invoke(proj, *first), value)) {
					auto saved = first;
					difference_type_t<I> n = 0;
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

		template <ForwardIterator I, Sentinel<I> S, class T, class Pred, class Proj>
		requires
			IndirectlyComparable<I, const T*, Pred, Proj>()
		I sized(I first_, S last, difference_type_t<I> d_,
			difference_type_t<I> count,
			const T& value, Pred pred, Proj proj)
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
					difference_type_t<I> n = 0;
					do {
						if (++n == count) {
							// Pattern exhausted, saved is the answer
							// (works for 1 element pattern)
							return ext::recounted(first_, __stl2::move(saved), d_ - d);
						}
					} while (__stl2::invoke(pred, __stl2::invoke(proj, *++first), value));
					d -= n;
				}
			}

			return __stl2::next(ext::recounted(first_, __stl2::move(first), d_ - d),
			__stl2::move(last));
		}
	}

	template <ForwardIterator I, Sentinel<I> S, class T,
		class Pred = equal_to<>, class Proj = identity>
	requires
		IndirectlyComparable<I, const T*, Pred, Proj>()
	I search_n(I first, S last, difference_type_t<I> count,
						 const T& value, Pred pred = Pred{}, Proj proj = Proj{})
	{
		return __search_n::unsized(__stl2::move(first), __stl2::move(last),
			count, value, __stl2::ref(pred), __stl2::ref(proj));
	}

	template <ForwardIterator I, Sentinel<I> S, class T,
		class Pred = equal_to<>, class Proj = identity>
	requires
		SizedSentinel<S, I>() &&
		IndirectlyComparable<I, const T*, Pred, Proj>()
	I search_n(I first, S last, difference_type_t<I> count,
						 const T& value, Pred pred = Pred{}, Proj proj = Proj{})
	{
		auto n = __stl2::distance(first, last);
		return __search_n::sized(__stl2::move(first), __stl2::move(last),
			n, count, value, __stl2::ref(pred), __stl2::ref(proj));
	}

	template <ForwardRange Rng, class T, class Pred = equal_to<>, class Proj = identity>
	requires
		IndirectlyComparable<
			iterator_t<Rng>, const T*, Pred, Proj>()
	safe_iterator_t<Rng>
	search_n(Rng&& rng, difference_type_t<iterator_t<Rng>> count,
		const T& value, Pred pred = Pred{}, Proj proj = Proj{})
	{
		return __search_n::unsized(
			__stl2::begin(rng), __stl2::end(rng), count, value,
			__stl2::ref(pred), __stl2::ref(proj));
	}

	template <ForwardRange Rng, class T, class Pred = equal_to<>, class Proj = identity>
	requires
		SizedRange<Rng>() &&
		IndirectlyComparable<
			iterator_t<Rng>, const T*, Pred, Proj>()
	safe_iterator_t<Rng>
	search_n(Rng&& rng, difference_type_t<iterator_t<Rng>> count,
		const T& value, Pred pred = Pred{}, Proj proj = Proj{})
	{
		return __search_n::sized(
			__stl2::begin(rng), __stl2::end(rng),
			__stl2::distance(rng), count, value,
			__stl2::ref(pred), __stl2::ref(proj));
	}

	template <class E, class T, class Pred = equal_to<>, class Proj = identity>
	requires
		IndirectlyComparable<
			const E*, const T*, Pred, Proj>()
	dangling<const E*>
	search_n(std::initializer_list<E>&& rng, std::ptrdiff_t count,
		const T& value, Pred pred = Pred{}, Proj proj = Proj{})
	{
		return __search_n::sized(
			__stl2::begin(rng), __stl2::end(rng),
			__stl2::distance(rng), count, value,
			__stl2::ref(pred), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
