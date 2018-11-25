// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_CONCEPTS_CORE_HPP
#define STL2_DETAIL_CONCEPTS_CORE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>

///////////////////////////////////////////////////////////////////////////////
// Language-related Concepts [concepts.lang]
//
STL2_OPEN_NAMESPACE {
	template<template<class...> class T, class... U>
	STL2_CONCEPT _Valid = requires { typename T<U...>; };

#if __cpp_concepts <= 201507
	template<bool B>
	inline constexpr bool __bool = B;

	template<class U, template<class...> class T, class... V>
	STL2_CONCEPT _Is = _Valid<T, U, V...> && __bool<T<U, V...>::value>;

	template<class U, template<class...> class T, class... V>
	STL2_CONCEPT _IsNot = _Valid<T, U, V...> && __bool<!T<U, V...>::value>;

	// U is a cv/ref-qualified specialization of class template T.
	template<class U, template<class...> class T>
	STL2_CONCEPT _SpecializationOf = __bool<meta::is<__uncvref<U>, T>::value>;
#else
	template<class U, template<class...> class T, class... V>
	STL2_CONCEPT _Is = _Valid<T, U, V...> && T<U, V...>::value;

	template<class U, template<class...> class T, class... V>
	STL2_CONCEPT _IsNot = _Valid<T, U, V...> && !T<U, V...>::value;

	// U is a cv/ref-qualified specialization of class template T.
	template<class U, template<class...> class T>
	STL2_CONCEPT _SpecializationOf = meta::is<__uncvref<U>, T>::value;
#endif

	///////////////////////////////////////////////////////////////////////////
	// Same
	//
#if defined(__GNUC__)
	template<class T, class U>
	STL2_CONCEPT _SameImpl = __is_same_as(T, U);
#else
	template<class T, class U>
	STL2_CONCEPT _SameImpl = std::is_same_v<T, U>;
#endif
	template<class T, class U>
	STL2_CONCEPT Same = _SameImpl<T, U> && _SameImpl<U, T>;

	template<class T>
	STL2_CONCEPT _Decayed = Same<T, std::decay_t<T>>;

	template<class T, class... Args>
	STL2_CONCEPT _OneOf = (Same<T, Args> || ...);

	template<class T, class U>
	STL2_CONCEPT _NotSameAs = !_SameImpl<__uncvref<T>, __uncvref<U>>;

	///////////////////////////////////////////////////////////////////////////
	// DerivedFrom
	//
	template<class T, class U>
	STL2_CONCEPT DerivedFrom =
#if defined(__GNUC__)
		__is_base_of(U, T) &&
#else
		std::is_base_of_v<U, T> &&
#endif
			std::is_convertible_v<const volatile T*, const volatile U*>;

	///////////////////////////////////////////////////////////////////////////
	// ConvertibleTo
	//
	template<class From, class To>
	STL2_CONCEPT ConvertibleTo =
		std::is_convertible_v<From, To> && requires(From (&f)()) {
			static_cast<To>(f());
		};
		// Axiom: implicit and explicit conversion have equal results.
} STL2_CLOSE_NAMESPACE

#endif
