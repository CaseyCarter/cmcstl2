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
	// U is a cv/ref-qualified specialization of class template T.
	template<class U, template<class...> class T>
	META_CONCEPT _SpecializationOf =
		META_CONCEPT_BARRIER(meta::is_v<__uncvref<U>, T>);

	template<class T>
	using __with_reference = T&;
	template<class T>
	META_CONCEPT __can_reference = requires { typename __with_reference<T>; };

	///////////////////////////////////////////////////////////////////////////
	// Same
	//
	template<class T, class U>
	META_CONCEPT Same = meta::Same<T, U> && meta::Same<U, T>;

	template<class T>
	META_CONCEPT _Decayed = meta::Same<T, std::decay_t<T>>;

	template<class T, class... Args>
	META_CONCEPT _OneOf = (meta::Same<T, Args> || ...);

	template<class T, class U>
	META_CONCEPT _NotSameAs = !meta::Same<__uncvref<T>, __uncvref<U>>;

	///////////////////////////////////////////////////////////////////////////
	// DerivedFrom
	//
	template<class Derived, class Base>
	META_CONCEPT DerivedFrom =
#if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
		META_CONCEPT_BARRIER(__is_base_of(Base, Derived)) &&
#else
		META_CONCEPT_BARRIER(std::is_base_of_v<Base, Derived>) &&
#endif
		_IsConvertibleImpl<const volatile Derived*, const volatile Base*>;

	///////////////////////////////////////////////////////////////////////////
	// ConvertibleTo
	//
	template<class From, class To>
	META_CONCEPT ConvertibleTo =
		_IsConvertibleImpl<From, To> &&
#if 1 // This is the PR for https://wg21.link/lwg3151
		requires {
			static_cast<To>(std::declval<From>());
		};
#else
		requires(From (&f)()) {
			static_cast<To>(f());
		};
#endif
		// Axiom: implicit and explicit conversion have equal results.
} STL2_CLOSE_NAMESPACE

#endif
