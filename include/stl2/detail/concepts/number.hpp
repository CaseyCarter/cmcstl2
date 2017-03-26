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
   template <class N>
   concept bool Number() {
     return !Same<bool, N>() &&
       !Same<char, N>() &&
       !Same<wchar_t, N>() &&
       !Same<char16_t, N>() &&
       !Same<char32_t, N>() &&
       Regular<N>() &&
       requires(N n, const N cn) {
         {N{0}};
         {+cn} -> N;
         {-cn} -> N;
         {cn + cn} -> N; // not required to be associative or commutative
         {cn - cn} -> N;
         {cn * cn} -> N; // not required to be associative, commutative, or distributive
         {cn / cn} -> N;
         {n += cn} -> N&; // not required to be associative or commutative
         {n -= cn} -> N&;
         {n *= cn} -> N&; // not required to be associative, commutative, or distributive
         {n /= cn} -> N&;
       };
   }

   template <class N1, class N2>
   concept bool Number() {
     return Number<N1>() &&
       Number<N2>() &&
       Assignable<N1&, const N2&>() &&
       requires(N1 n1, const N1 cn, const N2 n2) {
         {cn + n2} -> common_type_t<N1, N2>; // not required to be associative or commutative
         {cn - n2} -> common_type_t<N1, N2>;
         {cn * n2} -> common_type_t<N1, N2>; // not required to be associative, commutative, or distributive
         {cn / n2} -> common_type_t<N1, N2>;
         {n2 + cn} -> common_type_t<N1, N2>; // not required to be associative or commutative
         {n2 - cn} -> common_type_t<N1, N2>;
         {n2 * cn} -> common_type_t<N1, N2>; // not required to be associative, commutative, or distributive
         {n2 / cn} -> common_type_t<N1, N2>;
         {n1 += n2} -> N1&;
         {n1 -= n2} -> N1&;
         {n1 *= n2} -> N1&;
         {n1 /= n2} -> N1&;
       };
   }

   template <class N1, class N2, class N3, class... Ns>
   concept bool Number() {
     return Number<N1, N2>() &&
       Number<N1, N3>() &&
       Number<N1, Ns...>();
   }

   // operator+ required to be associative and commutative
   // operator* required to be associative, commutative and distributive
   // Differences between Number and RegularNumber are purely semantic
   // Alternative names: IrregularNumber, WeakNumber, GeneralizedNumber
   //       These names would flip the meaning of Number and RegularNumber'
   // This concept is a product of reduce, transform_reduce, etc. not requiring commutivity
   template <class N>
   concept bool RegularNumber() {
     return Number<N>();
   }

   template <class N1, class N2>
   concept bool RegularNumber() {
     return Number<N1, N2>();
   }

   template <class N1, class N2, class N3, class... Ns>
   concept bool RegularNumber() {
     return Number<N1, N2, N3, Ns...>();
   }

   template <class N>
   concept bool OrderedNumber() {
     return Number<N>() &&
       StrictTotallyOrdered<N>() &&
       requires(N n) {
         {++n} -> N&;
         {--n} -> N&;
         {n++} -> N;
         {n--} -> N;
       };
   }

   template <class N1, class N2>
   concept bool OrderedNumber() {
     return OrderedNumber<N1>() &&
       OrderedNumber<N2>() &&
       Number<N1, N2>() &&
       StrictTotallyOrdered<N1, N2>();
   }

   template <class N1, class N2, class N3, class... Ns>
   concept bool OrderedNumber() {
     return OrderedNumber<N1, N2>() &&
       OrderedNumber<N1, N3>() &&
       OrderedNumber<N1, Ns...>();
   }

   // operator+ required to be associative and commutative
   // operator* required to be associative, commutative and distributive
   // Differences between Number and RegularStrictNumber are purely semantic
   // Alternative names: IrregularStrictNumber, WeakStrictNumber, GeneralizedStrictNumber
   //       These names would flip the meaning of Number and RegularStrictNumber'
   // This concept is a product of reduce, transform_reduce, etc. not requiring commutivity
   template <class N>
   concept bool RegularOrderedNumber() {
     return RegularNumber<N>() &&
       OrderedNumber<N>();
   }

   template <class N1, class N2>
   concept bool RegularOrderedNumber() {
     return RegularNumber<N1, N2>() &&
       OrderedNumber<N1, N2>();
   }

   template <class N1, class N2, class N3, class... Ns>
   concept bool RegularOrderedNumber() {
     return RegularNumber<N1, N2, N3, Ns...>() &&
       OrderedNumber<N1, N2, N3, Ns...>();
   }

   template <class N, class I>
   concept bool BitwiseNumber() {
     return UnsignedIntegral<I>() &&
       RegularOrderedNumber<N>() &&
       sizeof(I) <= sizeof(N) &&
       requires(N n, const N cn, const I i) {
         {cn & i} -> N;
         {cn | i} -> N;
         {cn ^ i} -> N;
         {~cn} -> N;
         {cn >> i} -> N;
         {cn << i} -> N;
         {n &= i} -> N&;
         {n |= i} -> N&;
         {n ^= i} -> N&;
         {n >>= i} -> N&;
         {n <<= i} -> N&;
       };
   }
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_CONCEPTS_NUMBER_HPP