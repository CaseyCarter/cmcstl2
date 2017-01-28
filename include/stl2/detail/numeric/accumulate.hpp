// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2013 -- 2017 (range-v3/include/numeric/accumulate.hpp)
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
#ifndef STL2_DETAIL_NUMERIC_ACCUMULATE_HPP
#define STL2_DETAIL_NUMERIC_ACCUMULATE_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object/semiregular.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/iterator.hpp>

STL2_OPEN_NAMESPACE {
	template <InputIterator I, Sentinel<I> S,
				 Copyable T = value_type_t<I>,
				 class Proj = identity,
				 class BOp = plus<>,
				 class __Arg = value_type_t<projected<I, Proj>>>
	requires
		models::RegularInvocable<BOp, T, __Arg> &&
		models::Assignable<T&, result_of_t<BOp&(T, __Arg)>>
	T accumulate(I first, S last, T init = T{}, BOp bop = BOp{}, Proj proj = Proj{})
	{
		T acc = init;
		for (; first != last; ++first)
			acc = __stl2::invoke(bop, acc, __stl2::invoke(proj, *first));
		return acc;
	}

	template <InputRange Rng, Copyable T = value_type_t<iterator_t<Rng>>,
				 class Proj = identity,
				 class BOp = plus<>,
				 class __Arg = value_type_t<projected<iterator_t<Rng>, Proj>>>
	requires
		models::RegularInvocable<BOp, T, __Arg> &&
		models::Assignable<T&, result_of_t<BOp&(T, __Arg)>>
	T accumulate(Rng&& rng, T init = T{}, BOp bop = BOp{}, Proj proj = Proj{})
	{
		return accumulate(__stl2::begin(rng), __stl2::end(rng),
								__stl2::move(init), __stl2::ref(bop), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_ACCUMULATE_HPP
