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
    requires (const std::remove_reference_t<R>& r) {
      STL2_DEDUCTION_CONSTRAINT(size(r), Integral);
      STL2_CONVERSION_CONSTRAINT(size(r), DifferenceType<IteratorType<R&>>);
    };
}

///////////////////////////////////////////////////////////////////////////
// View
//
struct view_base {};

// HACKHACK around <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67038> (?)
template <class T>
constexpr bool __container_like() { return false; }
template <Range T>
  requires Range<const T>() &&
    !Same<ReferenceType<IteratorType<T>>,
          ReferenceType<IteratorType<const T>>>()
constexpr bool __container_like() { return true; }
template <class T>
concept bool _ContainerLike = __container_like<T>();

template <Range T>
struct is_view : is_view<detail::uncvref_t<T>> {};

template <Range T>
  requires _Unqual<T>
struct is_view<T> : std::false_type {};

template <Range T>
  requires _Unqual<T> &&
    Semiregular<T>() &&
    (DerivedFrom<T, view_base>() || !_ContainerLike<T>)
struct is_view<T> : std::true_type {};

template <class T>
concept bool View() {
  return Range<T>() &&
    Semiregular<T>() &&
    is_view<T>::value;
}

///////////////////////////////////////////////////////////////////////////
// Traits
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

constexpr bool container_like() { return false; }
_ContainerLike{T}
constexpr bool container_like() { return true; }

template <class>
constexpr bool view() { return false; }
View{T}
constexpr bool view() { return true; }

}}}} // namespace stl2::v1::ext::models

#endif
