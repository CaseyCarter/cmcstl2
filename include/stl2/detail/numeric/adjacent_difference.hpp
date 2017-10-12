// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2013 -- 2017 (range-v3/include/numeric/adjacent_difference.hpp)
//  Copyright Casey Carter 2015 -- 2017
//  Copyright Christopher Di Bella 2016 -- 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_NUMERIC_ADJACENT_DIFFERENCE_HPP
#define STL2_DETAIL_NUMERIC_ADJACENT_DIFFERENCE_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object/movable.hpp>
#include <stl2/iterator.hpp>

STL2_OPEN_NAMESPACE {
	template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O,
		class Proj = identity,
		ext::IndirectRegularInvocable<projected<I, Proj>, projected<I, Proj>> Op = minus<>>
	requires
		IndirectlyCopyable<projected<I, Proj>, O> &&
		Writable<O, value_type_t<projected<I, Proj>>> &&
		Movable<value_type_t<projected<I, Proj>>> &&
		CopyConstructible<value_type_t<projected<I, Proj>>>
	tagged_pair<tag::in(I), tag::out(O)>
	adjacent_difference(I first, S last, O result, Op op = Op{}, Proj proj = Proj{})
	{
		if (first != last) {
			auto acc = std::invoke(proj, *first++);
			*result++ = acc;
			for (; first != last; ++first, (void)++result) {
				auto val = std::invoke(proj, *first);
				*result = std::invoke(op, val, acc);
				acc = std::move(val);
			}
		}
		return {std::move(first), std::move(result)};
	}

	template <InputRange Rng, WeaklyIncrementable O,
		class Proj = identity,
		ext::IndirectRegularInvocable<
			projected<iterator_t<Rng>, Proj>,
			projected<iterator_t<Rng>, Proj>> Op = minus<>>
	requires
		IndirectlyCopyable<projected<iterator_t<Rng>, Proj>, O> &&
		Writable<O, value_type_t<projected<iterator_t<Rng>, Proj>>> &&
		Movable<value_type_t<projected<iterator_t<Rng>, Proj>>> &&
		CopyConstructible<value_type_t<projected<iterator_t<Rng>, Proj>>>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
	adjacent_difference(Rng&& rng, O result, Op op = Op{}, Proj proj = Proj{})
	{
		return adjacent_difference(__stl2::begin(rng), __stl2::end(rng), std::move(result),
											std::ref(op), std::ref(proj));
	}
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_NUMERIC_ADJACENT_DIFFERENCE_HPP
