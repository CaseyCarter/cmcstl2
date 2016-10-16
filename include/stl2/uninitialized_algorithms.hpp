#ifndef STL2_MEMORY_ALGORITHMS_HPP_INCLUDED
#define STL2_MEMORY_ALGORITHMS_HPP_INCLUDED
#include <new>
#include <stl2/iterator.hpp>
#include <stl2/detail/uninitialized_algorithms.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>

STL2_OPEN_NAMESPACE {
   ///////////////////////////////////////////////////////////////////////////
   // destroy_at [Extension]
   //
   template <Destructible T>
   inline void destroy_at(T* p) noexcept
   {
      detail::destroy_at(p);
   }

   ///////////////////////////////////////////////////////////////////////////
   // destroy [Extension]
   //
   template <ForwardIterator I, Sentinel<I> S>
   requires
      models::Destructible<reference_t<I>>
   inline void destroy(I&& first, S&& last) noexcept
   {
      detail::destroy(__stl2::forward<I>(first), __stl2::forward<S>(last));
   }

   template <ForwardRange Rng>
   requires
      models::Destructible<reference_t<Rng>>
   inline void
   destroy(Rng&& rng) noexcept
   {
      __stl2::destroy(__stl2::begin(rng), __stl2::end(rng));
   }

   ///////////////////////////////////////////////////////////////////////////
   // destroy_n [Extension]
   //
   template <ForwardIterator I, Integral Size>
   requires
      models::Destructible<reference_t<I>>
   inline __f<I>
   destroy_n(I&& first, Size size) noexcept
   {
      auto counted = __stl2::make_counted_iterator(__stl2::forward<I>(first), size);
      return detail::destroy(__stl2::move(counted), default_sentinel{});
   }

   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_copy [Extension]
   //
   template <InputIterator I, Sentinel<I> S, ForwardIterator O>
   requires
      models::Constructible<value_type_t<O>, reference_t<I>> &&
      models::Same<value_type_t<O>&, reference_t<O>>
   inline tagged_pair<tag::in(I), tag::out(O)>
   uninitialized_copy(I first, S last, O result)
   {
      using T = value_type_t<O>;
      auto saved = result;
      try {
         for (; first != last; ++first, void(++result)) {
            ::new(static_cast<void*>(&*result)) T(*first);
         }
      } catch(...) {
         destroy(saved, result);
         throw;
      }
      return {__stl2::move(first), __stl2::move(result)};
   }

   template <InputRange Rng, class O>
   requires
      models::ForwardIterator<__f<O>> &&
      models::Constructible<value_type_t<__f<O>>, reference_t<iterator_t<Rng>>> &&
      models::Same<value_type_t<__f<O>>&, reference_t<__f<O>>>
   inline tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
   uninitialized_copy(Rng&& rng, O&& result)
   {
      return __stl2::uninitialized_copy(
         __stl2::begin(rng), __stl2::end(rng), __stl2::forward<O>(result));
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
   inline tagged_pair<tag::in(__f<I>), tag::out(__f<O>)>
   uninitialized_copy_n(I&& first, difference_type_t<__f<I>> n, O&& out)
   {
      auto counted = __stl2::make_counted_iterator(__stl2::forward<I>(first), n);
      auto result = uninitialized_copy(__stl2::move(counted),
         default_sentinel{}, __stl2::forward<O>(out));
      return {result.in().base(), result.out()};
   }

   ///////////////////////////////////////////////////////////////////////////
   // uninitialized_fill [Extension]
   //
   template <ForwardIterator I, Sentinel<I> S, CopyConstructible T>
   requires
      models::Constructible<value_type_t<T>, reference_t<I>> &&
      models::Same<value_type_t<T>&, reference_t<T>>
   inline void uninitialized_fill(I&& first, S&& last, const T& value)
   {
      detail::uninitialized_fill(__stl2::forward<I>(first), __stl2::forward<S>(last), value);
   }

   template <ForwardRange Rng, CopyConstructible T>
   requires
      models::Constructible<value_type_t<__f<T>>, reference_t<iterator_t<Rng>>> &&
      models::Same<value_type_t<T>&, reference_t<T>>
   inline void uninitialized_fill(Rng&& rng, const T& value)
   {
      __stl2::uninitialized_fill(
         __stl2::begin(rng), __stl2::end(rng), value);
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
      return detail::uninitialized_fill(__stl2::move(counted), default_sentinel{}, value);
   }
} STL2_CLOSE_NAMESPACE

#endif // STL2_MEMORY_ALGORITHMS_HPP_INCLUDED
