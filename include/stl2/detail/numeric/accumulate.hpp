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
#ifndef STL2_DETAIL_NUMERIC_ACCUMULATE_HPP
#define STL2_DETAIL_NUMERIC_ACCUMULATE_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object/semiregular.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
   template <InputIterator I, Sentinel<I> S,
             Copyable T = value_type_t<I>,
             RegularCallable<value_type_t<I>, T> B = plus<T>>
   T accumulate(I first, S last, T init = T{}, B binary_op = B{})
   {
      T acc = init;
      for (; first != last; ++first)
         acc = binary_op(acc, *first);

      return acc;
   }

   template <InputRange Rng, Copyable T = value_type_t<iterator_t<Rng>>,
             RegularCallable<value_type_t<iterator_t<Rng>>, T> B = plus<T>>
   T accumulate(Rng&& rng, T init = T{}, B binary_op = B{})
   {
      return accumulate(__stl2::begin(rng), __stl2::end(rng),
                        __stl2::move(init), __stl2::move(binary_op));
   }
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_ACCUMULATE_HPP