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
#ifndef STL2_DETAIL_CONCEPTS_OBJECT_MOVABLE_HPP
#define STL2_DETAIL_CONCEPTS_OBJECT_MOVABLE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/swap.hpp>
#include <stl2/detail/concepts/object/assignable.hpp>
#include <stl2/detail/concepts/object/move_constructible.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Movable [concepts.lib.object.movable]
	//
	// Not to spec
	// https://github.com/ericniebler/stl2/issues/310
	template <class T>
	concept bool Movable =
		ext::Object<T> &&
		MoveConstructible<T> &&
		Assignable<T&, T> &&
		Swappable<T>;

	namespace models {
		template <class>
		constexpr bool Movable = false;
		__stl2::Movable{T}
		constexpr bool Movable<T> = true;
	} // namespace models
} STL2_CLOSE_NAMESPACE

#endif
