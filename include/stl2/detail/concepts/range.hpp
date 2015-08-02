#ifndef STL2_DETAIL_CONCEPTS_RANGE_HPP
#define STL2_DETAIL_CONCEPTS_RANGE_HPP

#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/iterator.hpp>

///////////////////////////////////////////////////////////////////////////
// Range concepts [iterables]
//
namespace stl2 { inline namespace v1 {

///////////////////////////////////////////////////////////////////////////
// Range
//
#ifndef STL2_STRICT_RANGE
#define STL2_STRICT_RANGE 0
#endif
#ifndef STL2_STRICT_VIEW
#define STL2_STRICT_VIEW 0
#endif

template <class T>
  requires Iterator<decltype(begin(declval<T&>()))>()
using IteratorType = decltype(begin(declval<T&>()));

template <class T>
  requires Sentinel<decltype(end(declval<T&>())), IteratorType<T>>()
using SentinelType = decltype(end(declval<T&>()));

template <class T>
concept bool Range() {
  return
#if STL2_STRICT_RANGE
    _IsNot<T, std::is_reference> &&
#endif
    requires { typename SentinelType<T>; };
}

// 20150729: Extension: DifferenceType<Range>.
template <Range T>
struct difference_type<T> {
  using type = DifferenceType<IteratorType<T>>;
};

template <class T>
concept bool _Unqual = Same<T, detail::uncvref_t<T>>();

///////////////////////////////////////////////////////////////////////////
// SizedRange
//
template <class T>
struct disable_sized_range :
  disable_sized_range<detail::uncvref_t<T>> {};

template <_Unqual T>
struct disable_sized_range<T> :
  std::false_type {};

template <class R>
concept bool SizedRange() {
  return _IsNot<R, disable_sized_range> &&
    Range<R>() &&
#if STL2_STRICT_RANGE
    requires (const R& r) {
#else
    requires (const std::remove_reference_t<R>& r) {
#endif
      STL2_DEDUCTION_CONSTRAINT(size(r), Integral);
      STL2_CONVERSION_CONSTRAINT(size(r), DifferenceType<IteratorType<R>>);
    };
}

///////////////////////////////////////////////////////////////////////////
// View
//
struct view_base {};

template <class T>
concept bool _ContainerLike =
  Range<T>() && Range<const T>() &&
  !Same<ReferenceType<IteratorType<T>>,
        ReferenceType<IteratorType<const T>>>();

template <class T>
struct enable_view {};

template <class T>
struct __view_predicate :
  std::true_type {};

template <class T>
  requires _Valid<meta::_t, enable_view<T>>
struct __view_predicate<T> :
  enable_view<T> {};

template <_ContainerLike T>
  requires !(DerivedFrom<T, view_base>() ||
             _Valid<meta::_t, enable_view<T>>)
struct __view_predicate<T> :
   std::false_type {};

template <class T>
concept bool View() {
  return Range<T>() &&
#if STL2_STRICT_RANGE || STL2_STRICT_VIEW
    Semiregular<T>() &&
    __view_predicate<T>::value;
#else
    Semiregular<std::remove_reference_t<T>>() &&
    __view_predicate<std::remove_reference_t<T>>::value;
#endif
}

///////////////////////////////////////////////////////////////////////////
// BoundedRange
//
template <class T>
concept bool BoundedRange() {
  return Range<T>() && Same<IteratorType<T>, SentinelType<T>>(); 
}

///////////////////////////////////////////////////////////////////////////
// Ranges with various IteratorType
//
namespace ext {
template <class T, class U>
concept bool OutputRange() {
  return Range<T>() && OutputIterator<IteratorType<T>, U>();
}
}

template <class T>
concept bool InputRange() {
  return Range<T>() && InputIterator<IteratorType<T>>();
}

template <class T>
concept bool ForwardRange() {
  return Range<T>() && ForwardIterator<IteratorType<T>>();
}

template <class T>
concept bool BidirectionalRange() {
  return Range<T>() && BidirectionalIterator<IteratorType<T>>();
}

template <class T>
concept bool RandomAccessRange() {
  return Range<T>() && RandomAccessIterator<IteratorType<T>>();
}

namespace ext {
template <class T>
concept bool ContiguousRange() {
  return Range<T>() && ContiguousIterator<IteratorType<T>>();
}
}

///////////////////////////////////////////////////////////////////////////
// Test Predicates
//
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
constexpr bool container_like() { return false; }
_ContainerLike{T}
constexpr bool container_like() { return true; }

template <class>
constexpr bool view() { return false; }
View{T}
constexpr bool view() { return true; }

template <class>
constexpr bool bounded_range() { return false; }
BoundedRange{T}
constexpr bool bounded_range() { return true; }

template <class, class>
constexpr bool output_range() { return false; }
OutputRange{T, U}
constexpr bool output_range() { return true; }

template <class>
constexpr bool input_range() { return false; }
InputRange{T}
constexpr bool input_range() { return true; }

template <class>
constexpr bool forward_range() { return false; }
ForwardRange{T}
constexpr bool forward_range() { return true; }

template <class>
constexpr bool bidirectional_range() { return false; }
BidirectionalRange{T}
constexpr bool bidirectional_range() { return true; }

template <class>
constexpr bool random_access_range() { return false; }
RandomAccessRange{T}
constexpr bool random_access_range() { return true; }

template <class>
constexpr bool contiguous_range() { return false; }
ContiguousRange{T}
constexpr bool contiguous_range() { return true; }

}}}} // namespace stl2::v1::ext::models

#endif
