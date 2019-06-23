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
#ifndef STL2_DETAIL_FUNCTIONAL_ARITHMETIC_MODULUS_HPP
#define STL2_DETAIL_FUNCTIONAL_ARITHMETIC_MODULUS_HPP

#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/functional/arithmetic/plus.hpp>
#include <stl2/detail/functional/arithmetic/quotient.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/numeric_traits/identity.hpp>
#include <type_traits>
#include <utility>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<class T, class U>
		META_CONCEPT __modulo_with = __summable_with<T, U> && __divisible_with<T, U> &&
			requires(__uncvref<T> t, __uncvref<U> u) {
				{ t %= std::forward<U>(u) } -> CommonReference<T&>;
				{ u %= std::forward<T>(t) } -> CommonReference<U&>;
				{ std::forward<T>(t) % std::forward<T>(t) } -> Common<T>;
				{ std::forward<U>(u) % std::forward<U>(u) } -> Common<U>;
				{ std::forward<T>(t) % std::forward<U>(u) } -> Common<T>;
				{ std::forward<T>(t) % std::forward<U>(u) } -> Common<U>;
				requires Same<decltype(std::forward<T>(t) / std::forward<U>(u)),
				              decltype(std::forward<U>(t) / std::forward<T>(u))>;
			};
			// Let n, q, r, t all be distinct objects of type T.
			// axiom: `t % q == q * r + n`

		struct modulus {
			template<class T, __modulo_with<T> U>
			constexpr decltype(auto) operator()(T&& t, U&& u) const {
				return std::forward<T>(t) % std::forward<U>(u);
			}
		};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_FUNCTIONAL_ARITHMETIC_MODULUS_HPP
