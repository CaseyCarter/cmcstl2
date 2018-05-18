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
#ifndef STL2_DETAIL_META_HPP
#define STL2_DETAIL_META_HPP

#include <type_traits>
#include <stl2/meta/meta.hpp>
#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
	template <class T>
	using __uncvref = std::remove_cv_t<std::remove_reference_t<T>>;

	namespace detail {
		template <class>
		struct __as_integer_sequence {};
		template <class T, T... Is>
		struct __as_integer_sequence<meta::list<std::integral_constant<T, Is>...>> {
			using type = std::integer_sequence<T, Is...>;
		};
	}
	template <class T>
	using __as_integer_sequence = meta::_t<detail::__as_integer_sequence<T>>;

	template <bool IsConst, class T>
	using __maybe_const = meta::if_c<IsConst, T const, T>;
} STL2_CLOSE_NAMESPACE

#endif
