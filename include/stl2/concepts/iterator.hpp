#ifndef STL2_CONCEPTS_ITERATOR_HPP
#define STL2_CONCEPTS_ITERATOR_HPP

#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/compare.hpp>
#include <stl2/concepts/core.hpp>
#include <stl2/concepts/object.hpp>
#include <stl2/utility.hpp>

////////////////////////////////////////////
// Iterator concepts [iterator.requirements]
//
namespace stl2 { inline namespace v1 {

namespace detail {

template <class T>
concept bool Dereferencable =
  requires (T& t) {
    { *t } -> auto&&;
  };
}

template <detail::Dereferencable R>
using ReferenceType =
  decltype(*declval<R&>());

namespace detail {

template <detail::Dereferencable R>
using __iter_move_t =
  meta::if_<
    std::is_reference<ReferenceType<R>>,
    std::remove_reference_t<ReferenceType<R>> &&,
    std::decay_t<ReferenceType<R>>>;

}

template <detail::Dereferencable R>
detail::__iter_move_t<R> iter_move2(R& r)
  noexcept(noexcept(detail::__iter_move_t<R>(stl2::move(*r))));

template <detail::Dereferencable R>
  requires requires (R& r) {
    { iter_move2(r) } -> auto&&;
  }
using RvalueReferenceType =
  decltype(iter_move2(declval<R&>()));

namespace detail {

template <class T>
concept bool NonVoid =
  !std::is_void<T>::value;

template <class T>
concept bool IsValueType =
  Same<T, std::decay_t<T>>() && NonVoid<T>;

template <class T>
concept bool MemberValueType =
  requires { typename T::value_type; };

template <class T>
concept bool MemberElementType =
  requires { typename T::element_type; };

} // namespace detail

template <class>
struct value_type {};

template <detail::MemberValueType T>
struct value_type<T> {
  using type = typename T::value_type;
};

template <detail::MemberElementType T>
  requires !detail::MemberValueType<T>
struct value_type<T> {
  using type = typename T::element_type;
};

template <detail::Dereferencable T>
  requires !(detail::MemberElementType<T> || detail::MemberValueType<T>)
struct value_type<T> {
  using type = std::decay_t<ReferenceType<T>>;
};

template <class T>
  requires detail::IsValueType<meta::_t<value_type<std::remove_cv_t<T>>>>
using ValueType =
  meta::_t<value_type<std::remove_cv_t<T>>>;

// 20150714: Not to spec, relax Semiregular<I> to
// Movable<I> && DefaultConstructible<I> as in
// https://github.com/ericniebler/range-v3/issues/179.
template <class I>
concept bool Readable() {
  return Movable<I>() &&
    DefaultConstructible<I>() &&
    detail::Dereferencable<const I> &&
    requires (const I& i) {
      typename ValueType<I>;
      typename RvalueReferenceType<I>;
      { *i } -> const ValueType<I>&; // Convertible<ReferenceType<I>, const ValueType<I>&>() ?
    };
}

// 20150714: Not to spec, relax Semiregular<I> to
// Movable<I> && DefaultConstructible<I> as in
// https://github.com/ericniebler/range-v3/issues/179.
template <class Out, class T>
concept bool MoveWritable() {
  return Movable<Out>() &&
    DefaultConstructible<Out>() &&
    detail::Dereferencable<Out> &&
    requires (Out& o, T&& t) {
      *o = stl2::move(t); // not equality preserving
    };
}

namespace detail {
  template <class In, class Out>
  concept bool IndirectlyMovable =
    Readable<In>() &&
    Constructible<ValueType<In>, RvalueReferenceType<In>>() &&
    MoveWritable<Out, RvalueReferenceType<In>>() &&
    MoveWritable<Out, ValueType<In>>();

  template <class In, class Out>
  constexpr bool is_nothrow_indirectly_movable_v = false;

  IndirectlyMovable{In, Out}
  constexpr bool is_nothrow_indirectly_movable_v =
    std::is_nothrow_constructible<ValueType<In>, RvalueReferenceType<In>>::value &&
    std::is_nothrow_assignable<ReferenceType<Out>, RvalueReferenceType<In>>::value &&
    std::is_nothrow_assignable<ReferenceType<Out>, ValueType<In>>::value;

  template <class In, class Out>
  using is_nothrow_indirectly_movable_t =
    meta::bool_<is_nothrow_indirectly_movable_v<In, Out>>;

