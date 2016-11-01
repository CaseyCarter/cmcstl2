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
#ifndef STL2_ALGORITHM_UNINITIALIZED_MOVE_HPP
#define STL2_ALGORITHM_UNINITIALIZED_MOVE_HPP

#include <new>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <stl2/detail/memory/reference_to.hpp>
#include <stl2/detail/tagged.hpp>

STL2_OPEN_NAMESPACE {
   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_move [Extension]
   //
   template <InputIterator I, Sentinel<I> S, /*NoThrow*/ForwardIterator O>
   requires
      Constructible<value_type_t<I>, rvalue_reference_t<O>>() &&
      __ReferenceTo<I, value_type_t<O>>()
   tagged_pair<tag::in(I), tag::out(O)> uninitialized_move(I first, S last, O result)
   {
      auto i = result;
      try {
         for (; first != last; (void)++result, ++first)
            ::new (const_cast<void*>(static_cast<const volatile void*>(__stl2::addressof(*result))))
               value_type_t<O>(__stl2::move(*first));
      }
      catch (...) {
         destroy(__stl2::move(i), __stl2::move(result));
         throw;
      }

      return {first, result};
   }

   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_move [Extension]
   //
   template <InputRange Rng, /*NoThrow*/ForwardIterator O>
   requires
      Constructible<value_type_t<iterator_t<Rng>>, rvalue_reference_t<O>>() &&
      __ReferenceTo<iterator_t<Rng>, value_type_t<O>>()
   tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)> uninitialized_move(Rng&& rng, O result)
   {
      return uninitialized_move(std::begin(rng), std::end(rng), __stl2::move(result));
   }

   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_move_n [Extension]
   //
   template <InputIterator I, /*NoThrow*/ForwardIterator O>
   requires
      Constructible<value_type_t<I>, rvalue_reference_t<O>>() &&
      __ReferenceTo<I, value_type_t<I>>()
   tagged_pair<tag::in(I), tag::out(O)>
   uninitialized_move_n(I first, difference_type_t<I> n, O result)
   {
      auto r = uninitialized_move(make_counted_iterator(first, n), default_sentinel{}, __stl2::move(result));
      return {r.in().base(), r.out()};
   }
} STL2_CLOSE_NAMESPACE

#endif // STL2_ALGORITHM_UNINITIALIZED_MOVE_HPP