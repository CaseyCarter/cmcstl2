#ifndef STL2_DETAIL_TUPLE_LIKE_HPP
#define STL2_DETAIL_TUPLE_LIKE_HPP

#include <tuple>
#include <stl2/detail/meta.hpp>

namespace stl2 { inline namespace v1 {
using std::tuple_size;
using std::tuple_element;
using std::get;

constexpr std::size_t tuple_not_found = -1;
static_assert(tuple_not_found == meta::_v<meta::npos>);

template <class T, class U>
constexpr std::size_t tuple_find = tuple_find<T, __uncvref<U>>;

template <class T, _Unqual U>
constexpr std::size_t tuple_find<T, U> = tuple_not_found;
}} // namespace stl2::v1

#endif
