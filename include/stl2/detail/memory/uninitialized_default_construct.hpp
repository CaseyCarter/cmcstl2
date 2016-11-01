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
#ifndef STL2_DETAIL_ALGORITHM_UNINITIALIZED_DEFAULT_CONSTRUCT_HPP
#define STL2_DETAIL_ALGORITHM_UNINITIALIZED_DEFAULT_CONSTRUCT_HPP

#include <new>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <stl2/detail/memory/reference_to.hpp>
#include <stl2/detail/tagged.hpp>

STL2_OPEN_NAMESPACE {
   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_default_construct [Extension]
   //
   template </*NoThrow*/ForwardIterator I, Sentinel<I> S>
   requires DefaultConstructible<value_type_t<I>>() &&
            __ReferenceTo<I, value_type_t<I>>()
   I uninitialized_default_construct(I first, S last)
   {
      for (; first != last; ++first)
         ::new (const_cast<void*>(static_cast<const volatile void*>(__stl2::addressof(*first))))
            value_type_t<I>;
      return first;
   }

   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_default_construct [Extension]
   //
   template </*NoThrow*/ForwardRange Rng>
   requires DefaultConstructible<value_type_t<iterator_t<Rng>>>() &&
            __ReferenceTo<iterator_t<Rng>, value_type_t<iterator_t<Rng>>>()
   safe_iterator_t<Rng> uninitialized_default_construct(Rng&& rng)
   {
      return uninitialized_default_construct(begin(rng), end(rng));
   }

   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_default_construct_n [Extension]
   //
   template </*NoThrow*/ForwardIterator I>
   requires DefaultConstructible<value_type_t<I>>() &&
            __ReferenceTo<I, value_type_t<I>>()
   I uninitialized_default_construct_n(I first, difference_type_t<I> n)
   {
      return uninitialized_default_construct(make_counted_iterator(first, n),
                                             default_sentinel{}).base();
   }
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_ALGORITHM_UNINITIALIZED_DEFAULT_CONSTRUCT_HPP