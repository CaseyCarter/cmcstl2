#ifndef STL2_CONCEPTS_ITERATOR_HPP
#define STL2_CONCEPTS_ITERATOR_HPP

#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/foundational.hpp>
#include <stl2/utility.hpp>

////////////////////
// Iterator concepts
//
namespace stl2 { inline namespace v1 {

template <class T>
using ReferenceType =
  decltype(*declval<T>());

namespace detail {
template <class R>
using __iter_move_t =
  meta::if_<
    std::is_reference<ReferenceType<R>>,
    std::remove_reference_t<ReferenceType<R>> &&,
    std::decay_t<ReferenceType<R>>>;
}

template <class T>
detail::__iter_move_t<T> iter_move2(T t)
  noexcept(noexcept(detail::__iter_move_t<T>(stl2::move(*t))));

template <class T>
using RvalueReferenceType = decltype(iter_move2(declval<T&>()));

namespace detail {

using meta::detail::uncvref_t;

template <class>
struct acceptable_value_type {};

template <class T>
  requires std::is_object<T>::value
struct acceptable_value_type<T> {
  using type = T;
};

template <class T>
concept bool Void =
  std::is_void<T>::value;

template <class T>
struct nonvoid { using type = T; };

Void{T}
struct nonvoid<T> {};

template <class T>
using nvuncvref_t = meta::eval<nonvoid<uncvref_t<T>>>;

template <class T>
concept bool HasValueType =
  requires { typename T::value_type; };

template <class T>
concept bool HasElementType =
  requires { typename T::element_type; };

template <class T>
concept bool HasReferenceType =
  requires { typename ReferenceType<T>; };

template <class T>
concept bool HasDifferenceType =
  requires { typename T::difference_type; };

} // namespace detail

template <class>
struct value_type {};

template <detail::HasValueType T>
struct value_type<T> {
  using type = typename T::value_type;
};

template <detail::HasElementType T>
  requires !detail::HasValueType<T>
struct value_type<T> {
  using type = typename T::element_type;
};

template <detail::HasReferenceType T>
  requires !detail::HasElementType<T> && !detail::HasValueType<T>
struct value_type<T> {
  using type = detail::uncvref_t<ReferenceType<T>>;
};

template <class T>
using ValueType =
  meta::eval<detail::acceptable_value_type<
    meta::eval<value_type<std::remove_cv_t<T>>>>>;

template <class I>
concept bool Readable =
  Semiregular<I> &&
  requires(I& i) {
    typename ValueType<I>;
    typename ReferenceType<I>;
    typename RvalueReferenceType<I>;
    { *i } -> const ValueType<I>&;
  };

template <class Out, class T>
concept bool MoveWritable =
  Semiregular<Out> &&
  requires(Out o, T t) {
    typename ReferenceType<Out>;
    *o = stl2::move(t);
  };

template <class Out, class T>
concept bool Writable =
  MoveWritable<Out, T> &&
  requires(Out o, const T& t) {
    *o = t;
  };

namespace detail {
  template <class In, class Out>
  concept bool IndirectlyMovable =
    Readable<In> &&
    Constructible<ValueType<In>, RvalueReferenceType<In>>() &&
    MoveWritable<Out, RvalueReferenceType<In>> &&
    MoveWritable<Out, ValueType<In>>;

  template <class In, class Out>
  constexpr bool is_nothrow_indirectly_movable_v = false;

  IndirectlyMovable{In, Out}
  constexpr bool is_nothrow_indirectly_movable_v =
    std::is_nothrow_constructible<ValueType<In>, RvalueReferenceType<In>>::value &&
    std::is_nothrow_assignable<ReferenceType<Out>, RvalueReferenceType<In>>::value &&
    std::is_nothrow_assignable<ReferenceType<Out>, ValueType<In>>::value;

  template <class In, class Out>
  struct is_nothrow_indirectly_movable
    : meta::bool_<is_nothrow_indirectly_movable_v<In, Out>> {};

