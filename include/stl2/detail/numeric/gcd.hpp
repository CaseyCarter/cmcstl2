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
#include <stl2/tuple.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/algorithm/minmax.hpp>
#include <stl2/detail/concepts/number.hpp>

STL2_OPEN_NAMESPACE {
	template <class M, class N, class T = common_type_t<M, N>>
		requires
			Same<T, common_type_t<M, N>>() &&
			Number<T, M, N>() &&
			requires(T t) {
				{t % t} -> T;
			}
	constexpr T gcd(M m, N n)
	{
		constexpr auto zero = T{0};
		// TODO: replace with structured bindings when compiler support exists
		std::pair<T, T> p = __stl2::minmax(static_cast<T>(m < 0 ? -m : m), static_cast<T>(n < 0 ? -n : n));
		T a = p.first;
		T b = p.second;
		// end TODO

		if (a == zero || b == zero)
			return zero;

		// b = ac + r
		for (auto r = b % a; r != zero; r = b % a) {
			b = a;
			a = r;
		}

		return a;
	}
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_NUMERIC_GCD_HPP
