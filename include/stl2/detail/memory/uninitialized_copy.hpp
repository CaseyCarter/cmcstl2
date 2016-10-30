// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_UNINITIALIZED_COPY_HPP
#define STL2_DETAIL_ALGORITHM_UNINITIALIZED_COPY_HPP

#include <new>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/memory/destroy.hpp>

STL2_OPEN_NAMESPACE {
   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_copy [Extension]
   //
   template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O>
   requires
      models::Constructible<value_type_t<O>, reference_t<I>> &&
      models::Same<value_type_t<O>&, reference_t<O>> &&
      __stl2::CopyConstructible<value_type_t<O>>()
   tagged_pair<tag::in(I), tag::out(O)>
   uninitialized_copy(I first, S last, O result)
   {
      auto saved = result;
      try {
         for (; first != last; ++first, void(++result)) {
            ::new(static_cast<void*>(__stl2::addressof(result))) value_type_t<I>(*first);
         }
      } catch(...) {
         for (; saved != result; ++saved)
            destroy_at(__stl2::addressof(*result));
         throw;
      }
      return {__stl2::move(first), __stl2::move(result)};
   }

   template <InputRange Rng, class O>
   requires
      models::ForwardIterator<__f<O>> &&
      models::Constructible<value_type_t<__f<O>>, reference_t<iterator_t<Rng>>> &&
      models::Same<value_type_t<__f<O>>&, reference_t<__f<O>>>
   auto //tagged_pair<tag::in(iterator_t<Rng>), tag::out(__f<O>)>
   uninitialized_copy(Rng&& rng, O&& result)
   {
      return __stl2::uninitialized_copy(__stl2::begin(rng), __stl2::end(rng),
                                                 __stl2::forward<O>(result));
   }

   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_copy_n [Extension]
   //
   template <class I, class O>
   requires
      models::InputIterator<__f<I>> &&
      models::ForwardIterator<__f<O>> &&
      models::Constructible<value_type_t<__f<O>>, reference_t<__f<I>>> &&
      models::Same<value_type_t<__f<O>>&, reference_t<__f<O>>>
   tagged_pair<tag::in(__f<I>), tag::out(__f<O>)>
   uninitialized_copy_n(I&& first, difference_type_t<__f<I>> n, O&& out)
   {
      auto counted = __stl2::make_counted_iterator(__stl2::forward<I>(first), n);
      auto result = uninitialized_copy(__stl2::move(counted),
         default_sentinel{}, __stl2::forward<O>(out));
      return {result.in().base(), result.out()};
   }
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_ALGORITHM_UNINITIALIZED_COPY_HPP
