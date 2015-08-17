#ifndef STL2_TUPLE_HPP
#define STL2_TUPLE_HPP

#include <tuple>
#include <meta/meta.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>

namespace stl2 { inline namespace v1 {
using std::tuple;
using std::make_tuple;
using std::forward_as_tuple;
using std::tie;
using std::ignore;
using std::tuple_cat;
using std::tuple_size;
using std::tuple_element;
using std::get;
using std::uses_allocator;

// tagged_tuple
template <class...Types>
using tagged_tuple = tagged<
  tuple<meta::_t<__tag_elem<Types>>...>,
  meta::_t<__tag_spec<Types>>...>;

// make_tagged_tuple
template<class...Tags, class...Types,
  class Tuple = tagged_tuple<Tags(__unwrap_t<Types>)...>>
constexpr Tuple
make_tagged_tuple(Types&&...ts) {
  return Tuple{stl2::forward<Types>(ts)...};
}

// Additions for N4542 variant
template <class...> class variant;

static constexpr auto tuple_not_found = meta::npos::value;
template <class T, class U>
class tuple_find :
  tuple_find<T, __uncvref_t<U>> {};

template <class T, _Unqual U>
class tuple_find<T, U> {};

template <class T, class... Types>
class tuple_find<T, tuple<Types...>> :
  meta::find_index<meta::list<Types...>, T> {};

template <class T, class T1, class T2>
class tuple_find<T, pair<T1, T2>> :
  meta::find_index<meta::list<T1, T2>, T> {};

template <class T, class... Types>
class tuple_find<T, variant<Types...>> :
  meta::find_index<meta::list<Types...>, T> {};
}} // namespace stl2::v1

namespace std {
template <class...Types>
struct tuple_size<stl2::variant<Types...>> :
  meta::size_t<sizeof...(Types)> {};

template <size_t I, class...Types>
struct tuple_element<I, stl2::variant<Types...>> :
  meta::at_c<meta::list<Types...>, I> {};
}

#endif
