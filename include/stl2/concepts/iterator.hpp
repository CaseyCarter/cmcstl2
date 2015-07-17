#ifndef STL2_CONCEPTS_ITERATOR_HPP
#define STL2_CONCEPTS_ITERATOR_HPP

#include <iterator>
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

// 20150715: Not to spec, forbids void.
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

template <class RR, class R = std::remove_reference_t<RR>>
  requires detail::Dereferencable<R>
detail::__iter_move_t<R> iter_move2(RR& r)
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

// 20150715: Not to spec for various reasons:
// * Resolves the ambiguity for a class with both members value_type
//   and element_type in favor of value_type.
// * Falls back on decay_t<ReferenceType<T>> when neither value_type
//   nor element_type are present.
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

// 20150715: Not to spec.
// * Strips cv-qualifiers before evaluating value_type. Someone out
//   there may think it's a good idea to have T, const T, and volatile T
//   be iterators with differing value types, I think they are insane.
// * Requires ValueType to actually be a value type, i.e., non-void
//   and Same<T, decay_t<T>>. I don't think generic code can reasonably
//   be expected to work with an iterator whose value_type is a function
//   or array.
// The end result is that the logic is mostly in ValueType, since
// value_type is subject to the depradations of user specialization.
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
      { *i } -> const ValueType<I>&; // Convertible<ReferenceType<I>, const ValueType<I>&>() ?
      typename RvalueReferenceType<I>;
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
      *o = (T&&)t; // not equality preserving // Maybe void(*o = (T&&)t) ?
    };
}

template <class Out, class T>
concept bool Writable() {
  return MoveWritable<Out, T>() &&
    requires (Out& o, const T& t) {
      *o = t; // not equality preserving // Maybe void(*o = t) ?
    };
}

// 20150715: Not to spec, additionally requires:
//   ValueType<In> v{iter_move2(i)};
// and
//   *o = iter_move2(i);
// to be valid (with stl2::iter_move2 in scope).
template <class In, class Out>
concept bool IndirectlyMovable() {
  return Readable<In>() &&
    Constructible<ValueType<In>, RvalueReferenceType<In>>() &&
    MoveWritable<Out, RvalueReferenceType<In>>() &&
    MoveWritable<Out, ValueType<In>>();
}

namespace ext {

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
struct is_nothrow_indirectly_movable :
  is_nothrow_indirectly_movable_t<In, Out> {};

}

namespace detail {
// This is a manual expansion of:
//   IndirectlyMovable<I1, I2> && IndirectlyMovable<I2, I1>
// with modifications to keep the compiler from blowing up
// memory when it sees the declaration of iter_swap2.
template <class I1, class I2>
concept bool IterSwappable =
  Readable<I1>() && Readable<I2>() &&
  Constructible<ValueType<I1>, RvalueReferenceType<I1>>() &&
  //Constructible<ValueType<I2>, RvalueReferenceType<I2>>() && // BUG: OOM
  MoveWritable<I1, RvalueReferenceType<I2>>() &&
  MoveWritable<I2, RvalueReferenceType<I1>>() &&
  MoveWritable<I2, ValueType<I1>>() &&
  MoveWritable<I1, ValueType<I2>>();

// Similarly, this is a manual:
//   is_nothrow_indirectly_movable_v<T, U> &&
//   is_nothrow_indirectly_movable_v<U, T>
template <class, class>
constexpr bool is_nothrow_iter_swappable_v = false;

IterSwappable{I1, I2}
constexpr bool is_nothrow_iter_swappable_v =
  std::is_nothrow_constructible<ValueType<I1>, RvalueReferenceType<I1>>::value &&
  std::is_nothrow_constructible<ValueType<I2>, RvalueReferenceType<I2>>::value &&
  std::is_nothrow_assignable<ReferenceType<I1>, RvalueReferenceType<I2>>::value &&
  std::is_nothrow_assignable<ReferenceType<I2>, RvalueReferenceType<I1>>::value &&
  std::is_nothrow_assignable<ReferenceType<I2>, ValueType<I1>>::value &&
  std::is_nothrow_assignable<ReferenceType<I1>, ValueType<I2>>::value;
}

// To spec, but should probably be replaced by detail::IterSwappable
// or IndirectlyMovable<I1, I2>() && IndirectlyMovable<I2, I1>()
template <class I1, class I2>
concept bool IndirectlySwappable() {
  return Readable<I1>() && Readable<I2>() &&
    Swappable<ReferenceType<I1>, ReferenceType<I2>>();
}

namespace detail {

template <class T>
concept bool MemberDifferenceType =
  requires { typename T::difference_type; };

}

// 20150715: Conforming extension: defaults to
// make_unsigned_t<decltype(t - t)> when decltype(t - t) models
// Integral, instead of only when T models Integral.
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
      STL2_DEDUCTION_CONSTRAINT(a - b, Integral);
    }
struct difference_type<T> :
  std::make_signed<
    decltype(declval<const T>() - declval<const T>())> {};

// 20150715: Not to spec.
// * Strips cv-qualifiers before applying difference_type (see
//   ValueType for why)
// * Requires DifferenceType to model SignedIntegral
template <class T>
  requires SignedIntegral<meta::_t<difference_type<std::remove_cv_t<T>>>>()
using DifferenceType =
  meta::_t<difference_type<std::remove_cv_t<T>>>;

// Technically to spec, although the requirement
// SignedIntegral<DifferenceType<I>>() is applied directly
// in DifferenceType.
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

namespace detail {
template <class T>
concept bool MemberIteratorCategory =
  requires { typename T::iterator_category; };

namespace std_iterator_category_ {
  input_iterator_tag f(std::input_iterator_tag*);
  forward_iterator_tag f(std::forward_iterator_tag*);
  bidirectional_iterator_tag f(std::bidirectional_iterator_tag*);
  random_access_iterator_tag f(std::random_access_iterator_tag*);
}

template <class T>
using std_iterator_category =
  decltype(std_iterator_category_::f((T*)nullptr));
}

template <class> struct iterator_category {};

template <detail::NonVoid T>
struct iterator_category<T*> {
  using type = ext::contiguous_iterator_tag;
};

template <detail::MemberIteratorCategory T>
struct iterator_category<T> {
  using type = typename T::iterator_category;
};

template <detail::MemberIteratorCategory T>
  requires Derived<typename T::iterator_category, std::output_iterator_tag>()
struct iterator_category<T> {};

template <detail::MemberIteratorCategory T>
  requires Derived<typename T::iterator_category, std::input_iterator_tag>()
struct iterator_category<T> {
  using type = detail::std_iterator_category<typename T::iterator_category>;
};

template <class T>
using IteratorCategory =
  meta::_t<iterator_category<std::remove_cv_t<T>>>;

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
