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
#ifndef STL2_DETAIL_ALGORITHM_PARTITION_COPY_HPP
#define STL2_DETAIL_ALGORITHM_PARTITION_COPY_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/tuple.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// partition_copy [alg.partitions]
//
STL2_OPEN_NAMESPACE {
	template<InputIterator I, Sentinel<I> S, WeaklyIncrementable O1,
		WeaklyIncrementable O2, class Pred, class Proj = identity>
	requires
		IndirectlyCopyable<I, O1> &&
		IndirectlyCopyable<I, O2> &&
		IndirectUnaryPredicate<
			Pred, projected<I, Proj>>
	tagged_tuple<tag::in(I), tag::out1(O1), tag::out2(O2)>
	partition_copy(I first, S last, O1 out_true, O2 out_false, Pred pred,
		Proj proj = {})
	{
		for (; first != last; ++first) {
			iter_reference_t<I>&& v = *first;
			if (__stl2::invoke(pred, __stl2::invoke(proj, v))) {
				*out_true  = std::forward<iter_reference_t<I>>(v);
				++out_true;
			} else {
				*out_false = std::forward<iter_reference_t<I>>(v);
				++out_false;
			}
		}
		return {std::move(first),
			std::move(out_true), std::move(out_false)};
	}

	template<InputRange Rng, class O1, class O2, class Pred, class Proj = identity>
	requires
		WeaklyIncrementable<__f<O1>> &&
		WeaklyIncrementable<__f<O2>> &&
		IndirectlyCopyable<iterator_t<Rng>, __f<O1>> &&
		IndirectlyCopyable<iterator_t<Rng>, __f<O2>> &&
		IndirectUnaryPredicate<
			Pred, projected<iterator_t<Rng>, Proj>>
	tagged_tuple<
		tag::in(safe_iterator_t<Rng>),
		tag::out1(__f<O1>),
		tag::out2(__f<O2>)>
	partition_copy(Rng&& rng, O1&& out_true, O2&& out_false, Pred pred,
		Proj proj = {})
	{
		return __stl2::partition_copy(
			begin(rng), end(rng),
			std::forward<O1>(out_true), std::forward<O2>(out_false),
			std::ref(pred), std::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
