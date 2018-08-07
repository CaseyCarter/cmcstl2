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
#ifndef STL2_DETAIL_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP
#define STL2_DETAIL_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// lexicographical_compare [alg.lex.comparison]
//
STL2_OPEN_NAMESPACE {
	inline constexpr struct __lexicographical_compare_fn
	{
		template <InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
			class Comp = less<>, class Proj1 = identity, class Proj2 = identity>
		requires
			IndirectStrictWeakOrder<Comp,
				projected<I1, Proj1>, projected<I2, Proj2>>
		constexpr bool operator()(I1 first1, S1 last1, I2 first2, S2 last2,
			Comp comp = Comp{}, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
				if (__stl2::invoke(comp, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2))) {
					return true;
				}
				if (__stl2::invoke(comp, __stl2::invoke(proj2, *first2), __stl2::invoke(proj1, *first1))) {
					return false;
				}
			}
			return first1 == last1 && first2 != last2;
		}

		template <InputRange Rng1, InputRange Rng2, class Comp = less<>,
			class Proj1 = identity, class Proj2 = identity>
		requires
			IndirectStrictWeakOrder<Comp,
				projected<iterator_t<Rng1>, Proj1>,
				projected<iterator_t<Rng2>, Proj2>>
		constexpr bool operator()(Rng1&& rng1, Rng2&& rng2,
			Comp comp = Comp{}, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			return (*this)(
				__stl2::begin(rng1), __stl2::end(rng1),
				__stl2::begin(rng2), __stl2::end(rng2),
				std::ref(comp),
				std::ref(proj1),
				std::ref(proj2));
		}
	} lexicographical_compare {};
} STL2_CLOSE_NAMESPACE

#endif
