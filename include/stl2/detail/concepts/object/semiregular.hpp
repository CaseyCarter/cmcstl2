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
	// copy_constructible [concepts.lib.object.copyconstructible]
	//
	template<class T>
	META_CONCEPT copy_constructible =
		move_constructible<T> &&
		constructible_from<T, T&> && convertible_to<T&, T> &&
		constructible_from<T, const T&> && convertible_to<const T&, T> &&
		constructible_from<T, const T> && convertible_to<const T, T>;

	///////////////////////////////////////////////////////////////////////////
	// copyable [concepts.lib.object.copyable]
	//
	template<class T>
	META_CONCEPT copyable =
		copy_constructible<T> && movable<T> && assignable_from<T&, const T&>;

	///////////////////////////////////////////////////////////////////////////
	// semiregular [concepts.lib.object.semiregular]
	//
	template<class T>
	META_CONCEPT semiregular = copyable<T> && default_initializable<T>;
} STL2_CLOSE_NAMESPACE

#endif
