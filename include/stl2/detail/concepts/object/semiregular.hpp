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
#ifndef STL2_DETAIL_CONCEPTS_OBJECT_SEMIREGULAR_HPP
#define STL2_DETAIL_CONCEPTS_OBJECT_SEMIREGULAR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object/assignable.hpp>
#include <stl2/detail/concepts/object/movable.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// CopyConstructible [concepts.lib.object.copyconstructible]
	//
	template<class T>
	STL2_CONCEPT CopyConstructible =
		MoveConstructible<T> &&
		Constructible<T, T&> && ConvertibleTo<T&, T> &&
		Constructible<T, const T&> && ConvertibleTo<const T&, T> &&
		Constructible<T, const T> && ConvertibleTo<const T, T>;

	///////////////////////////////////////////////////////////////////////////
	// Copyable [concepts.lib.object.copyable]
	//
	template<class T>
	STL2_CONCEPT Copyable =
		CopyConstructible<T> && Movable<T> && Assignable<T&, const T&>;

	///////////////////////////////////////////////////////////////////////////
	// Semiregular [concepts.lib.object.semiregular]
	//
	template<class T>
	STL2_CONCEPT Semiregular = Copyable<T> && DefaultConstructible<T>;
} STL2_CLOSE_NAMESPACE

#endif
