#ifndef STL2_DETAIL_TUPLE_LIKE_HPP
#define STL2_DETAIL_TUPLE_LIKE_HPP

#include <tuple>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
  using std::tuple_size;
  using std::tuple_element;
  using std::get;
  
  constexpr std::size_t tuple_not_found = -1;
  static_assert(tuple_not_found == meta::_v<meta::npos>);
  
  template <class T, class U>
  constexpr std::size_t tuple_find = tuple_not_found;
  
  template <class T, class U>
    requires !Same<U, __uncvref<U>>()
  constexpr std::size_t tuple_find<T, U> =
    tuple_find<T, __uncvref<U>>;
} STL2_CLOSE_NAMESPACE

#endif
