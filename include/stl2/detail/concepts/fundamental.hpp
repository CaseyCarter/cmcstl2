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
#ifndef STL2_DETAIL_CONCEPTS_FUNDAMENTAL_HPP
#define STL2_DETAIL_CONCEPTS_FUNDAMENTAL_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Scalar [Extension]
	//
	namespace ext {
		template<class T>
		META_CONCEPT Scalar =
			std::is_scalar_v<T> && regular<T>;
	}

	///////////////////////////////////////////////////////////////////////////
	// Arithmetic [Extension]
	//
	namespace ext {
		template<class T>
		META_CONCEPT Arithmetic =
			std::is_arithmetic_v<T> && Scalar<T> && totally_ordered<T>;
	}

	///////////////////////////////////////////////////////////////////////////
	// floating_point
	//
	template<class T>
	META_CONCEPT floating_point =
		std::is_floating_point_v<T> && ext::Arithmetic<T>;

	///////////////////////////////////////////////////////////////////////////
	// integral [concepts.lib.corelang.integral]
	//
	template<class T>
	META_CONCEPT integral =
		std::is_integral_v<T> && ext::Arithmetic<T>;

	///////////////////////////////////////////////////////////////////////////
	// signed_integral [concepts.lib.corelang.signedintegral]
	//
	template<class T>
	META_CONCEPT signed_integral =
		integral<T> && (T(-1) < T(0));

	///////////////////////////////////////////////////////////////////////////
	// unsigned_integral [concepts.lib.corelang.unsignedintegral]
	//
	template<class T>
	META_CONCEPT unsigned_integral =
		integral<T> && !signed_integral<T>;
} STL2_CLOSE_NAMESPACE

#endif
