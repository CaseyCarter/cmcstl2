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
	// same_as
	//
	template<class T, class U>
	META_CONCEPT same_as = meta::Same<T, U> && meta::Same<U, T>;

	template<class T>
	META_CONCEPT _Decayed = meta::Same<T, std::decay_t<T>>;

	template<class T, class... Args>
	META_CONCEPT _OneOf = (meta::Same<T, Args> || ...);

	template<class T, class U>
	META_CONCEPT _NotSameAs = !meta::Same<__uncvref<T>, __uncvref<U>>;

	///////////////////////////////////////////////////////////////////////////
	// derived_from
	//
	template<class Derived, class Base>
	META_CONCEPT derived_from =
		META_CONCEPT_BARRIER(STL2_IS_BASE(Base, Derived)) &&
		META_CONCEPT_BARRIER(STL2_IS_CONVERTIBLE(
			const volatile Derived*, const volatile Base*));

	///////////////////////////////////////////////////////////////////////////
	// convertible_to
	//
	template<class From, class To>
	META_CONCEPT convertible_to =
		META_CONCEPT_BARRIER(STL2_IS_CONVERTIBLE(From, To)) &&
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
