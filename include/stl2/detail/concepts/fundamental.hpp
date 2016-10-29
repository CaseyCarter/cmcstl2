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
		concept bool Scalar() {
			return _Is<T, is_scalar> && Regular<T>();
		}
	}

	namespace models {
		template <class>
		constexpr bool Scalar = false;
		__stl2::ext::Scalar{T}
		constexpr bool Scalar<T> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Arithmetic [Extension]
	//
	namespace ext {
		template <class T>
		concept bool Arithmetic() {
			return _Is<T, is_arithmetic> && Scalar<T>() && StrictTotallyOrdered<T>();
		}
	}

	namespace models {
		template <class>
		constexpr bool Arithmetic = false;
		__stl2::ext::Arithmetic{T}
		constexpr bool Arithmetic<T> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// FloatingPoint [Extension]
	//
	namespace ext {
		template <class T>
		concept bool FloatingPoint() {
			return _Is<T, is_floating_point> && Arithmetic<T>();
		}
	}

	namespace models {
		template <class>
		constexpr bool FloatingPoint = false;
		__stl2::ext::FloatingPoint{T}
		constexpr bool FloatingPoint<T> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Integral [concepts.lib.corelang.integral]
	//
	template <class T>
	concept bool Integral() {
		return _Is<T, is_integral> && ext::Arithmetic<T>();
	}

	namespace models {
		template <class>
		constexpr bool Integral = false;
		__stl2::Integral{T}
		constexpr bool Integral<T> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// SignedIntegral [concepts.lib.corelang.signedintegral]
	//
	template <class T>
	concept bool SignedIntegral() {
		return Integral<T>() && (T(-1) < T(0));
	}

	namespace models {
		template <class>
		constexpr bool SignedIntegral = false;
		__stl2::SignedIntegral{T}
		constexpr bool SignedIntegral<T> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// UnsignedIntegral [concepts.lib.corelang.unsignedintegral]
	//
	template <class T>
	concept bool UnsignedIntegral() {
		return Integral<T>() && !SignedIntegral<T>();
	}

	namespace models {
		template <class>
		constexpr bool UnsignedIntegral = false;
		__stl2::UnsignedIntegral{T}
		constexpr bool UnsignedIntegral<T> = true;
	}
} STL2_CLOSE_NAMESPACE

#endif
