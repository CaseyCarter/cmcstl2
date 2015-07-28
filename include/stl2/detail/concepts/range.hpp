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
concept bool _SizedRangeLike =
  Range<T&>() &&
  requires (T& t) {
    size(t);
    STL2_DEDUCTION_CONSTRAINT(size(t), Integral);
    STL2_CONVERSION_CONSTRAINT(size(t), DifferenceType<IteratorType<T&>>);
  };
template <class T>
concept bool _Unqual = Same<T, uncvref_t<T>>();
}

template <detail::_SizedRangeLike T>
struct is_sized_range : is_sized_range<detail::uncvref_t<T>> {};

template <detail::_SizedRangeLike T>
  requires detail::_Unqual<T>
struct is_sized_range<T> : std::true_type {};

template <class T>
concept bool SizedRange() {
  return detail::_SizedRangeLike<T>
    && meta::_v<is_sized_range<T>>;
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
detail::_SizedRangeLike{T}
constexpr bool sized_range_like() { return true; }

}}}} // namespace stl2::v1::ext::models

#endif
