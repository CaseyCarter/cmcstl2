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
		template <class T>
		concept bool Scalar =
			_Is<T, is_scalar> && Regular<T>;
	}

	///////////////////////////////////////////////////////////////////////////
	// Arithmetic [Extension]
	//
	namespace ext {
		template <class T>
		concept bool Arithmetic =
			_Is<T, is_arithmetic> && Scalar<T> && StrictTotallyOrdered<T>;
	}

	///////////////////////////////////////////////////////////////////////////
	// FloatingPoint [Extension]
	//
	namespace ext {
		template <class T>
		concept bool FloatingPoint =
			_Is<T, is_floating_point> && Arithmetic<T>;
	}

	///////////////////////////////////////////////////////////////////////////
	// Integral [concepts.lib.corelang.integral]
	//
	template <class T>
	concept bool Integral =
		_Is<T, is_integral> && ext::Arithmetic<T>;

	///////////////////////////////////////////////////////////////////////////
	// SignedIntegral [concepts.lib.corelang.signedintegral]
	//
	template <class T>
	concept bool SignedIntegral =
		Integral<T> && (T(-1) < T(0));

	///////////////////////////////////////////////////////////////////////////
	// UnsignedIntegral [concepts.lib.corelang.unsignedintegral]
	//
	template <class T>
	concept bool UnsignedIntegral =
		Integral<T> && !SignedIntegral<T>;
} STL2_CLOSE_NAMESPACE

#endif
