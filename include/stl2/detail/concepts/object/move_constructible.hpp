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

	///////////////////////////////////////////////////////////////////////////
	// Destructible [concepts.lib.object.destructible]
	//
	template <class T>
	concept bool Destructible =
		_Is<T, is_nothrow_destructible> && __addressable<T>;

	///////////////////////////////////////////////////////////////////////////
	// Constructible [concepts.lib.object.constructible]
	//
	template <class T, class... Args>
	concept bool Constructible =
		Destructible<T> && _Is<T, is_constructible, Args...>;

	///////////////////////////////////////////////////////////////////////////
	// DefaultConstructible [concepts.lib.object.defaultconstructible]
	//
	template <class T>
	concept bool DefaultConstructible =
		Constructible<T>;

	///////////////////////////////////////////////////////////////////////////
	// MoveConstructible [concepts.lib.object.moveconstructible]
	//
	template <class T>
	concept bool MoveConstructible =
		Constructible<T, T> && ConvertibleTo<T, T>;
} STL2_CLOSE_NAMESPACE

#endif
