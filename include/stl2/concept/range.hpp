#ifndef STL2_CONCEPT_RANGE_HPP
#define STL2_CONCEPT_RANGE_HPP

#include <stl2/detail/config.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/concept/iterator.hpp>

#include <iterator>

/////////////////
// Range concepts
//
namespace stl2 { namespace v1 {

namespace detail {
using std::begin;
auto adl_begin(auto&& t) ->
  decltype(begin((decltype(t)&&)t)) {
    return begin((decltype(t)&&)t);
}

using std::end;
auto adl_end(auto&& t) ->
  decltype(end((decltype(t)&&)t)) {
    return end((decltype(t)&&)t);
}
} // namespace detail

template <class T>
using IteratorType = decltype(detail::adl_begin(declval<T>()));
template <class T>
using SentinelType = decltype(detail::adl_end(declval<T>()));

template <class T>
concept bool Range =
  requires(T&& t) {
    detail::adl_begin(t);
    detail::adl_end(t);
    typename IteratorType<T>;
    typename SentinelType<T>;
    requires Sentinel<SentinelType<T>, IteratorType<T>>;
  };

}} // namespace stl2::v1

#endif // STL2_CONCEPT_RANGE_HPP
