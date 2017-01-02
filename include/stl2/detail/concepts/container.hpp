// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_CONCEPTS_CONTAINER_HPP
#define STL2_DETAIL_CONCEPTS_CONTAINER_HPP

#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/iterator.hpp>

STL2_OPEN_NAMESPACE {
   namespace ext {
      template <class T>
      using const_iterator_t = decltype(ranges::begin(declval<const T&>()));

      template <class T>
      using const_sentinel_t = decltype(ranges::end(declval<const T&>()));

      template <class T>
      using size_type_t = typename T::size_type;

      template <class T>
      using allocator_type_t = typename T::allocator_type;
   } // namespace ext

   namespace ext {
      // stricter than a C++17 Container since this requires
      // both MoveAssignable _and_ CopyAssignable be true
      template <class C>
      concept bool Container() {
         return Regular<C>() &&
            //CopyInsertable<C, value_type_t<C>, typename C::allocator_type>() &&
            Same<typename C::reference, value_type_t<C>&>() &&
            Same<typename C::const_reference, const typename C::value_type&>() &&
            Same<const_iterator_t<C>, iterator_t<const C>>() &&
            Same<const_sentinel_t<C>, sentinel_t<const C>>() &&
            UnsignedIntegral<size_type_t<C>>() && 
            SignedIntegral<difference_type_t<C>>() &&
            sizeof(size_type_t<C>) >= sizeof(difference_type_t<C>) &&
            ForwardIterator<iterator_t<C>>() &&
            ForwardRange<C>() &&
            Constructible<const_iterator_t<C>, iterator_t<C>>() &&
            requires(C a, C b, const C& c) {
               typename value_type_t<C>;
               typename C::reference;
               typename C::const_reference;
               typename iterator_t<C>;
               typename sentinel_t<C>;
               typename const_iterator_t<C>;
               typename const_sentinel_t<C>;
               typename difference_type_t<C>;
               typename size_type_t<C>;
               {a.begin()} -> iterator_t<C>;
               {a.end()} -> sentinel_t<C>;
               {a.cbegin()} -> const_iterator_t<C>;
               {a.cend()} -> const_sentinel_t<C>;
               {c.begin()} -> const_sentinel_t<C>;
               {c.end()} -> const_sentinel_t<C>;
               {a.swap(b)};
               {swap(a, b)};
               {a.size()} -> size_type_t<C>;
               {a.max_size()} -> size_type_t<C>;
               {a.empty()} -> Boolean;
         };
      }
   } // namespace ext

   namespace models {
      template <class C>
      constexpr bool Container = false;
      __stl2::ext::Container{C}
      constexpr bool Container<C> = true;
   }
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_CONCEPTS_CONTAINER_HPP
