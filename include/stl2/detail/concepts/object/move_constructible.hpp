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
		template <class T>
		concept bool Object = _Is<T, std::is_object>;
	} // namespace ext

	namespace models {
		template <class>
		constexpr bool Object = false;
		__stl2::ext::Object{T}
		constexpr bool Object<T> = true;
	} // namespace models

	///////////////////////////////////////////////////////////////////////////
	// Addressable [Extension]
	//
	template <class T>
	concept bool __addressable =
		requires(T& t, const remove_reference_t<T>& ct) {
			{ &t } -> Same<remove_reference_t<T>*>&&;
			{ &ct } -> Same<const remove_reference_t<T>*>&&;
			// Axiom: &t == addressof(t)
			// Axiom: &ct == addressof(ct)
		};

	namespace ext {
		template <class T>
		concept bool Addressable =
			Object<T> && __addressable<T>;
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
	template <class T>
	concept bool Destructible =
		_Is<T, is_nothrow_destructible> && __addressable<T>;

	namespace models {
		template <class T>
		constexpr bool Destructible = false;
		__stl2::Destructible{T}
		constexpr bool Destructible<T> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Constructible [concepts.lib.object.constructible]
	// Not to spec
	// See https://github.com/ericniebler/stl2/issues/301
	template <class T, class... Args>
	concept bool Constructible =
		Destructible<T> && _Is<T, is_constructible, Args...>;

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
	concept bool DefaultConstructible =
		Constructible<T>;

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
	concept bool MoveConstructible =
		Constructible<T, T> && ConvertibleTo<T, T>;

	namespace models {
		template <class>
		constexpr bool MoveConstructible = false;
		__stl2::MoveConstructible{T}
		constexpr bool MoveConstructible<T> = true;
	}
} STL2_CLOSE_NAMESPACE

#endif
