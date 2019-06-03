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
#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
	template<bool> struct __cond_ {
		template<class T, class> using invoke = T;
	};
	template<> struct __cond_<false> {
		template<class, class T> using invoke = T;
	};
	template<bool B, class T, class U> using __cond =
		typename __cond_<B>::template invoke<T, U>;

	template<class T>
	using __uncvref = std::remove_cv_t<std::remove_reference_t<T>>;

	template<bool IsConst, class T>
	using __maybe_const = __cond<IsConst, const T, T>;

	template<class From, class To>
	inline constexpr bool _IsConvertibleImpl =
#if defined(__clang__)
		__is_convertible(From, To);
#elif defined(_MSC_VER)
		__is_convertible_to(From, To);
#else
		std::is_convertible_v<From, To>;
#endif

	template<class From, class To>
	inline constexpr bool is_nothrow_convertible_v =
		std::is_void_v<From> && std::is_void_v<To>;

	template<class T>
	void __nothrow_convertible_helper(T) noexcept; // not defined

	template<class From, class To>
	requires requires(From&& f) {
#if STL2_BROKEN_COMPOUND_REQUIREMENT
		__nothrow_convertible_helper<To>(static_cast<From&&>(f));
		requires noexcept(__nothrow_convertible_helper<To>(static_cast<From&&>(f)));
#else
		{ __nothrow_convertible_helper<To>(static_cast<From&&>(f)) } noexcept;
#endif // STL2_BROKEN_COMPOUND_REQUIREMENT
	}
	inline constexpr bool is_nothrow_convertible_v<From, To> = true;

	template<class T, class D = std::decay_t<T>>
	requires _IsConvertibleImpl<T, D>
	D __decay_copy(T&& t)
	noexcept(is_nothrow_convertible_v<T, D>); // not defined
} STL2_CLOSE_NAMESPACE

#endif
