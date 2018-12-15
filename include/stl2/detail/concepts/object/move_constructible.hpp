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
#ifndef STL2_DETAIL_CONCEPTS_OBJECT_MOVE_CONSTRUCTIBLE_HPP
#define STL2_DETAIL_CONCEPTS_OBJECT_MOVE_CONSTRUCTIBLE_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Object [Extension]
	//
	namespace ext {
		template<class T>
		STL2_CONCEPT Object = _Is<T, std::is_object>;
	} // namespace ext

	///////////////////////////////////////////////////////////////////////////
	// Destructible [concept.destructible]
	//
	template<class T>
	STL2_CONCEPT Destructible = _Is<T, std::is_nothrow_destructible>;

	///////////////////////////////////////////////////////////////////////////
	// Constructible [concept.constructible]
	//
	template<class T, class... Args>
	STL2_CONCEPT Constructible =
		Destructible<T> && _Is<T, std::is_constructible, Args...>;

	///////////////////////////////////////////////////////////////////////////
	// DefaultConstructible [concept.defaultconstructible]
	//
	template<class T>
	STL2_CONCEPT DefaultConstructible = Constructible<T> && requires { T{}; };

	///////////////////////////////////////////////////////////////////////////
	// MoveConstructible [concept.moveconstructible]
	//
	template<class T>
	STL2_CONCEPT MoveConstructible = Constructible<T, T> && ConvertibleTo<T, T>;
} STL2_CLOSE_NAMESPACE

#endif
