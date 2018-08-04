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
#ifndef STL2_DETAIL_NUMERIC_GCD_HPP
#define STL2_DETAIL_NUMERIC_GCD_HPP

#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/type_traits.hpp>
#include "divisor_common.hpp"

STL2_OPEN_NAMESPACE {
	template <class T>
	requires
		detail::ProtoNumber<T>
	constexpr T gcd(T a, T b) noexcept
	{
		using detail::zero;
		if (a == zero<T> && b == zero<T>)
			return zero<T>;

		// algorithm Copyright Alexander Stepanov, Paul McJones 2009
		// Elements of Programming
		while (true) {
			if (b == zero<T>)
				return detail::abs(a);
			a = detail::remainder(a, b);
			if (a == zero<T>)
				return detail::abs(b);
			b = detail::remainder(b, a);
		}
	}

	template <class M, class N, class T = common_type_t<M, N>>
	requires
		Same<T, common_type_t<M, N>> &&
		detail::ProtoNumber<T>
	constexpr T gcd(M m, N n) noexcept
	{
		return __stl2::gcd(T(m), T(n));
	}
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_NUMERIC_GCD_HPP
