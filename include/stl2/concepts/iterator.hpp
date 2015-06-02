#ifndef STL2_CONCEPTS_ITERATOR_HPP
#define STL2_CONCEPTS_ITERATOR_HPP

#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/config.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/foundational.hpp>

////////////////////
// Iterator concepts
//
namespace stl2 { namespace v1 {

template <class T>
using ReferenceType =
  decltype(*declval<T>());

namespace detail {

template <class T>
using uncvref =
  std::remove_cv_t<std::remove_reference_t<T>>;

template <class T>
concept bool Void =
  std::is_void<T>::value;

template <class T>
struct nonvoid { using type = T; };

template <Void T>
struct nonvoid<T> {};

template <class T>
using nvuncvref = meta::eval<nonvoid<uncvref<T>>>;

template <class T>
concept bool HasValueType =
  requires { typename T::value_type; };

template <class T>
concept bool HasElementType =
  requires { typename T::element_type; };

template <class T>
concept bool HasReferenceType =
  requires { typename ReferenceType<T>; };

} // namespace detail

template <class>
struct value_type {};

template <detail::HasValueType T>
struct value_type<T> { using type = typename T::value_type; };

template <detail::HasElementType T>
  requires !detail::HasValueType<T>
struct value_type<T> { using type = typename T::element_type; };

template <detail::HasReferenceType T>
  requires !detail::HasElementType<T> && !detail::HasValueType<T>
struct value_type<T> { using type = ReferenceType<T>; };

template <class T>
using ValueType =
  detail::nvuncvref<meta::eval<value_type<T>>>;

namespace concepts {

template <class I>
concept bool Readable =
  Semiregular<I> &&
  requires(I& i) {
    typename ValueType<I>;
    { *i } -> const ValueType<I>&;
  };

template <class Out, class T>
concept bool Writable =
  Semiregular<Out> &&
  requires(Out& o, T&& t) {
    *o = (T&&)t;
  };

template <class I, class Out>
concept bool IndirectlyAssignable =
  Readable<I> &&
  Writable<Out, ReferenceType<I>>;

template <class I1, class I2 = I1>
concept bool IndirectlySwappable =
  Readable<I1> &&
  Readable<I2> &&
  Swappable<ReferenceType<I1>, ReferenceType<I2>>;

} // namespace concepts

namespace detail {

template <class T>
concept bool HasDifferenceType =
  requires { typename T::difference_type; };

} // namespace detail

template <class> struct difference_type {};

template <detail::HasDifferenceType T>
struct difference_type<T> { using type = typename T::difference_type; };

template <class T>
  requires !detail::HasDifferenceType<T> &&
    requires(T& a, T& b) {
      a - b; !detail::Void<decltype(a - b)>;
    }
struct difference_type<T> :
  std::remove_cv<decltype(declval<T>() - declval<T>())> {};

template <>
struct difference_type<std::nullptr_t> {
  using type = std::ptrdiff_t;
};

template <class T>
using DifferenceType =
  detail::nvuncvref<meta::eval<difference_type<T>>>;

namespace detail {

template <class T>
concept bool IntegralDifference =
  requires {
    typename DifferenceType<T>;
    requires Integral<DifferenceType<T>>;
  };

} // namespace detail

template <class> struct distance_type {};
template <detail::IntegralDifference T>
struct distance_type<T> :
  std::make_unsigned<DifferenceType<T>> {};

template <class T>
using DistanceType =
  meta::eval<distance_type<T>>;

namespace concepts {

template <class I>
concept bool WeaklyIncrementable =
  Semiregular<I> &&
  requires(I& i) {
    typename DistanceType<I>;
    requires Integral<DistanceType<I>>; // Try without this?
    ++i; requires Same<I&, decltype(++i)>;
    i++;
  };

template <class I>
concept bool Incrementable =
  WeaklyIncrementable<I> &&
  EqualityComparable<I> &&
  requires(I& i) {
    i++; requires Same<I, decltype(i++)>;
  };

} // namespace concepts

struct weak_input_iterator_tag {};
struct input_iterator_tag :
  weak_input_iterator_tag {};
struct forward_iterator_tag :
  input_iterator_tag {};
struct bidirectional_iterator_tag :
  forward_iterator_tag {};
struct random_access_iterator_tag :
  bidirectional_iterator_tag {};
struct contiguous_iterator_tag :
  random_access_iterator_tag {};

template <class>
struct iterator_category {};
template <class T>
struct iterator_category<T*> {
  using type = contiguous_iterator_tag;
};
template <class T>
  requires requires { typename T::iterator_category; }
struct iterator_category<T> {
  using type = typename T::iterator_category;
};

template <class T>
using IteratorCategory =
  meta::eval<iterator_category<T>>;

namespace concepts {

template <class I>
concept bool WeakIterator =
  WeaklyIncrementable<I> &&
  requires(I& i) {
    //{ *i } -> auto&&;
    *i; requires !detail::Void<decltype(*i)>;
  };

template <class I>
concept bool Iterator =
  WeakIterator<I> &&
  EqualityComparable<I>;

template <class S, class I>
concept bool Sentinel =
  Iterator<I> &&
  Regular<S> &&
  EqualityComparable<I, S>;

template <class I>
concept bool WeakInputIterator =
  WeakIterator<I> &&
  Readable<I> &&
  requires(I i) {
    typename IteratorCategory<I>;
    Derived<IteratorCategory<I>, weak_input_iterator_tag>;
    //{ i++ } -> Readable;
    requires Readable<decltype(i++)>;
    requires Same<ValueType<I>,ValueType<decltype(i++)>>;
  };

template <class I>
concept bool InputIterator =
  WeakInputIterator<I> &&
  Iterator<I> &&
  Derived<IteratorCategory<I>, input_iterator_tag>;

template <class I, class T>
concept bool WeakOutputIterator =
  WeakIterator<I> &&
  Writable<I, T>;

template <class I, class T>
concept bool OutputIterator =
  WeakOutputIterator<I, T> &&
  Iterator<I>;

template <class I>
concept bool ForwardIterator =
  InputIterator<I> &&
  Incrementable<I> &&
  Derived<IteratorCategory<I>, forward_iterator_tag>;

template <class I>
concept bool BidirectionalIterator =
  ForwardIterator<I> &&
  Derived<IteratorCategory<I>, bidirectional_iterator_tag> &&
  requires(I i) {
    --i; requires Same<I&, decltype(--i)>;
    i--; requires Same<I, decltype(i--)>;
  };

template <class I, class S = I>
concept bool SizedIteratorRange =
  Sentinel<S, I> &&
  requires(I i, S s) {
    typename DifferenceType<I>;
    // Common<DifferenceType<I>, DistanceType<I>> ??
    { i - i } -> DifferenceType<I>;
    { s - s } -> DifferenceType<I>;
    { s - i } -> DifferenceType<I>;
    { i - s } -> DifferenceType<I>;
  };

template <class I>
concept bool RandomAccessIterator =
  BidirectionalIterator<I> &&
  Derived<IteratorCategory<I>, random_access_iterator_tag> &&
  TotallyOrdered<I> &&
  SizedIteratorRange<I> &&
  requires(I i, I j, DifferenceType<I> n) {
    i += n; requires Same<I&, decltype(i += n)>;
    i + n; requires Same<I, decltype(i + n)>;
    n + i; requires Same<I, decltype(n + i)>;
    i -= n; requires Same<I&, decltype(i -= n)>;
    i - n; requires Same<I, decltype(i - n)>;
    i[n]; requires Same<ReferenceType<I>,decltype(i[n])>;
  };

template <class I>
concept bool ContiguousIterator =
  RandomAccessIterator<I> &&
  Derived<IteratorCategory<I>, contiguous_iterator_tag> &&
  std::is_reference<ReferenceType<I>>::value;

namespace check {

template <class>
constexpr bool is_readable() { return false; }

template <Readable>
constexpr bool is_readable() { return true; }


template <class>
constexpr bool is_weakly_incrementable() { return false; }

template <WeaklyIncrementable>
constexpr bool is_weakly_incrementable() { return true; }


template <class>
constexpr bool is_incrementable() { return false; }

template <Incrementable>
constexpr bool is_incrementable() { return true; }

}}}} // namespace stl2::v1::concepts::check

#endif // STL2_CONCEPTS_ITERATOR_HPP
