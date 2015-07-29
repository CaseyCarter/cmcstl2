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
  return
    requires {
      typename SentinelType<T>;
      typename IteratorType<T>;
    } &&
    Sentinel<SentinelType<T>, IteratorType<T>>();
}

struct view_base {};

template <class T>
concept bool _Unqual = Same<T, detail::uncvref_t<T>>();

template <class T>
concept bool _SizedRangeLike =
  Range<T>() &&
  requires (const T& t) {
    STL2_DEDUCTION_CONSTRAINT(size(t), Integral);
    STL2_CONVERSION_CONSTRAINT(size(t), DifferenceType<IteratorType<T&>>);
  };

template <_SizedRangeLike T>
struct is_sized_range : is_sized_range<detail::uncvref_t<T>> {};

template <class T>
  requires _SizedRangeLike<T&> && std::is_array<T>::value
struct is_sized_range<T&> : std::true_type {};

template <_SizedRangeLike T>
  requires _Unqual<T>
struct is_sized_range<T> : std::true_type {};

template <class T>
concept bool SizedRange() {
  return Range<T>() &&
    _SizedRangeLike<T> &&
    is_sized_range<T>::value;
}

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
_SizedRangeLike{T}
constexpr bool sized_range_like() { return true; }

template <class>
constexpr bool container_like() { return false; }
_ContainerLike{T}
constexpr bool container_like() { return true; }

template <class>
constexpr bool view() { return false; }
View{T}
constexpr bool view() { return true; }

}}}} // namespace stl2::v1::ext::models

#endif
