#ifndef STL2_UTILITY_HPP
#define STL2_UTILITY_HPP

#include <utility>

#include <stl2/functional.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/swap.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/tuple_like.hpp>

namespace stl2 { inline namespace v1 {
using std::initializer_list;

namespace rel_ops = std::rel_ops;

using std::forward;
using std::move;
using std::move_if_noexcept;
using std::declval;

using std::pair;
using std::make_pair;
using std::piecewise_construct_t;
using std::tuple_size;
using std::tuple_element;
using std::get;

using std::integer_sequence;
using std::index_sequence;
using std::make_integer_sequence;
using std::make_index_sequence;
using std::index_sequence_for;

template <class T, class T1, class T2>
constexpr std::size_t tuple_find<T, pair<T1, T2>> =
  meta::_v<meta::find_index<meta::list<T1, T2>, T>>;

// tagged_pair
template <TaggedType F, TaggedType S>
using tagged_pair =
  tagged<pair<__tag_elem<F>, __tag_elem<S>>,
    __tag_spec<F>, __tag_spec<S>>;

// make_tagged_pair
template <TagSpecifier Tag1, TagSpecifier Tag2, class T1, class T2>
constexpr tagged_pair<Tag1(__unwrap<T1>), Tag2(__unwrap<T2>)>
make_tagged_pair(T1&& x, T2&& y) {
  return {stl2::forward<T1>(x), stl2::forward<T2>(y)};
}
}} // namespace stl2::v1

#endif
