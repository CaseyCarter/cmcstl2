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
#ifndef STL2_DETAIL_NUMERIC_ADJACENT_DIFFERENCE_HPP
#define STL2_DETAIL_NUMERIC_ADJACENT_DIFFERENCE_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object/movable.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
   template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O,
             class Proj = identity,
             IndirectRegularCallable<projected<I, Proj>, projected<I, Proj>> BinaryOp = minus<value_type_t<I>>>
   requires models::IndirectlyCopyable<I, O> &&
      models::Movable<value_type_t<I>>// &&
      //Constructible<value_type_t<I>, value_type_t<I>>()
   tagged_pair<tag::in(__f<I>), tag::out(__f<O>)>
   adjacent_difference(I first, S last, O result, BinaryOp binary_op = BinaryOp{}, Proj proj = Proj{})
   {
      
      if (first != last) {
         auto acc = *first++;
         *result++ = acc;
         for (; first != last; ++first, (void)++result) {
            auto val = *first;
            *result = __stl2::invoke(binary_op, __stl2::invoke(proj, val),
                                     __stl2::invoke(proj, acc));
            acc = __stl2::move(val);
         }
      }

      return {first, result};
   }
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_NUMERIC_ADJACENT_DIFFERENCE_HPP