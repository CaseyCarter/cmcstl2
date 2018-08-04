// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015 -- 2017
//  Copyright Christopher Di Bella 2016 -- 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_NUMERIC_LCM_HPP
#define STL2_DETAIL_NUMERIC_LCM_HPP

#include <stl2/tuple.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/algorithm/minmax.hpp>
#include <stl2/detail/numeric/gcd.hpp>
#include "divisor_common.hpp"

STL2_OPEN_NAMESPACE {
	template <class T>
	requires
		detail::ProtoNumber<T>
	constexpr T lcm(T a, T b) noexcept
	{
		using detail::zero;
		if (a == zero<T> && b != zero<T>)
			return detail::abs(b); // lcm(0, n) = |n|
		if (b == zero<T>)
			return detail::abs(a); // lcm(m, 0) = |m|

		// lcm(a, b) = ab/gcd(a, b)
		const auto gcd = __stl2::gcd(a, b);
		return gcd == zero<T> ? zero<T> : detail::abs(a * b) / gcd;
	}

	template <class M, class N, class T = common_type_t<M, N>>
	requires
		Same<T, common_type_t<M, N>> &&
		detail::ProtoNumber<T>
	constexpr T lcm(M m, N n)
	{
		return lcm(T(m), T(n));
	}
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_NUMERIC_LCM_HPP
