// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2013-2014 (range-v3/include/numeric/inner_product.hpp)
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
#ifndef STL2_DETAIL_NUMERIC_INNER_PRODUCT_HPP
#define STL2_DETAIL_NUMERIC_INNER_PRODUCT_HPP

#include <stl2/functional.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/number.hpp>
#include <stl2/detail/concepts/object/semiregular.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
   template <InputIterator I1, Sentinel<I1> S, InputIterator I2, Copyable T = value_type_t<I1>,
             class Proj1 = identity, class Proj2 = identity,
             IndirectRegularCallable<projected<I1, Proj1>, projected<I2, Proj2>> BinaryOp1 = plus<value_type_t<I1>>,
             class Result = indirect_result_of_t<BinaryOp1&(projected<I1, Proj1>, projected<I2, Proj2>)>,
             RegularCallable<T, Result> BinaryOp2 = multiplies<T>>
   requires models::Assignable<T&, Result>
   T inner_product(I1 first1,
                   S last1,
                   I2 first2,
                   T init = T{},
                   BinaryOp1 binary_op1 = BinaryOp1{},
                   BinaryOp2 binary_op2 = BinaryOp2{},
                   Proj1 proj1 = Proj1{},
                   Proj2 proj2 = Proj2{})
   {
      for (; first1 != last1; ++first1, (void)++first2) {
         init = __stl2::invoke(binary_op1, init,
                               __stl2::invoke(binary_op2,
                                              __stl2::invoke(proj1, *first1),
                                              __stl2::invoke(proj2, *first2)));
      }

      return init;
   }

   template <InputRange Rng, InputIterator I, Copyable T = value_type_t<iterator_t<Rng>>,
             class Proj1 = identity, class Proj2 = identity,
             IndirectRegularCallable<projected<iterator_t<Rng>, Proj1>, projected<I, Proj2>> BinaryOp1 = plus<value_type_t<iterator_t<Rng>>>,
             class Result = indirect_result_of_t<BinaryOp1&(projected<iterator_t<Rng>, Proj1>, projected<I, Proj2>)>,
             RegularCallable<T, Result> BinaryOp2 = multiplies<T>>
   requires models::Assignable<T&, Result>
   T inner_product(Rng&& rng, I first2, T init = T{}, BinaryOp1 binary_op1 = BinaryOp1{},
                   BinaryOp2 binary_op2 = BinaryOp2{}, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
   {
      return __stl2::inner_product(__stl2::begin(rng), __stl2::end(rng), __stl2::move(first2),
                                   __stl2::move(init),
                                   __stl2::move(binary_op1), __stl2::move(binary_op2),
                                   __stl2::move(proj1), __stl2::move(proj2));
   }
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_INNER_PRODUCT_HPP
