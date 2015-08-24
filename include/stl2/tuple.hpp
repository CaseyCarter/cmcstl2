#ifndef STL2_TUPLE_HPP
#define STL2_TUPLE_HPP

#include <stl2/functional.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/tuple_like.hpp>

namespace stl2 { inline namespace v1 {
using std::tuple;
using std::make_tuple;
using std::forward_as_tuple;
using std::tie;
using std::ignore;
using std::tuple_cat;

// tuple_find specialization for tuple
template <class T, class... Types>
constexpr std::size_t tuple_find<T, tuple<Types...>> =
  meta::_v<meta::find_index<meta::list<Types...>, T>>;

// tagged_tuple
template <TaggedType...Types>
using tagged_tuple =
  tagged<tuple<__tag_elem<Types>...>, __tag_spec<Types>...>;

// make_tagged_tuple
template<TagSpecifier...Tags, class...Types>
constexpr auto make_tagged_tuple(Types&&...ts) {
  return tagged_tuple<Tags(__unwrap<Types>)...>{
    stl2::forward<Types>(ts)...
  };
}
}} // namespace stl2::v1

#endif