  template <class In, class Out>
  struct is_nothrow_indirectly_movable
    : is_nothrow_indirectly_movable_t<In, Out> {};
}

template <class Out, class T>
concept bool Writable() {
  return MoveWritable<Out, T>() &&
    requires (Out& o, const T& t) {
      *o = t; // not equality preserving
    };
}

template <class I1, class I2 = I1>
concept bool IndirectlySwappable() {
  return Readable<I1> &&
    Readable<I2> &&
    Swappable<ReferenceType<I1>, ReferenceType<I2>>();
}

namespace detail {

template <class T>
concept bool MemberDifferenceType =
  requires { typename T::difference_type; };

}

template <class> struct difference_type {};

template <> struct difference_type<void*> {};

template <> struct difference_type<std::nullptr_t> {
  using type = std::ptrdiff_t;
};

template <detail::MemberDifferenceType T>
struct difference_type<T> {
  using type = typename T::difference_type;
};

template <class T>
  requires !detail::MemberDifferenceType<T> &&
    requires (const T& a, const T& b) {
      a - b;
    }
struct difference_type<T> :
  std::decay<decltype(declval<const T>() - declval<const T>())> {};

template <class T>
  requires SignedIntegral<meta::_t<difference_type<T>>>()
using DifferenceType =
  meta::_t<difference_type<T>>;

template <class I>
concept bool WeaklyIncrementable() {
  return Semiregular<I>() &&
    requires (I& i) {
      typename DifferenceType<I>;
      STL2_EXACT_TYPE_CONSTRAINT(++i, I&); // not equality preserving
      i++; // not equality preserving
    };
}

template <class I>
concept bool Incrementable() {
  return WeaklyIncrementable<I>() &&
    Regular<I>() &&
    requires (I& i) {
      STL2_EXACT_TYPE_CONSTRAINT(i++, I);
    };
}

struct weak_input_iterator_tag {};
struct input_iterator_tag :
  weak_input_iterator_tag {};
struct forward_iterator_tag :
  input_iterator_tag {};
struct bidirectional_iterator_tag :
  forward_iterator_tag {};
struct random_access_iterator_tag :
  bidirectional_iterator_tag {};

namespace ext {
struct contiguous_iterator_tag :
  random_access_iterator_tag {};
}

template <class> struct iterator_category {};

template <detail::NonVoid T>
struct iterator_category<T*> {
  using type = ext::contiguous_iterator_tag;
};

template <class T>
  requires requires { typename T::iterator_category; }
struct iterator_category<T> {
  using type = typename T::iterator_category;
};

template <class T>
using IteratorCategory =
  meta::_t<iterator_category<T>>;

template <class I>
concept bool WeakIterator() {
  return WeaklyIncrementable<I>() &&
    detail::Dereferencable<const I&>;
}

template <class I>
concept bool Iterator() {
  return WeakIterator<I>() &&
    EqualityComparable<I>();
}

template <class S, class I>
concept bool Sentinel() {
  return Iterator<I>() &&
    Regular<S>() &&
    EqualityComparable<I, S>();
}

template <class I>
concept bool WeakInputIterator() {
  return WeakIterator<I>() &&
    Readable<I>() &&
    requires (I i) {
      typename IteratorCategory<I>;
      Derived<IteratorCategory<I>, weak_input_iterator_tag>();
      STL2_DEDUCTION_CONSTRAINT(i++, Readable);
      requires Same<ValueType<I>, ValueType<decltype(i++)>>();
    };
}

template <class I, class T>
concept bool WeakOutputIterator() {
  return WeakIterator<I>() &&
    Writable<I, T>();
}

template <class I>
concept bool InputIterator() {
  return WeakInputIterator<I>() &&
    Iterator<I>() &&
    Derived<IteratorCategory<I>, input_iterator_tag>();
}

template <class I, class T>
concept bool OutputIterator() {
  return WeakOutputIterator<I, T>() &&
    Iterator<I>();
}

template <class I>
concept bool ForwardIterator() {
  return InputIterator<I>() &&
    Incrementable<I>() &&
    Derived<IteratorCategory<I>, forward_iterator_tag>();
}

template <class I>
concept bool BidirectionalIterator() {
  return ForwardIterator<I>() &&
    Derived<IteratorCategory<I>, bidirectional_iterator_tag>() &&
    requires (I i) {
      STL2_EXACT_TYPE_CONSTRAINT(--i, I&);
      STL2_EXACT_TYPE_CONSTRAINT(i--, I);
    };
}

template <class I, class S = I>
concept bool SizedIteratorRange() {
  return Sentinel<S, I>() &&
    requires (I i, S s) {
      { i - i } -> DifferenceType<I>;
      { s - s } -> DifferenceType<I>;
      { s - i } -> DifferenceType<I>;
      { i - s } -> DifferenceType<I>;
    };
}

template <class I>
concept bool RandomAccessIterator() {
  return BidirectionalIterator<I>() &&
    Derived<IteratorCategory<I>, random_access_iterator_tag>() &&
    TotallyOrdered<I>() &&
    SizedIteratorRange<I>() &&
    requires (I i, const I j, DifferenceType<I> n) {
      STL2_EXACT_TYPE_CONSTRAINT(i += n, I&);
      STL2_EXACT_TYPE_CONSTRAINT(j + n, I);
      STL2_EXACT_TYPE_CONSTRAINT(n + j, I);
      STL2_EXACT_TYPE_CONSTRAINT(i -= n, I&);
      STL2_EXACT_TYPE_CONSTRAINT(j - n, I);
      STL2_EXACT_TYPE_CONSTRAINT(j[n], ReferenceType<I>);
    };
}

namespace ext {

template <class I>
concept bool ContiguousIterator() {
  return RandomAccessIterator<I>() &&
    Derived<IteratorCategory<I>, contiguous_iterator_tag>() &&
    std::is_reference<ReferenceType<I>>::value; // or something
}

namespace models {

template <class>
constexpr bool readable() { return false; }
Readable{R}
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
WeaklyIncrementable{I}
constexpr bool weakly_incrementable() { return true; }

template <class>
constexpr bool incrementable() { return false; }
Incrementable{I}
constexpr bool incrementable() { return true; }

template <class>
constexpr bool weak_iterator() { return false; }
WeakIterator{I}
constexpr bool weak_iterator() { return true; }

#if 0
template <class>
constexpr bool input_iterator() { return false; }
InputIterator{I}
constexpr bool input_iterator() { return true; }
#endif

template <class>
constexpr bool iterator() { return false; }
Iterator{I}
constexpr bool iterator() { return true; }

#if 0 // FIXME: explodes memory
template <class, class>
constexpr bool sentinel() { return false; }
Sentinel{S, I}
constexpr bool sentinel() { return true; }
#endif

}} // namespace ext::models

}} // namespace stl2::v1

#endif // STL2_CONCEPTS_ITERATOR_HPP
