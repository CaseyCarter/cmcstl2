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
#ifndef STL2_DETAIL_FUNCTIONAL_ARITHMETIC_NEGATE_HPP
#define STL2_DETAIL_FUNCTIONAL_ARITHMETIC_NEGATE_HPP

#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/functional/arithmetic/plus.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/numeric_traits/identity.hpp>
#include <type_traits>
#include <utility>

STL2_OPEN_NAMESPACE {
	template<class T>
	META_CONCEPT __negatable = requires(T&& t) {
		{ -std::forward<T>(t) } -> Common<T>;
	};

	struct negate {
		template<__negatable T>
		constexpr decltype(auto) operator()(T&& t) const {
			return -std::forward<T>(t);
		}

		using is_transparent = std::true_type;
	};
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_FUNCTIONAL_ARITHMETIC_NEGATE_HPP
