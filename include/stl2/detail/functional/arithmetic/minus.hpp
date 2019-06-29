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
#ifndef STL2_DETAIL_FUNCTIONAL_ARITHMETIC_MINUS_HPP
#define STL2_DETAIL_FUNCTIONAL_ARITHMETIC_MINUS_HPP

#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/functional/arithmetic/plus.hpp>
#include <stl2/detail/functional/arithmetic/negate.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/numeric_traits/identity.hpp>
#include <type_traits>
#include <utility>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<class T, class U>
		META_CONCEPT __differenceable_with =
			__summable_with<T, U> &&
			__negatable<T> &&
			__negatable<U> &&
			requires(__uncvref<T> t, __uncvref<U> u) {
				{ t -= std::forward<U>(u) } -> CommonReference<T&>;
				{ u -= std::forward<T>(t) } -> CommonReference<U&>;
				{ std::forward<T>(t) - std::forward<T>(t) } -> Common<T>;
				{ std::forward<U>(u) - std::forward<U>(u) } -> Common<U>;
				{ std::forward<T>(t) - std::forward<U>(u) } -> Common<T>;
				{ std::forward<T>(t) - std::forward<U>(u) } -> Common<U>;
				requires Same<decltype(std::forward<T>(t) - std::forward<U>(u)),
				              decltype(std::forward<U>(t) - std::forward<T>(u))>;
			};
		// axiom: `t - t` is equivalent to `T{}`
		// axiom: `t - (-t)` is equivalent to `t + t`
		// axiom: `-t - t` is equivalent to `-(t + t)`
		// axiom: `t + t - t` is equivalent to `t`

		struct minus {
			template<class T, __differenceable_with<T> U>
			constexpr decltype(auto) operator()(T&& t, U&& u) const {
				return std::forward<T>(t) - std::forward<U>(u);
			}

			using is_transparent = std::true_type;
		};

		template<class T, class U>
		requires Magma<__stl2::ext::minus, T, U>
		struct right_identity<__stl2::ext::minus, T, U>
		: private right_identity<__stl2::ext::plus, T, U> {
			using right_identity<__stl2::ext::plus, T, U>::value;
		};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_FUNCTIONAL_ARITHMETIC_MINUS_HPP
