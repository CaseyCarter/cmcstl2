// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_CONCEPTS_NUMERIC_SEMIGROUP_HPP
#define STL2_DETAIL_CONCEPTS_NUMERIC_SEMIGROUP_HPP

#include <stl2/detail/concepts/numeric/magma.hpp>
#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<class BOp, class T, class U>
		META_CONCEPT Semigroup = Magma<BOp, T, U>;
		// axiom: Let `bop` be an object of type `BOp`, `t` be an object of type `T`, and `u` be an
		//        object of type `U`. The expression `invoke(bop, t, invoke(bop, t, u))` is
		//        expression-equivalent to `invoke(bop, invoke(bop, t, u), u)`.
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_CONCEPTS_NUMERIC_SEMIGROUP_HPP
