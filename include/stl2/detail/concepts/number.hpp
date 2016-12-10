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
   concept bool Number() {
      return !models::Same<bool, T> && // not a fan of these, but I dont' know a better way at
         !models::Same<char, T> &&     // present
         !models::Same<wchar_t, T> &&
         !models::Same<char16_t, T> &&
         !models::Same<char32_t, T> &&
         models::Regular<T> &&
         models::Copyable<T> &&
         requires(T t, bool b) {
            {+t} -> T;
            {-t} -> T;
            {t + t} -> T; // not required to be associative or commutative
            {t - t} -> T;
            {t * t} -> T; // not requried to be associative, commutative, or distributive
            {t / t} -> T;
            {t += t} -> T&;
            {t -= t} -> T&;
            {t *= t} -> T&;
            {t /= t} -> T&;
            {&t} -> Same<T*>;
            // Constructible<T, bool> as an alternative? I think not, but commentary welcome
            {static_cast<T>(b)} -> T;
         };
   }

   namespace models {
      template <class...>
      constexpr bool Number = false;
      __stl2::Number{T}
      constexpr bool Number<T> = true;
   }

   template <class T, class U>
   concept bool Number() {
      return models::Number<T> &&
         models::Number<U> &&
         models::Common<T, U> &&
         requires(T t, U u) {
            {t + u} -> common_type_t<T, U>; // not required to be associative or commutative
            {t - u} -> common_type_t<T, U>;
            {t * u} -> common_type_t<T, U>; // not required to be associative, commutative, or distributive
            {t / u} -> common_type_t<T, U>;
            {u + t} -> common_type_t<U, T>; // not required to be associative or commutative
            {u - t} -> common_type_t<U, T>;
            {u * t} -> common_type_t<U, T>; // not required to be associative, commutative, or distributive
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

   // operator+ required to be associative and commutative
   // operator* required to be associative, commutative and distributive
   // Differences between Number and RegularNumber are purely semantic
   // Alternative names: IrregularNumber, WeakNumber, GeneralizedNumber
   //       These names would flip the meaning of Number and RegularNumber'
   // This concept is a product of reduce, transform_reduce, etc. not requiring commutivity
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
   }

   template <class T>
   concept bool StrictNumber() {
      return models::Number<T> &&
         models::StrictTotallyOrdered<T> &&
         requires(T t) {
            {++t} -> T&;
            {--t} -> T&;
            {t++} -> T;
            {t--} -> T;
            // here for commentary... this is actually a problematic requirement!
            //{static_cast<bool>(t)} -> bool;
          };
   }

   namespace models {
      template <class...>
      constexpr bool StrictNumber = false;
      __stl2::StrictNumber{T}
      constexpr bool StrictNumber<T> = true;
   }

   template <class T, class U>
   concept bool StrictNumber() {
      return models::StrictNumber<T> &&
         models::StrictNumber<U> &&
         models::Number<T, U> &&
         models::StrictTotallyOrdered<T, U>;
   }

   namespace models {
      template <class T, class U>
      requires __stl2::StrictNumber<T, U>()
      constexpr bool StrictNumber<T, U> = true;
   }

   template <class A1, class A2, class A3, class... As>
   concept bool StrictNumber() {
      return models::StrictNumber<A1, A2> &&
         models::StrictNumber<A1, A3> &&
         StrictNumber<A1, As...>();
   }

   namespace models {
      template <class N1, class N2, class N3, class... Ns>
      requires __stl2::StrictNumber<N1, N2, N3, Ns...>()
      constexpr bool StrictNumber<N1, N2, N3, Ns...> = true;
   }

   // operator+ required to be associative and commutative
   // operator* required to be associative, commutative and distributive
   // Differences between Number and RegularStrictNumber are purely semantic
   // Alternative names: IrregularStrictNumber, WeakStrictNumber, GeneralizedStrictNumber
   //       These names would flip the meaning of Number and RegularStrictNumber'
   // This concept is a product of reduce, transform_reduce, etc. not requiring commutivity
   template <class... Ts>
   concept bool RegularStrictNumber() {
      return models::StrictNumber<Ts...>;
   }

   namespace models {
      template <class...>
      constexpr bool RegularStrictNumber = false;
      template <class... Ts>
      requires __stl2::RegularStrictNumber<Ts...>()
      constexpr bool RegularStrictNumber<Ts...> = true;
   } // namespace models
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_CONCEPTS_NUMBER_HPP