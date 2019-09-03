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

// Define some portable type_trait intrinsics
#if defined(__clang__)
#define STL2_IS_CONVERTIBLE(From, To) __is_convertible(From, To)
#elif defined(_MSC_VER)
#define STL2_IS_CONVERTIBLE(From, To) __is_convertible_to(From, To)
#else
#define STL2_IS_CONVERTIBLE(From, To) std::is_convertible_v<From, To>
#endif

#if defined(__clang__)
#define STL2_IS_VOID(T) __is_void(T)
#else
#define STL2_IS_VOID(T) std::is_void_v<T>
#endif

#if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
#define STL2_IS_BASE(Base, Derived) __is_base_of(Base, Derived)
#else
#define STL2_IS_BASE(Base, Derived) std::is_base_of_v<Base, Derived>
#endif

STL2_OPEN_NAMESPACE {
	template<class T>
	using __uncvref = std::remove_cv_t<std::remove_reference_t<T>>;

	template<bool IsConst, class T>
	using __maybe_const = std::conditional_t<IsConst, T const, T>;

	template<class From, class To>
	inline constexpr bool is_nothrow_convertible_v =
		STL2_IS_VOID(From) && STL2_IS_VOID(To);

	template<class T>
	void __nothrow_convertible_helper(T) noexcept; // not defined

	template<class From, class To>
	requires META_CONCEPT_BARRIER(STL2_IS_CONVERTIBLE(From, To))
	inline constexpr bool is_nothrow_convertible_v<From, To> =
		noexcept(__nothrow_convertible_helper<To>(std::declval<From>()));

	template<class T>
	T __decay_copy(T) noexcept; // not defined
} STL2_CLOSE_NAMESPACE

#endif
