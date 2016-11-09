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
#ifndef STL2_DETAIL_MEMORY_REFERENCE_TO_UNINITIALIZED_COPY_HPP
#define STL2_DETAIL_MEMORY_REFERENCE_TO_UNINITIALIZED_COPY_HPP

#include <stl2/concepts.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>

STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // __ReferenceTo [Exposition]
  //
  template <class I, class T>
  concept bool __ReferenceTo() { // exposition only
    return InputIterator<I>() &&
           is_lvalue_reference_v<reference_t<I>> &&
           Same<remove_cv_t<remove_reference_t<reference_t<I>>>, T>();
  }
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_MEMORY_REFERENCE_TO_UNINITIALIZED_COPY_HPP
