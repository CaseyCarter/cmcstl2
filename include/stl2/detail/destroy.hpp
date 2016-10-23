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
#ifndef STL2_DETAIL_ALGORITHM_DESTROY_HPP
#define STL2_DETAIL_ALGORITHM_DESTROY_HPP
#include <stl2/concepts.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
   ///////////////////////////////////////////////////////////////////////////
   // destroy_at [Extension]
   //
   template <Destructible T>
   void destroy_at(T* p) noexcept
   {
      p->~T();
   }

   ///////////////////////////////////////////////////////////////////////////
   // destroy [Extension]
   //
   template <ForwardIterator I, Sentinel<I> S>
   requires
      models::Destructible<value_type_t<I>>
   __f<I>
   destroy(I&& first, S&& last) noexcept
   {
      for (; first != last; ++first)
         destroy_at(__stl2::addressof(*first));

      return first;
   }

   template <ForwardRange Rng>
   requires
      models::Destructible<value_type_t<iterator_t<Rng>>>
   __f<safe_iterator_t<Rng>>
   destroy(Rng&& rng) noexcept
   {
      return __stl2::destroy(__stl2::begin(rng), __stl2::end(rng));
   }

   ///////////////////////////////////////////////////////////////////////////
   // destroy_n [Extension]
   //
   template <ForwardIterator I>
   requires
      models::Destructible<value_type_t<I>>
   __f<I>
   destroy_n(I&& first, difference_type_t<I> size) noexcept
   {
      auto counted = __stl2::make_counted_iterator(__stl2::forward<I>(first), size);
      return __stl2::destroy(__stl2::move(counted), default_sentinel{});
   }
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_ALGORITHM_DESTROY_HPP
