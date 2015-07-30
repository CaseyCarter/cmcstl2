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
  requires Iterator<decltype(begin(declval<T>()))>()
using IteratorType = decltype(begin(declval<T>()));
template <class T>
  requires Sentinel<decltype(end(declval<T>())), IteratorType<T>>()
using SentinelType = decltype(end(declval<T>()));

template <class T>
concept bool Range() {
  return requires { typename SentinelType<T>; };
}

// 20150729: Extension: DifferenceType<Range>.
template <class T>
  requires Range<T&>()
struct difference_type<T> {
  using type = DifferenceType<IteratorType<T&>>;
};

template <class T>
concept bool _SizedRangeLike =
  Range<T&>() &&
  requires (const T& t) {
    STL2_DEDUCTION_CONSTRAINT(size(t), Integral);
    STL2_CONVERSION_CONSTRAINT(size(t), DifferenceType<IteratorType<T&>>);
  };

template <class T>
concept bool _Unqual = Same<T, detail::uncvref_t<T>>();

template <_SizedRangeLike T>
struct is_sized_range : is_sized_range<detail::uncvref_t<T>> {};

template <_SizedRangeLike T>
  requires _Unqual<T>
struct is_sized_range<T> : std::true_type {};

template <class T>
concept bool SizedRange() {
  return Range<T>() &&
    _SizedRangeLike<T> &&
    is_sized_range<T>::value;
}

namespace ext { namespace models {

template <class>
constexpr bool range() { return false; }
Range{T}
constexpr bool range() { return true; }

template <class>
constexpr bool sized_range_like() { return false; }
_SizedRangeLike{T}
constexpr bool sized_range_like() { return true; }

template <class>
constexpr bool sized_range() { return false; }
SizedRange{T}
constexpr bool sized_range() { return true; }

}}}} // namespace stl2::v1::ext::models

#endif
