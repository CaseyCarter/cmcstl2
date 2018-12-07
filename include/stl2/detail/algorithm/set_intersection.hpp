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
#ifndef STL2_DETAIL_ALGORITHM_SET_INTERSECTION_HPP
#define STL2_DETAIL_ALGORITHM_SET_INTERSECTION_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// set_intersection [set.intersection]
//
STL2_OPEN_NAMESPACE {
	template<InputIterator I1, Sentinel<I1> S1,
		InputIterator I2, Sentinel<I2> S2,
		WeaklyIncrementable O, class Comp = less,
		class Proj1 = identity, class Proj2 = identity>
	requires
		Mergeable<I1, I2, O, Comp, Proj1, Proj2>
	O set_intersection(I1 first1, S1 last1, I2 first2, S2 last2, O result,
		Comp comp = {}, Proj1 proj1 = {},
		Proj2 proj2 = {})
	{
		while (first1 != last1 && first2 != last2) {
			iter_reference_t<I1>&& v1 = *first1;
			iter_reference_t<I2>&& v2 = *first2;
			auto&& p1 = __stl2::invoke(proj1, v1);
			auto&& p2 = __stl2::invoke(proj2, v2);
			if (__stl2::invoke(comp, p1, p2)) {
				++first1;
			} else if (__stl2::invoke(comp, p2, p1)) {
				++first2;
			} else {
				*result = std::forward<iter_reference_t<I1>>(v1);
				++result;
				++first1;
				++first2;
			}
		}
		return result;
	}

	template<InputRange Rng1, InputRange Rng2, class O, class Comp = less,
		class Proj1 = identity, class Proj2 = identity>
	requires
		WeaklyIncrementable<__f<O>> &&
		Mergeable<
			iterator_t<Rng1>, iterator_t<Rng2>, __f<O>,
			Comp, Proj1, Proj2>
	__f<O>
	set_intersection(Rng1&& rng1, Rng2&& rng2, O&& result, Comp comp = {},
		Proj1 proj1 = {}, Proj2 proj2 = {})
	{
		return __stl2::set_intersection(__stl2::begin(rng1), __stl2::end(rng1),
			__stl2::begin(rng2), __stl2::end(rng2), std::forward<O>(result),
			std::ref(comp), std::ref(proj1), std::ref(proj2));
	}
} STL2_CLOSE_NAMESPACE

#endif
