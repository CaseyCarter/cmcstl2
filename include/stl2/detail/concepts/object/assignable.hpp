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
#ifndef STL2_DETAIL_CONCEPTS_OBJECT_ASSIGNABLE_HPP
#define STL2_DETAIL_CONCEPTS_OBJECT_ASSIGNABLE_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Assignable [concepts.lib.corelang.assignable]
	//
	// Not to spec
	// See https://github.com/ericniebler/stl2/issues/229
	template <class, class>
	constexpr bool __assignable = false;
	template <class T, class U>
	requires
		requires(T& t, U&& u) {
			STL2_EXACT_TYPE_CONSTRAINT(t = (U&&)u, T&);
		}
	constexpr bool __assignable<T&, U> = true;

	template <class T, class U>
	concept bool Assignable() {
		return Same<T, decay_t<T>&>() &&
		    CommonReference<T, const U&>() &&
			__assignable<T, U>;
	}

	namespace models {
		template <class, class>
		constexpr bool Assignable = false;
		__stl2::Assignable{T, U}
		constexpr bool Assignable<T, U> = true;
	}
} STL2_CLOSE_NAMESPACE

#endif
