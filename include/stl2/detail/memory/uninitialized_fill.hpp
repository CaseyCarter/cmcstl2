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
#include <stl2/detail/destroy.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>

STL2_OPEN_NAMESPACE {
   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_fill [Extension]
   //
   template <ForwardIterator I, Sentinel<I> S, CopyConstructible T>
   requires
      models::Constructible<value_type_t<T>, reference_t<I>> &&
      models::Same<value_type_t<T>&, reference_t<T>>
   inline tag::in(I)
   uninitialized_fill(I&& first, S last, const T& value)
   {
      auto i = first;
      try {
         for ( ; i != last; ++i)
            ::new(static_cast<void*>(&*i)) T(value);
      }
      catch (...) {
         destroy(__stl2::forward<I>(first), __stl2::move(i));
         throw;
      }

      return __stl2::move(first);
   }

   template <ForwardRange Rng, CopyConstructible T>
   requires
      models::Constructible<value_type_t<__f<T>>, reference_t<iterator_t<Rng>>> &&
      models::Same<value_type_t<T>&, reference_t<T>>
   inline tag::in(I)
   uninitialized_fill(Rng&& rng, const T& value)
   {
      return __stl2::uninitialized_fill(__stl2::begin(rng),
         __stl2::end(rng), value);
   }

   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_fill_n [Extension]
   //
   template <ForwardIterator I, CopyConstructible T>
   requires
      models::Constructible<value_type_t<T>, reference_t<I>> &&
      models::Same<value_type_t<T>&, reference_t<T>>
   inline __f<I>
   uninitialized_fill_n(I&& first, const Integral size, const T& value)
   {
      auto counted = __stl2::make_counted_iterator(__stl2::forward<I>(first), size);
      return uninitialized_fill(__stl2::move(counted),
         default_sentinel{}, value).in().base();
   }
} STL2_CLOSE_NAMESPACE

#endif // STL2_ALGORITHM_UNINITIALIZED_FILL_HPP
