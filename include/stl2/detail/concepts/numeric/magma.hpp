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
#ifndef STL2_DETAIL_CONCEPTS_NUMERIC_MAGMA_HPP
#define STL2_DETAIL_CONCEPTS_NUMERIC_MAGMA_HPP

#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<class BOp, class T, class U>
		META_CONCEPT Magma =
			Common<T, U> &&
			RegularInvocable<BOp, T, T> &&
			RegularInvocable<BOp, U, U> &&
			RegularInvocable<BOp, T, U> &&
			RegularInvocable<BOp, U, T> &&
			Common<invoke_result_t<BOp&, T, U>, T> &&
			Common<invoke_result_t<BOp&, T, U>, U> &&
			Same<invoke_result_t<BOp&, T, U>, invoke_result_t<BOp&, U, T>>;
		// axiom: Let `bop` be an object of type `BOp`, `t` be an object of type `T`, and `u` be an
		//        object of type `U`. The expression `invoke(bop, t, u)` must return a result that is
		//        representable by `common_type_t<T, U>`.
		// axiom: The result of `invoke(bop, t, u)` needn't be the same as `invoke(bop, u, t)`.
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_CONCEPTS_NUMERIC_MAGMA_HPP
