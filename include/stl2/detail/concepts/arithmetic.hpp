// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_CONCEPTS_ARITHMETIC_HPP
#define STL2_DETAIL_CONCEPTS_ARITHMETIC_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object/regular.hpp>
#include <stl2/detail/concepts/object/semiregular.hpp>
#include <type_traits>

STL2_OPEN_NAMESPACE {
   template <class T>
   concept bool Arithmetic() {
      return Same<T, bool>() ||
       (Regular<T>() &&
        Copyable<T>() &&
        ConvertibleTo<bool, T>() &&
        StrictTotallyOrdered<T>() &&
        requires(T t) {
           {t + t} -> T;
           {t - t} -> T;
           {t * t} -> T;
           {t / t} -> T;
           {t += t} -> T&;
           {t -= t} -> T&;
           {t *= t} -> T&;
           {t /= t} -> T&;
           {++t} -> T&;
           {--t} -> T&;
           {t++} -> T;
           {t--} -> T;
           {&t} -> Same<T*>;
        });
   }

   template <class T, class U>
   concept bool Arithmetic() {
      return Arithmetic<T>() &&
        Arithmetic<U>() &&
        Common<T, U>() &&
        ConvertibleTo<T, U>() &&
        Constructible<T, U>() &&
        StrictTotallyOrdered<T, U>() &&
        requires(T t, U u) {
           {t + u} -> common_type_t<T, U>;
           {t - u} -> common_type_t<T, U>;
           {t * u} -> common_type_t<T, U>;
           {t / u} -> common_type_t<T, U>;
           {t = u} -> T&;
           {t += u} -> T&;
           {t -= u} -> T&;
           {t *= u} -> T&;
           {t /= u} -> T&;
        };
   }

   template <class A1, class A2, class A3, class... As>
   concept bool Arithmetic() {
      return Arithmetic<A1, A2>() &&
         Arithmetic<A1, A3>() &&
         Arithmetic<A1, As...>();
   }

   template <class... Ts>
   concept bool RegularArithmetic() {
      return Arithmetic<Ts...>();
   }

   namespace models {
      template <class... Ts>
      constexpr bool Arithmetic = false;
      template <__stl2::Arithmetic... Ts>
      constexpr bool Arithmetic<Ts...> = true;

      template <class... Ts>
      constexpr bool RegularArithmetic = false;
      __stl2::RegularArithmetic{... Ts}
      constexpr bool RegularArithmetic<Ts...> = true;
   } // namespace models
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_CONCEPTS_ARITHMETIC_HPP