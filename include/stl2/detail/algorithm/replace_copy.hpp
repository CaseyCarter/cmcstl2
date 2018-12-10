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
#ifndef STL2_DETAIL_ALGORITHM_REPLACE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_REPLACE_COPY_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// replace_copy [alg.replace]
//
STL2_OPEN_NAMESPACE {
	template<InputIterator I, Sentinel<I> S, class T1, class T2,
		OutputIterator<const T2&> O, class Proj = identity>
	requires
		IndirectlyCopyable<I, O> &&
		IndirectRelation<
			equal_to, projected<I, Proj>, const T1*>
	tagged_pair<tag::in(I), tag::out(O)>
	replace_copy(I first, S last, O result, const T1& old_value,
		const T2& new_value, Proj proj = {})
	{
		for (; first != last; ++first, ++result) {
			iter_reference_t<I>&& v = *first;
			if (__stl2::invoke(proj, v) == old_value) {
				*result = new_value;
			} else {
				*result = std::forward<iter_reference_t<I>>(v);
			}
		}
		return {std::move(first), std::move(result)};
	}

	template<InputRange Rng, class T1, class T2, class O, class Proj = identity>
	requires
		OutputIterator<__f<O>, const T2&> &&
		IndirectlyCopyable<iterator_t<Rng>, __f<O>> &&
		IndirectRelation<
			equal_to, projected<iterator_t<Rng>, Proj>, const T1*>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
	replace_copy(Rng&& rng, O&& result, const T1& old_value,
		const T2& new_value, Proj proj = {})
	{
		return __stl2::replace_copy(
			begin(rng), end(rng), std::forward<O>(result),
			old_value, new_value, std::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
