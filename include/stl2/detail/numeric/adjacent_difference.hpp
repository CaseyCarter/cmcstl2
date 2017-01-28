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
				 class Op = minus<>,
				 class __Arg = projected<I, Proj>>
	requires
		models::IndirectlyCopyable<__Arg, O> &&
		models::Writable<O, value_type_t<__Arg>> &&
		models::Movable<value_type_t<__Arg>> &&
		models::CopyConstructible<value_type_t<__Arg>> &&
		models::IndirectRegularInvocable<Op, __Arg, __Arg>
	tagged_pair<tag::in(I), tag::out(O)>
	adjacent_difference(I first, S last, O result, Op op = Op{}, Proj proj = Proj{})
	{
		if (first != last) {
			auto acc = __stl2::invoke(proj, *first++);
			*result++ = acc;
			for (; first != last; ++first, (void)++result) {
				auto val = __stl2::invoke(proj, *first);
				*result = __stl2::invoke(op, val, acc);
				acc = __stl2::move(val);
			}
		}

		return {__stl2::move(first), __stl2::move(result)};
	}

	template <InputRange Rng, WeaklyIncrementable O,
				 class Proj = identity,
				 class Op = minus<>,
				 class __Arg = projected<iterator_t<Rng>, Proj>>
	requires
		models::IndirectlyCopyable<__Arg, O> &&
		models::Writable<O, value_type_t<__Arg>> &&
		models::Movable<value_type_t<__Arg>> &&
		models::CopyConstructible<value_type_t<__Arg>> &&
		models::IndirectRegularInvocable<Op, __Arg, __Arg>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
	adjacent_difference(Rng&& rng, O result, Op op = Op{}, Proj proj = Proj{})
	{
		return adjacent_difference(__stl2::begin(rng), __stl2::end(rng), __stl2::move(result),
											__stl2::ref(op), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_NUMERIC_ADJACENT_DIFFERENCE_HPP
