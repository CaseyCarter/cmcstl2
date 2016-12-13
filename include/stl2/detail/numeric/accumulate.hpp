// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2013-2014 (range-v3/include/numeric/accumulate.hpp)
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
#ifndef STL2_DETAIL_NUMERIC_ACCUMULATE_HPP
#define STL2_DETAIL_NUMERIC_ACCUMULATE_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object/semiregular.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
   template <InputIterator I, Sentinel<I> S,
             Copyable T = value_type_t<I>,
             RegularCallable<T, value_type_t<I>> BinaryOp = plus<>>
             // class Proj = identity,
             // IndirectRegularCallable<T, projected<I, Proj>> BinaryOp = plus<T>>
   requires Assignable<T&, result_of_t<BinaryOp&(T, value_type_t<I>)>>()
   T accumulate(I first, S last, T init = T{}, BinaryOp binary_op = BinaryOp{})
   {
      T acc = init;
      for (; first != last; ++first)
         acc = __stl2::invoke(binary_op, acc, *first);
      return acc;
   }

   template <InputRange Rng, Copyable T = value_type_t<iterator_t<Rng>>,
             RegularCallable<T, value_type_t<iterator_t<Rng>>> BinaryOp = plus<>>
   requires Assignable<T&, result_of_t<BinaryOp&(T, value_type_t<iterator_t<Rng>>)>>()
   T accumulate(Rng&& rng, T init = T{}, BinaryOp binary_op = BinaryOp{})
   {
      return accumulate(__stl2::begin(rng), __stl2::end(rng),
                        __stl2::move(init), __stl2::move(binary_op));
   }
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_ACCUMULATE_HPP