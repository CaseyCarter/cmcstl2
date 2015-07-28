#ifndef STL2_DETAIL_CONCEPTS_RANGE_HPP
#define STL2_DETAIL_CONCEPTS_RANGE_HPP

#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/iterator.hpp>

/////////////////////////////
// Range concepts [iterables]
//
namespace stl2 { inline namespace v1 {

template <class T>
using IteratorType = decltype(begin(declval<T>()));
template <class T>
using SentinelType = decltype(end(declval<T>()));

template <class T>
concept bool Range() {
  return Sentinel<SentinelType<T>, IteratorType<T>>();
}

namespace detail {
template <class T>
concept bool SizedRangeLike =
  Range<T>() &&
  requires (T& t) {
    STL2_DEDUCTION_CONSTRAINT(size(t), Integral);
    STL2_CONVERSION_CONSTRAINT(size(t), DifferenceType<IteratorType<T>>);
  };  
}

template <class>
struct is_sized_range : std::true_type {};

template <class T>
concept bool SizedRange() {
  return detail::SizedRangeLike<T> &&
    meta::_v<is_sized_range<detail::uncvref_t<T>>>;
}

namespace ext { namespace models {

template <class>
constexpr bool range() { return false; }
Range{T}
constexpr bool range() { return true; }

template <class>
constexpr bool sized_range() { return false; }
SizedRange{T}
constexpr bool sized_range() { return true; }

template <class>
constexpr bool sized_range_like() { return false; }
detail::SizedRangeLike{T}
constexpr bool sized_range_like() { return true; }

}}}} // namespace stl2::v1::ext::models

#endif
