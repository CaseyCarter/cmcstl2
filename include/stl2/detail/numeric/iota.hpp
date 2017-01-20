// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2013 -- 2017 (range-v3/include/numeric/iota.hpp)
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
#ifndef STL2_DETAIL_NUMERIC_IOTA_HPP
#define STL2_DETAIL_NUMERIC_IOTA_HPP

#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/iterator.hpp>

STL2_OPEN_NAMESPACE {
   template <ForwardIterator I, Sentinel<I> S, class T>
   requires
      models::ConvertibleTo<T, value_type_t<I>> && // is this necessary with line 25? I think not
      models::Writable<I, T> &&
      requires(T t) {
         {++t} -> T&;
      }
   void iota(I first, S last, T value)
   {
      for (; first != last; ++first, (void)++value)
         *first = value;
   }

   template <ForwardRange Rng, class T>
   requires
      models::ConvertibleTo<T, value_type_t<iterator_t<Rng>>> && // ditto :24
      models::Writable<iterator_t<Rng>, T> &&
      requires(T t) {
         {++t} -> T&;
      }
   void iota(Rng&& rng, T value)
   {
      iota(__stl2::begin(rng), __stl2::end(rng), __stl2::move(value));
   }
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_IOTA_HPP
