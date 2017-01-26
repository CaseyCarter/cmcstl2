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
	// Addressable [Extension]
	//
	template <class>
	constexpr bool __addressable = false;
	template <class T>
		requires requires(T& t, const T& ct) {
			STL2_EXACT_TYPE_CONSTRAINT(&t, add_pointer_t<T>);
			STL2_EXACT_TYPE_CONSTRAINT(&ct, add_pointer_t<const T>);
			// Axiom: &ct == addressof(ct)
		}
	constexpr bool __addressable<T> = true;

	namespace ext {
		template <class T>
		concept bool Addressable() {
			return _Is<T, is_object> && __addressable<T>;
		}
	}

	namespace models {
		template <class>
		constexpr bool Addressable = false;
		__stl2::ext::Addressable{T}
		constexpr bool Addressable<T> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Destructible [concepts.lib.object.destructible]
	// Not to spec
	// https://github.com/ericniebler/stl2/issues/301
	namespace models {
		template <class T>
		constexpr bool Destructible =
			is_nothrow_destructible<T>::value && __addressable<T>;
	}

	template <class T>
	concept bool Destructible() {
		return models::Destructible<T>;
	}

	///////////////////////////////////////////////////////////////////////////
	// Constructible [concepts.lib.object.constructible]
	// Not to spec
	// See https://github.com/ericniebler/stl2/issues/301
	template <class T, class... Args>
	concept bool Constructible() {
		return Destructible<T>() && _Is<T, is_constructible, Args...>;
	}

	namespace models {
		template <class, class...>
		constexpr bool Constructible = false;
		__stl2::Constructible{T, ...Args}
		constexpr bool Constructible<T, Args...> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// DefaultConstructible [concepts.lib.object.defaultconstructible]
	// Not to spec
	// https://github.com/ericniebler/stl2/issues/301
	template <class T>
	concept bool DefaultConstructible() {
		return Constructible<T>();
	}

	namespace models {
		template <class>
		constexpr bool DefaultConstructible = false;
		__stl2::DefaultConstructible{T}
		constexpr bool DefaultConstructible<T> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// MoveConstructible [concepts.lib.object.moveconstructible]
	// Not to spec
	// https://github.com/ericniebler/stl2/issues/313
	template <class T>
	concept bool MoveConstructible() {
		return Constructible<T, T>() &&
			ConvertibleTo<T, T>();
	}

	namespace models {
		template <class>
		constexpr bool MoveConstructible = false;
		__stl2::MoveConstructible{T}
		constexpr bool MoveConstructible<T> = true;
	}
} STL2_CLOSE_NAMESPACE

#endif
