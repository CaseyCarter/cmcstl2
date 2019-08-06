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
		META_CONCEPT Object = std::is_object_v<T>;
	} // namespace ext

	///////////////////////////////////////////////////////////////////////////
	// destructible [concept.destructible]
	//
	template<class T>
	META_CONCEPT destructible = std::is_nothrow_destructible_v<T>;

	///////////////////////////////////////////////////////////////////////////
	// constructible_from [concept.constructible]
	//
	template<class T, class... Args>
	META_CONCEPT constructible_from =
		destructible<T> && std::is_constructible_v<T, Args...>;

	///////////////////////////////////////////////////////////////////////////
	// default_initializable [concept.defaultconstructible]
	//
	template<class T>
	META_CONCEPT default_initializable = constructible_from<T>
#if !STL2_WORKAROUND_GCC_UNKNOWN0 // Implement P/R for LWG 3149
		&& requires { T{}; } // && is-default-initializable<T>
#endif // LWG 3149
		;

	///////////////////////////////////////////////////////////////////////////
	// move_constructible [concept.moveconstructible]
	//
	template<class T>
	META_CONCEPT move_constructible = constructible_from<T, T> && convertible_to<T, T>;
} STL2_CLOSE_NAMESPACE

#endif
