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
#ifndef STL_DETAIL_UNINITIALIZED_ALGORITHM_HPP
#define STL_DETAIL_UNINITIALIZED_ALGORITHM_HPP
#include <stl2/concepts.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
   namespace detail {
      template <Destructible T>
      inline void destroy_at(T* p) noexcept
      {
         p->~T();
      }

      template <ForwardIterator I, Sentinel<I> S>
      requires
         models::Destructible<reference_t<I>>
      inline __f<I>
      destroy(I first, S last) noexcept
      {
         for ( ; first != last; ++first)
            destroy_at(__stl2::addressof(*first));
      }

      template <ForwardIterator I, Sentinel<I> S, CopyConstructible T>
      requires
         models::Constructible<value_type_t<T>, reference_t<I>> &&
         models::Same<value_type_t<T>&, reference_t<T>>
      inline __f<I> uninitialized_fill(I&& first, S last, const T& value)
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

         return i;
      }

   } // namespace detail
} STL2_CLOSE_NAMESPACE
#endif // STL_DETAIL_UNINITIALIZED_ALGORITHM_HPP
