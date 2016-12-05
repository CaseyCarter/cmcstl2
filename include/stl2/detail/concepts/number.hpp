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
#ifndef STL2_DETAIL_CONCEPTS_NUMBER_HPP
#define STL2_DETAIL_CONCEPTS_NUMBER_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/detail/concepts/object/regular.hpp>
#include <stl2/detail/concepts/object/semiregular.hpp>
#include <type_traits>

STL2_OPEN_NAMESPACE {
   template <class T>
   concept bool WeakNumber() {
      return !models::Same<bool, T> &&
         !models::Same<char, T> &&
         !models::Same<wchar_t, T> &&
         !models::Same<char16_t, T> &&
         !models::Same<char32_t, T> &&
         models::Regular<T> &&
         models::Copyable<T> &&
         requires(T t, bool b) {
            {+t} -> T;
            {-t} -> T;
            {t + t} -> T;
            {t - t} -> T;
            {t * t} -> T;
            {t / t} -> T;
            {t += t} -> T&;
            {t -= t} -> T&;
            {t *= t} -> T&;
            {t /= t} -> T&;
            {&t} -> Same<T*>;
            {static_cast<T>(b)} -> T; // or Constructible<T, bool>?
         };
   }

   namespace models {
      template <class...>
      constexpr bool WeakNumber = false;
      template <__stl2::WeakNumber T>
      constexpr bool WeakNumber<T> = true;
   }

   template <class T, class U>
   concept bool WeakNumber() {
      return models::WeakNumber<T> &&
         models::WeakNumber<U> &&
         models::Common<T, U> &&
         requires(T t, U u) {
            {t + u} -> common_type_t<T, U>;
            {t - u} -> common_type_t<T, U>;
            {t * u} -> common_type_t<T, U>;
            {t / u} -> common_type_t<T, U>;
            {u + t} -> common_type_t<U, T>;
            {u - t} -> common_type_t<U, T>;
            {u * t} -> common_type_t<U, T>;
            {u / t} -> common_type_t<U, T>;
            {t = u} -> T&;
            {t += u} -> T&;
            {t -= u} -> T&;
            {t *= u} -> T&;
            {t /= u} -> T&;
            {static_cast<T>(u)} -> T;
         };
   }

   namespace models {
      template <class T, class U>
         requires __stl2::WeakNumber<T, U>()
      constexpr bool WeakNumber<T, U> = true;
   }

   template <class A1, class A2, class A3, class... As>
   concept bool WeakNumber() {
      return models::WeakNumber<A1, A2> &&
         models::WeakNumber<A1, A3> &&
         WeakNumber<A1, As...>();
   }

   namespace models {
      template <class N1, class N2, class N3, class... Ns>
         requires __stl2::WeakNumber<N1, N2, N3, Ns...>()
      constexpr bool WeakNumber<N1, N2, N3, Ns...> = true;
   }

   template <class... Ts>
   concept bool RegularWeakNumber() {
      return models::WeakNumber<Ts...>;
   }

   namespace models {
      template <class...>
      constexpr bool RegularWeakNumber = false;
      template <class... Ts>
         requires __stl2::RegularWeakNumber<Ts...>()
      constexpr bool RegularWeakNumber<Ts...> = true;
   }

   template <class T>
   concept bool Number() {
      return models::WeakNumber<T> &&
         models::StrictTotallyOrdered<T> &&
         requires(T t) {
            {++t} -> T&;
            {--t} -> T&;
            {t++} -> T;
            {t--} -> T;
            //{static_cast<bool>(t)} -> bool;
          };
   }

   namespace models {
      template <class...>
      constexpr bool Number = false;
      template <__stl2::Number T>
      constexpr bool Number<T> = true;
   }

   template <class T, class U>
   concept bool Number() {
      return models::Number<T> &&
         models::Number<U> &&
         models::WeakNumber<T, U> &&
         models::StrictTotallyOrdered<T, U>;
   }

   namespace models {
      template <class T, class U>
         requires __stl2::Number<T, U>()
      constexpr bool Number<T, U> = true;
   }

   template <class A1, class A2, class A3, class... As>
   concept bool Number() {
      return models::Number<A1, A2> &&
         models::Number<A1, A3> &&
         Number<A1, As...>();
   }

   namespace models {
      template <class N1, class N2, class N3, class... Ns>
         requires __stl2::Number<N1, N2, N3, Ns...>()
      constexpr bool Number<N1, N2, N3, Ns...> = true;
   }

   template <class... Ts>
   concept bool RegularNumber() {
      return models::Number<Ts...>;
   }

   namespace models {
      template <class...>
      constexpr bool RegularNumber = false;
      template <class... Ts>
         requires __stl2::RegularNumber<Ts...>()
      constexpr bool RegularNumber<Ts...> = true;
   } // namespace models
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_CONCEPTS_NUMBER_HPP