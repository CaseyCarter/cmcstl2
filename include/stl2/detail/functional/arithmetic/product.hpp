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
#ifndef STL2_DETAIL_FUNCTIONAL_ARITHMETIC_PRODUCT_HPP
#define STL2_DETAIL_FUNCTIONAL_ARITHMETIC_PRODUCT_HPP

#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/numeric_traits/identity.hpp>
#include <type_traits>
#include <utility>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<class T, class U>
		META_CONCEPT __multiplicable_with =
			DefaultConstructible<remove_reference_t<T>> &&
			DefaultConstructible<remove_reference_t<U>> &&
			Constructible<remove_reference_t<T>, int> && // specifically T{0} and T{1}
			Constructible<remove_reference_t<T>, int> && // specifically U{0} and U{1}
			CommonReference<T, U> &&
			requires(__uncvref<T> t, __uncvref<U> u) {
				{ t *= std::forward<U>(u) } -> CommonReference<T&>;
				{ u *= std::forward<T>(t) } -> CommonReference<U&>;
				{ std::forward<T>(t) * std::forward<T>(t) } -> Common<T>;
				{ std::forward<U>(u) * std::forward<U>(u) } -> Common<U>;
				{ std::forward<T>(t) * std::forward<U>(u) } -> Common<T>;
				{ std::forward<T>(t) * std::forward<U>(u) } -> Common<U>;
				requires Same<decltype(std::forward<T>(t) * std::forward<U>(u)),
					           decltype(std::forward<U>(t) * std::forward<T>(u))>;
			};
		// axiom: T{0} is equivalent to T{}, and similarly for U
		// axiom: `t * T{1} == t` and `T{1} * t == t`, and similarly for U

		// std::multiplies renamed to std::ranges::product
		struct product {
			template<class T, __multiplicable_with<T> U>
			constexpr decltype(auto) operator()(T&& t, U&& u) const {
				return std::forward<T>(t) * std::forward<U>(u);
			}

			using is_transparent = std::true_type;
		};

		template<class T, class U>
		struct left_identity<product, T, U> {
			constexpr explicit left_identity(const product&, const T&, const U&)
			requires Magma<product, T, U>
			{}

			constexpr auto value() const
			{ return T{1}; }
		};

		template<class T, class U>
		struct right_identity<product, T, U> {
			constexpr explicit right_identity(const product&, const T&, const U&)
			requires Magma<product, T, U>
			{}

			constexpr auto value() const
			{ return U{1}; }
		};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_FUNCTIONAL_ARITHMETIC_PRODUCT_HPP
