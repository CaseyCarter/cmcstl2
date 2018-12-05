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
#ifndef STL2_DETAIL_ALGORITHM_INCLUDES_HPP
#define STL2_DETAIL_ALGORITHM_INCLUDES_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// includes [includes]
//
STL2_OPEN_NAMESPACE {
	template<InputIterator I1, Sentinel<I1> S1,
		InputIterator I2, Sentinel<I2> S2, class Comp = less,
		class Proj1 = identity, class Proj2 = identity>
	requires
		IndirectStrictWeakOrder<
			Comp, projected<I1, Proj1>, projected<I2, Proj2>>
	bool includes(I1 first1, S1 last1, I2 first2, S2 last2, Comp comp = Comp{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		while (true) {
			if (first2 == last2) {
				return true;
			}
			if (first1 == last1) {
				return false;
			}
			if (__stl2::invoke(comp, __stl2::invoke(proj2, *first2), __stl2::invoke(proj1, *first1))) {
				return false;
			}
			if (!__stl2::invoke(comp, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2))) {
				++first2;
			}
			++first1;
		}
	}

	template<InputRange Rng1, InputRange Rng2, class Comp = less,
		class Proj1 = identity, class Proj2 = identity>
	requires
		IndirectStrictWeakOrder<Comp,
			projected<iterator_t<Rng1>, Proj1>,
			projected<iterator_t<Rng2>, Proj2>>
	bool includes(Rng1&& rng1, Rng2&& rng2, Comp comp = Comp{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __stl2::includes(
			__stl2::begin(rng1), __stl2::end(rng1),
			__stl2::begin(rng2), __stl2::end(rng2),
			std::ref(comp),
			std::ref(proj1),
			std::ref(proj2));
	}
} STL2_CLOSE_NAMESPACE

#endif
