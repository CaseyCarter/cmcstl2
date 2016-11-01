// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_ALGORITHM_UNINITIALIZED_FILL_HPP
#define STL2_ALGORITHM_UNINITIALIZED_FILL_HPP

#include <new>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <stl2/detail/memory/reference_to.hpp>
#include <stl2/detail/tagged.hpp>

STL2_OPEN_NAMESPACE {
   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_fill [Extension]
   //
   template </*NoThrow*/ForwardIterator I, Sentinel<I> S, typename T>
   requires
      Constructible<value_type_t<I>, const T&>() &&
      __ReferenceTo<I, value_type_t<I>>()
   I uninitialized_fill(I first, S last, const T& x)
   {
      auto i = first;
      try {
         for (; i != last; ++i)
            ::new (const_cast<void*>(static_cast<const volatile void*>(__stl2::addressof(*i))))
               value_type_t<I>(x);
      }
      catch (...) {
         destroy(__stl2::move(first), __stl2::move(i));
         throw;
      }

      return __stl2::move(first);
   }

   template </*NoThrow*/ForwardRange Rng, typename T>
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
   template </*NoThrow*/ForwardIterator I, typename T>
   requires
      Constructible<value_type_t<I>, const T&>() &&
      __ReferenceTo<I, value_type_t<I>>()
   I uninitialized_fill_n(I first, const difference_type_t<I> n, const T& x)
   {
      return uninitialized_fill(make_counted_iterator(__stl2::move(first), n),
                                default_sentinel{}, x).base();
   }
} STL2_CLOSE_NAMESPACE

#endif // STL2_ALGORITHM_UNINITIALIZED_FILL_HPP
