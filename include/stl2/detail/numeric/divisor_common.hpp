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
#ifndef STL2_DETAIL_NUMERIC_DIVSOR_COMMON_HPP
#define STL2_DETAIL_NUMERIC_DIVSOR_COMMON_HPP

#include <cinttypes>
#include <cmath>
#include <stl2/detail/concepts/fundamental.hpp>
#include <type_traits>

STL2_OPEN_NAMESPACE {
	namespace detail {
		template <class T>
		constexpr T remainder(const T a, const T b)
		{
			return std::remainder(a, b);
		}

		template <Integral I>
		constexpr I remainder(const I a, const I b)
		{
			return a % b;
		}

		template <class T>
		constexpr T abs(const T a) noexcept
		{
			return std::abs(a);
		}

		template <UnsignedIntegral I>
		constexpr I abs(const I i) noexcept
		{
         return i;
      }

      template <class T>
      constexpr auto zero = T(0);
      
      template <class N>
      concept bool ProtoNumber = requires(N n) {
         {N(0)};
         {n + n} -> N;
         {n - n} -> N;
         {n * n} -> N;
         {n / n} -> N;
         {__stl2::detail::remainder(n, n)} -> N;
         {__stl2::detail::abs(n)} -> N;
      };
   } // namespace detail
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_DIVSOR_COMMON_HPP