  template <class In, class Out>
  using is_nothrow_indirectly_movable_t = meta::_t<is_nothrow_indirectly_movable<In, Out>>;
}

// iter_swap2
template <Readable R1, Readable R2>
  requires Swappable<ReferenceType<R1>, ReferenceType<R2>>
void iter_swap2(R1 r1, R2 r2)
  noexcept(is_nothrow_swappable_v<ReferenceType<R1>, ReferenceType<R2>>);

template <Readable R1, Readable R2>
  requires !Swappable<ReferenceType<R1>, ReferenceType<R2>> &&
    detail::IndirectlyMovable<R1, R2> && detail::IndirectlyMovable<R2, R1>
void iter_swap2(R1 r1, R2 r2)
  noexcept(detail::is_nothrow_indirectly_movable_v<R1, R2> &&
           detail::is_nothrow_indirectly_movable_v<R2, R1>);

template <class I, class Out>
concept bool IndirectlyAssignable =
  Readable<I> &&
  Writable<Out, ReferenceType<I>>;

template <class I1, class I2 = I1>
concept bool IndirectlySwappable =
  Readable<I1> &&
  Readable<I2> &&
  Swappable<ReferenceType<I1>, ReferenceType<I2>>;

template <class> struct difference_type {};

template <> struct difference_type<void*> {};

template <detail::HasDifferenceType T>
struct difference_type<T> { using type = typename T::difference_type; };

template <class T>
  requires !detail::HasDifferenceType<T> &&
    requires(T& a, T& b) {
      a - b;
      requires !detail::Void<decltype(a - b)>;
    }
struct difference_type<T> :
  std::remove_cv<decltype(declval<T>() - declval<T>())> {};

template <>
struct difference_type<std::nullptr_t> {
  using type = std::ptrdiff_t;
};

template <class T>
using DifferenceType =
  detail::nvuncvref_t<meta::eval<difference_type<T>>>;

namespace detail {

template <class T>
concept bool IntegralDifference =
  requires {
    typename DifferenceType<T>;
    requires Integral<DifferenceType<T>>;
  };

template <class T>
concept bool HasDistanceType =
  requires { typename T::distance_type; };

} // namespace detail

template <class> struct distance_type {};

template <detail::HasDistanceType T>
struct distance_type<T> {
  using type = typename T::distance_type;
};

template <detail::IntegralDifference T>
  requires(!detail::HasDistanceType<T>)
struct distance_type<T> :
  std::make_unsigned<DifferenceType<T>> {};

template <class T>
using DistanceType =
  meta::eval<distance_type<T>>;

template <class I>
concept bool WeaklyIncrementable =
  Semiregular<I> &&
  requires(I& i) {
    typename DistanceType<I>;
    requires Integral<DistanceType<I>>; // Try without this?
    ++i;
    requires Same<I&, decltype(++i)>;
    i++;
  };

template <class I>
concept bool Incrementable =
  WeaklyIncrementable<I> &&
  EqualityComparable<I> &&
  requires(I& i) {
    i++;
    requires Same<I, decltype(i++)>;
  };

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

template <class I>
concept bool WeakIterator =
  WeaklyIncrementable<I> &&
  requires(I& i) {
    //{ *i } -> auto&&;
    *i;
    requires !detail::Void<decltype(*i)>;
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
    requires Same<ValueType<I>, ValueType<decltype(i++)>>;
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

namespace models {

template <class>
constexpr bool readable() { return false; }

Readable{T}
constexpr bool readable() { return true; }


template <class, class>
constexpr bool move_writable() { return false; }

MoveWritable{O, T}
constexpr bool move_writable() { return true; }

template <class, class>
constexpr bool writable() { return false; }

Writable{O, T}
constexpr bool writable() { return true; }


template <class>
constexpr bool weakly_incrementable() { return false; }

WeaklyIncrementable{T}
constexpr bool weakly_incrementable() { return true; }


template <class>
constexpr bool incrementable() { return false; }

Incrementable{T}
constexpr bool incrementable() { return true; }


template <class>
constexpr bool weak_iterator() { return false; }

WeakIterator{T}
constexpr bool weak_iterator() { return true; }


template <class>
constexpr bool iterator() { return false; }

Iterator{T}
constexpr bool iterator() { return true; }

#if 0 // FIXME: explodes memory
template <class, class>
constexpr bool sentinel() { return false; }

Sentinel{S, I}
constexpr bool sentinel() { return true; }
#endif

} // namespace models


template <class R>
detail::__iter_move_t<R> iter_move2(R r)
  noexcept(noexcept(detail::__iter_move_t<R>(stl2::move(*r)))) {
  return stl2::move(*r);
}

// iter_swap2
template <Readable R1, Readable R2>
  requires Swappable<ReferenceType<R1>, ReferenceType<R2>>
void iter_swap2(R1 r1, R2 r2)
  noexcept(is_nothrow_swappable_v<ReferenceType<R1>, ReferenceType<R2>>) {
  swap(*r1, *r2);
}

template <Readable R1, Readable R2>
  requires !Swappable<ReferenceType<R1>, ReferenceType<R2>> &&
    detail::IndirectlyMovable<R1, R2> && detail::IndirectlyMovable<R2, R1>
void iter_swap2(R1 r1, R2 r2)
  noexcept(detail::is_nothrow_indirectly_movable_v<R1, R2> &&
           detail::is_nothrow_indirectly_movable_v<R2, R1>) {
  ValueType<R1> tmp = iter_move2(r1);
  *r1 = iter_move2(r2);
  *r2 = std::move(tmp);
}

}} // namespace stl2::v1

#endif // STL2_CONCEPTS_ITERATOR_HPP
