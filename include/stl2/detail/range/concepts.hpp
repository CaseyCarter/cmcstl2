#ifndef STL2_DETAIL_RANGE_CONCEPTS_HPP
#define STL2_DETAIL_RANGE_CONCEPTS_HPP

#include <initializer_list>
#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>

namespace stl2 { inline namespace v1 {
///////////////////////////////////////////////////////////////////////////
// Range [iterable.iterables]
//
template <class T>
using IteratorType = decltype(stl2::begin(declval<T&>()));

template <class T>
using SentinelType = decltype(stl2::end(declval<T&>()));

template <class T>
concept bool Range() {
  return requires { typename SentinelType<T>; };
}

// 20150729: Extension: DifferenceType<Range>.
template <Range T>
  requires !_Is<T, std::is_array>
struct difference_type<T> {
  using type = DifferenceType<IteratorType<T>>;
};

///////////////////////////////////////////////////////////////////////////
// SizedRange [sized.iterables]
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
    requires (const std::remove_reference_t<R>& r) {
      STL2_DEDUCTION_CONSTRAINT(stl2::size(r), Integral);
      STL2_CONVERSION_CONSTRAINT(stl2::size(r), DifferenceType<IteratorType<R>>);
    };
}

///////////////////////////////////////////////////////////////////////////
// View [range.iterables]
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
    Semiregular<T>() &&
    __view_predicate<T>::value;
}

///////////////////////////////////////////////////////////////////////////
// BoundedRange [bounded.iterables]
//
template <class T>
concept bool BoundedRange() {
  return Range<T>() && Same<IteratorType<T>, SentinelType<T>>();
}

namespace ext {
template <class T, class U>
concept bool OutputRange() {
  return Range<T>() && OutputIterator<IteratorType<T>, U>();
}
}

///////////////////////////////////////////////////////////////////////////
// InputRange [input.iterables]
//
template <class T>
concept bool InputRange() {
  return Range<T>() && InputIterator<IteratorType<T>>();
}

///////////////////////////////////////////////////////////////////////////
// ForwardRange [forward.iterables]
//
template <class T>
concept bool ForwardRange() {
  return Range<T>() && ForwardIterator<IteratorType<T>>();
}

///////////////////////////////////////////////////////////////////////////
// BidirectionalRange [bidirectional.iterables]
//
template <class T>
concept bool BidirectionalRange() {
  return Range<T>() && BidirectionalIterator<IteratorType<T>>();
}

///////////////////////////////////////////////////////////////////////////
// RandomAccessRange [random.access.iterables]
//
template <class T>
concept bool RandomAccessRange() {
  return Range<T>() && RandomAccessIterator<IteratorType<T>>();
}

namespace ext {
template <class T>
concept bool ContiguousRange() {
  return SizedRange<T>() && ContiguousIterator<IteratorType<T>>() &&
    requires (T& t) {
      STL2_EXACT_TYPE_CONSTRAINT(stl2::data(t), std::add_pointer_t<ReferenceType<IteratorType<T>>>);
    };
}
}

///////////////////////////////////////////////////////////////////////////
// Range concept test Predicates
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
