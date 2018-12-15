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
	template<class T>
	using __uncvref = std::remove_cv_t<std::remove_reference_t<T>>;

	template<bool IsConst, class T>
	using __maybe_const = std::conditional_t<IsConst, T const, T>;

	template<class From, class To>
	inline constexpr bool _IsConvertibleImpl =
#if defined(__clang__)
		META_CONCEPT_BARRIER(__is_convertible(From, To));
#elif defined(_MSC_VER)
		META_CONCEPT_BARRIER(__is_convertible_to(From, To));
#else
		META_CONCEPT_BARRIER(std::is_convertible_v<From, To>);
#endif

	template<class From, class To>
	inline constexpr bool is_nothrow_convertible_v = false;

	template<class T>
	void __nothrow_convertible_helper(T) noexcept; // not defined

	template<class From, class To>
	requires _IsConvertibleImpl<From, To>
	inline constexpr bool is_nothrow_convertible_v<From, To> =
		noexcept(__nothrow_convertible_helper<To>(std::declval<From>()));

	template<class T, class D = std::decay_t<T>>
	requires _IsConvertibleImpl<T, D>
	D __decay_copy(T&& t)
	noexcept(is_nothrow_convertible_v<T, D>)
#if defined(__GNUC__) && !defined(__clang__)
	{
		return static_cast<T&&>(t);
	}
#else
	; // not defined
#endif
} STL2_CLOSE_NAMESPACE

#endif
