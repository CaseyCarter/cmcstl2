// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_MEMORY_UNINITIALIZED_FILL_HPP
#define STL2_DETAIL_MEMORY_UNINITIALIZED_FILL_HPP

#include <new>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/memory/construct_at.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <stl2/detail/memory/nothrow_forward_iterator.hpp>
#include <stl2/detail/memory/reference_to.hpp>
#include <stl2/detail/tagged.hpp>

STL2_OPEN_NAMESPACE {
   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_fill [Extension]
   //
   template <__NoThrowForwardIterator I, Sentinel<I> S, typename T>
   requires
      Constructible<value_type_t<I>, const T&>() &&
      __ReferenceTo<I, value_type_t<I>>()
   I uninitialized_fill(I first, S last, const T& x)
   {
      auto saved = first;
      try {
         for (; first != last; ++first)
            __stl2::__construct_at(*first, x);
      }
      catch (...) {
         __stl2::destroy(__stl2::move(saved), __stl2::move(first));
         throw;
      }

      return __stl2::move(first);
   }

   template <__NoThrowForwardRange Rng, typename T>
   requires
      Constructible<value_type_t<iterator_t<Rng>>, const T&>() &&
      __ReferenceTo<iterator_t<Rng>, value_type_t<iterator_t<Rng>>>()
   safe_iterator_t<Rng>
   uninitialized_fill(Rng&& rng, const T& x)
   {
      return __stl2::uninitialized_fill(__stl2::begin(rng),
         __stl2::end(rng), x);
   }

   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_fill_n [Extension]
   //
   template <__NoThrowForwardIterator I, typename T>
   requires
      Constructible<value_type_t<I>, const T&>() &&
      __ReferenceTo<I, value_type_t<I>>()
   I uninitialized_fill_n(I first, const difference_type_t<I> n, const T& x)
   {
      return __stl2::uninitialized_fill(__stl2::make_counted_iterator(__stl2::move(first), n),
                                        default_sentinel{}, x).base();
   }
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_UNINITIALIZED_FILL_HPP
