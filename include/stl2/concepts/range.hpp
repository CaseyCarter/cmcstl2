#ifndef STL2_CONCEPTS_RANGE_HPP
#define STL2_CONCEPTS_RANGE_HPP

#include <stl2/detail/config.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/iterator.hpp>

#include <iterator>

/////////////////
// Range concepts
//
namespace stl2 { inline namespace v1 {

using std::begin;
using std::end;

namespace concepts {

template <class T>
using IteratorType = decltype(begin(declval<T>()));
template <class T>
using SentinelType = decltype(end(declval<T>()));

template <class T>
concept bool Range =
  requires(T&& t) {
    begin(t);
    end(t);
    typename IteratorType<T>;
    typename SentinelType<T>;
    requires Sentinel<SentinelType<T>, IteratorType<T>>;
  };

namespace models {

template <class>
constexpr bool range() { return false; }

Range{T}
constexpr bool range() { return true; }

}}}} // namespace stl2::v1::concepts::models

#endif // STL2_CONCEPTS_RANGE_HPP
