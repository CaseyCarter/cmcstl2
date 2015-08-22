#ifndef STL2_DETAIL_ITERATOR_CONCEPTS_HPP
#define STL2_DETAIL_ITERATOR_CONCEPTS_HPP

#include <functional>
#include <iterator>
#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/concepts/function.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/utility.hpp>

////////////////////////////////////////////
// Iterator concepts [iterator.requirements]
//
namespace stl2 { inline namespace v1 {
namespace detail {
template <class T>
concept bool Dereferenceable =
  requires (T& t) {
    { *t } -> auto&&;
  };
}

// 20150715: Not to spec, forbids void.
template <detail::Dereferenceable R>
using ReferenceType =
  decltype(*declval<R&>());

namespace detail {
template <Dereferenceable R>
using __iter_move_t =
  meta::if_<
    std::is_reference<ReferenceType<R>>,
    std::remove_reference_t<ReferenceType<R>> &&,
    std::decay_t<ReferenceType<R>>>;

namespace __iter_move {
template <class R, Dereferenceable _R = std::remove_reference_t<R>>
__iter_move_t<_R> iter_move(R&& r)
  noexcept(noexcept(__iter_move_t<_R>(stl2::move(*r)))) {
  return stl2::move(*r);
}

struct fn {
  template <class R>
    requires requires (R&& r) { { iter_move((R&&)r) } -> auto&&; }
  constexpr auto operator()(R&& r) const
    noexcept(noexcept(iter_move(stl2::forward<R>(r)))) ->
    decltype(iter_move(stl2::forward<R>(r))) {
    return iter_move(stl2::forward<R>(r));
  }
};
}}
inline namespace {
  constexpr auto& iter_move = detail::static_const<detail::__iter_move::fn>::value;
}

template <detail::Dereferenceable R>
using RvalueReferenceType =
  decltype(iter_move(declval<R&>()));

namespace detail {
template <class T>
concept bool IsValueType =
  Same<T, std::decay_t<T>>() && _IsNot<T, std::is_void>;

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

template <detail::Dereferenceable T>
  requires !(detail::MemberElementType<T> || detail::MemberValueType<T>)
struct value_type<T> {
  using type = std::decay_t<ReferenceType<T>>;
};

// 20150715: Not to spec.
// * Strips cv-qualifiers before evaluating value_type. Someone out
//   there may think it's a good idea to have T, const T, and volatile T
//   be iterators with differing value types - I think they are insane.
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

template <class I>
concept bool Readable() {
  return detail::Dereferenceable<I> &&
    Movable<I>() &&
    DefaultConstructible<I>() &&
    requires (const I& i) {
      // Associated types
      typename ValueType<I>;
      typename ReferenceType<I>;
      typename RvalueReferenceType<I>;

      // Valid expressions
      STL2_EXACT_TYPE_CONSTRAINT(*i, ReferenceType<I>);
      STL2_EXACT_TYPE_CONSTRAINT(iter_move(i), RvalueReferenceType<I>);
    } &&
    // Relationships between associated types
    CommonReference<ReferenceType<I>, ValueType<I>&>() &&
    CommonReference<ReferenceType<I>, RvalueReferenceType<I>>() &&
    CommonReference<RvalueReferenceType<I>, const ValueType<I>&>() &&
    // Extra sanity checks (not strictly needed)
    Same<
      CommonReferenceType<ReferenceType<I>, ValueType<I>>,
      ValueType<I>>() &&
    Same<
      CommonReferenceType<RvalueReferenceType<I>, ValueType<I>>,
      ValueType<I>>();
}

// A generally useful dependent type
template <Readable I>
using iter_common_reference_t =
  common_reference_t<ReferenceType<I>, ValueType<I>&>;

template <class Out, class T>
concept bool MoveWritable() {
  return detail::Dereferenceable<Out> &&
    Movable<Out>() &&
    DefaultConstructible<Out>() &&
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

template <class In, class Out>
concept bool IndirectlyMovable() {
  return Readable<In>() && Movable<ValueType<In>>() &&
    Constructible<ValueType<In>, RvalueReferenceType<In>>() &&
    Assignable<ValueType<In>&, RvalueReferenceType<In>>() &&
    MoveWritable<Out, RvalueReferenceType<In>>() &&
    MoveWritable<Out, ValueType<In>>();
}

template <class In, class Out>
concept bool IndirectlyCopyable() {
  return IndirectlyMovable<In, Out>() && Copyable<ValueType<In>>() &&
    Constructible<ValueType<In>, ReferenceType<In>>() &&
    Assignable<ValueType<In>&, ReferenceType<In>>() &&
    Writable<Out, ReferenceType<In>>() &&
    Writable<Out, ValueType<In>>();
}

template <class In, class Out>
constexpr bool is_nothrow_indirectly_movable_v = false;

IndirectlyMovable{In, Out}
constexpr bool is_nothrow_indirectly_movable_v<In, Out> =
  std::is_nothrow_constructible<ValueType<In>, RvalueReferenceType<In>>::value &&
  std::is_nothrow_assignable<ValueType<In> &, RvalueReferenceType<In>>::value &&
  std::is_nothrow_assignable<ReferenceType<Out>, RvalueReferenceType<In>>::value &&
  std::is_nothrow_assignable<ReferenceType<Out>, ValueType<In>>::value;

template <class In, class Out>
using is_nothrow_indirectly_movable_t =
  meta::bool_<is_nothrow_indirectly_movable_v<In, Out>>;

template <class In, class Out>
struct is_nothrow_indirectly_movable
  : is_nothrow_indirectly_movable_t<In, Out> {};

// iter_swap2
namespace __iter_swap {
template <class R1, class R2,
  Readable _R1 = std::remove_reference_t<R1>,
  Readable _R2 = std::remove_reference_t<R2>>
  requires Swappable<ReferenceType<_R1>, ReferenceType<_R2>>()
void iter_swap2(R1&& r1, R2&& r2)
  noexcept(is_nothrow_swappable_v<ReferenceType<_R1>, ReferenceType<_R2>>) {
  swap(*r1, *r2);
}

template <class R1, class R2,
  Readable _R1 = std::remove_reference_t<R1>,
  Readable _R2 = std::remove_reference_t<R2>>
  requires IndirectlyMovable<_R1, _R2>() && IndirectlyMovable<_R2, _R1>() &&
    !Swappable<ReferenceType<_R1>, ReferenceType<_R2>>()
void iter_swap2(R1&& r1, R2&& r2)
  noexcept(is_nothrow_indirectly_movable_v<_R1, _R2> &&
           is_nothrow_indirectly_movable_v<_R2, _R1>) {
  ValueType<_R1> tmp = iter_move(r1);
  *r1 = iter_move(r2);
  *r2 = stl2::move(tmp);
}

struct fn {
  template <class R1, class R2>
    requires requires (R1&& r1, R1&& r2) {
      iter_swap2((R1&&)r1, (R2&&)r2);
    }
  constexpr void operator()(R1&& r1, R2&& r2) const
    noexcept(noexcept(iter_swap2(stl2::forward<R1>(r1), stl2::forward<R2>(r2)))) {
    iter_swap2(stl2::forward<R1>(r1), stl2::forward<R2>(r2));
  }
};
}
inline namespace {
  constexpr auto& iter_swap2 = detail::static_const<__iter_swap::fn>::value;
}

template <class I1, class I2 = I1>
concept bool IndirectlySwappable() {
  return Readable<I1>() && Readable<I2>() &&
    requires (I1 i1, I2 i2) {
      iter_swap2(i1, i2);
      iter_swap2(i2, i1);
      iter_swap2(i1, i1);
      iter_swap2(i2, i2);
    };
}

template <class R1, class R2>
constexpr bool is_nothrow_indirectly_swappable_v = false;

IndirectlySwappable{R1, R2}
constexpr bool is_nothrow_indirectly_swappable_v<R1, R2> =
  noexcept(iter_swap2(stl2::declval<R1>(), stl2::declval<R2>())) &&
  noexcept(iter_swap2(stl2::declval<R2>(), stl2::declval<R1>())) &&
  noexcept(iter_swap2(stl2::declval<R1>(), stl2::declval<R1>())) &&
  noexcept(iter_swap2(stl2::declval<R2>(), stl2::declval<R2>()));

template <class R1, class R2>
using is_nothrow_indirectly_swappable_t =
  meta::bool_<is_nothrow_indirectly_swappable_v<R1, R2>>;

template <class R1, class R2>
struct is_nothrow_indirectly_swappable
  : is_nothrow_indirectly_swappable_t<R1, R2> {};

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

struct output_iterator_tag {};
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

namespace std_to_stl2_iterator_category_ {
  input_iterator_tag f(std::input_iterator_tag*);
  forward_iterator_tag f(std::forward_iterator_tag*);
  bidirectional_iterator_tag f(std::bidirectional_iterator_tag*);
  random_access_iterator_tag f(std::random_access_iterator_tag*);
}

template <class T>
using std_to_stl2_iterator_category =
  decltype(std_to_stl2_iterator_category_::f((T*)nullptr));
}

// 20150716: Conforming extension: category for pointers is
// ext::contiguous_iterator_tag which derives from
// random_access_iterator_tag.
template <class> struct iterator_category {};

template <_IsNot<std::is_void> T>
struct iterator_category<T*> {
  using type = ext::contiguous_iterator_tag;
};

template <detail::MemberIteratorCategory T>
struct iterator_category<T> {
  using type = typename T::iterator_category;
};

template <detail::MemberIteratorCategory T>
  requires DerivedFrom<typename T::iterator_category, std::output_iterator_tag>()
struct iterator_category<T> {};

template <detail::MemberIteratorCategory T>
  requires DerivedFrom<typename T::iterator_category, std::input_iterator_tag>()
struct iterator_category<T> {
  using type = detail::std_to_stl2_iterator_category<typename T::iterator_category>;
};

template <class T>
using IteratorCategory =
  meta::_t<iterator_category<std::remove_cv_t<T>>>;

template <class I>
concept bool WeakIterator() {
  return WeaklyIncrementable<I>() &&
    detail::Dereferenceable<I&>;
}

template <class I>
concept bool Iterator() {
  return WeakIterator<I>() &&
    Regular<I>();
}

template <class S, class I>
concept bool Sentinel() {
  return Iterator<I>() &&
    Regular<S>() &&
    EqualityComparable<I, S>();
}

template <class I, class T>
concept bool WeakOutputIterator() {
  return WeakIterator<I>() &&
    Writable<I, T>();
}

template <class I, class T>
concept bool OutputIterator() {
  return WeakOutputIterator<I, T>() &&
    Iterator<I>();
}

template <class I>
concept bool WeakInputIterator() {
  return WeakIterator<I>() &&
    Readable<I>() &&
    requires (I& i, const I& ci) {
      typename IteratorCategory<I>;
      DerivedFrom<IteratorCategory<I>, weak_input_iterator_tag>();
      STL2_DEDUCTION_CONSTRAINT(i++, Readable);
      requires Same<ValueType<I>, ValueType<decltype(i++)>>();
      { *ci } -> const ValueType<I>&;
    };
}

template <class I>
concept bool InputIterator() {
  return WeakInputIterator<I>() &&
    Iterator<I>() &&
    DerivedFrom<IteratorCategory<I>, input_iterator_tag>();
}

template <class I>
concept bool ForwardIterator() {
  return InputIterator<I>() &&
    Incrementable<I>() &&
    DerivedFrom<IteratorCategory<I>, forward_iterator_tag>();
}

template <class I>
concept bool BidirectionalIterator() {
  return ForwardIterator<I>() &&
    DerivedFrom<IteratorCategory<I>, bidirectional_iterator_tag>() &&
    requires (I& i) {
      STL2_EXACT_TYPE_CONSTRAINT(--i, I&);
      STL2_EXACT_TYPE_CONSTRAINT(i--, I);
    };
}

template <class I, class S = I>
concept bool SizedIteratorRange() {
  return Sentinel<S, I>() &&
    requires (const I& i, const S& s) {
      { i - i } -> DifferenceType<I>;
      { s - s } -> DifferenceType<I>;
      { s - i } -> DifferenceType<I>;
      { i - s } -> DifferenceType<I>;
    };
}

namespace detail {
template <class I>
concept bool MutableIterator =
  Iterator<I>() &&
  requires (const I& i) {
    { *i } -> auto&;
    *i = *i;
  };
}

template <class I>
concept bool RandomAccessIterator() {
  return BidirectionalIterator<I>() &&
    TotallyOrdered<I>() &&
    DerivedFrom<IteratorCategory<I>, random_access_iterator_tag>() &&
    SizedIteratorRange<I>() &&
    requires (I& i, const I& j, const DifferenceType<I> n) {
      STL2_EXACT_TYPE_CONSTRAINT(i += n, I&);
      STL2_EXACT_TYPE_CONSTRAINT(j + n, I);
      STL2_EXACT_TYPE_CONSTRAINT(n + j, I);
      STL2_EXACT_TYPE_CONSTRAINT(i -= n, I&);
      STL2_EXACT_TYPE_CONSTRAINT(j - n, I);
      STL2_CONVERSION_CONSTRAINT(j[n], const ValueType<I>&);
    } &&
    (!detail::MutableIterator<I> ||
     requires (const I& i, const DifferenceType<I> n) {
       i[n] = *i;
       *i = i[n];
     });
}

namespace ext {
template <class I>
concept bool ContiguousIterator() {
  return RandomAccessIterator<I>() &&
    DerivedFrom<IteratorCategory<I>, contiguous_iterator_tag>() &&
    _Is<ReferenceType<I>, std::is_reference> &&
    Same<ValueType<I>, std::decay_t<ReferenceType<I>>>();
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

template <class, class>
constexpr bool indirectly_movable() { return false; }
IndirectlyMovable{T, U}
constexpr bool indirectly_movable() { return true; }

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

template <class>
constexpr bool iterator() { return false; }
Iterator{I}
constexpr bool iterator() { return true; }

template <class, class>
constexpr bool sentinel() { return false; }
Sentinel{S, I}
constexpr bool sentinel() { return true; }

template <class, class>
constexpr bool weak_output_iterator() { return false; }
WeakOutputIterator{I, T}
constexpr bool weak_output_iterator() { return true; }

template <class, class>
constexpr bool output_iterator() { return false; }
OutputIterator{I, T}
constexpr bool output_iterator() { return true; }

template <class>
constexpr bool weak_input_iterator() { return false; }
WeakInputIterator{I}
constexpr bool weak_input_iterator() { return true; }

template <class>
constexpr bool input_iterator() { return false; }
InputIterator{I}
constexpr bool input_iterator() { return true; }

template <class>
constexpr bool forward_iterator() { return false; }
ForwardIterator{I}
constexpr bool forward_iterator() { return true; }

template <class>
constexpr bool bidirectional_iterator() { return false; }
BidirectionalIterator{I}
constexpr bool bidirectional_iterator() { return true; }

template <class I, class S = I>
constexpr bool sized_iterator_range() { return false; }
SizedIteratorRange{I, S}
constexpr bool sized_iterator_range() { return true; }

template <class>
constexpr bool random_access_iterator() { return false; }
RandomAccessIterator{I}
constexpr bool random_access_iterator() { return true; }

template <class>
constexpr bool contiguous_iterator() { return false; }
ContiguousIterator{I}
constexpr bool contiguous_iterator() { return true; }
}} // namespace ext::models

///////////////////////////////////////////////////////////////////////////
// iterator_traits [iterator.assoc]
//
template <WeakInputIterator I>
struct __pointer_type {
  using type = std::add_pointer_t<ReferenceType<I>>;
};

template <WeakInputIterator I>
  requires requires (I i) { { i.operator->() } -> auto&&; }
struct __pointer_type<I> {
  using type = decltype(declval<I>().operator->());
};

template <class>
struct __iterator_traits { };

template <WeakIterator I>
struct __iterator_traits<I> {
  using difference_type = DifferenceType<I>;
  using value_type = void;
  using reference = void;
  using pointer = void;
  using iterator_category = output_iterator_tag;
};

template <WeakInputIterator I>
struct __iterator_traits<I> {
  using difference_type = DifferenceType<I>;
  using value_type = ValueType<I>;
  using reference = ReferenceType<I>;
  using pointer = meta::_t<__pointer_type<I>>;
  using iterator_category = IteratorCategory<I>;
};

template <class I>
using iterator_traits = __iterator_traits<I>;

///////////////////////////////////////////////////////////////////////////
// Standard iterator traits [iterator.stdtraits]
// 20150802: Not to spec: Adds constraints to ::iterator_category so as to
//           apply only to STL2 iterators and avoid "partial specialization
//           after instantiation" errors.
//
template <stl2::WeakIterator W>
struct __std_out_value_type {
  using type = void;
};
template <stl2::WeakIterator W>
  requires detail::MemberValueType<W>
struct __std_out_value_type<W> {
  using type = typename W::value_type;
};

template <stl2::WeakIterator W, class Default = void>
struct __std_reference_type {
  using type = Default;
};
template <stl2::WeakIterator W, class Default>
  requires requires { typename W::reference; }
struct __std_reference_type<W, Default> {
  using type = typename W::reference;
};

template <stl2::WeakIterator W, class Default = void>
struct __std_pointer_type {
  using type = Default;
};
template <stl2::WeakIterator W, class Default>
  requires requires { typename W::pointer; }
struct __std_pointer_type<W, Default> {
  using type = typename W::pointer;
};

namespace detail {
namespace stl2_to_std_iterator_category_ {
  std::input_iterator_tag f(input_iterator_tag*);
  std::forward_iterator_tag f(forward_iterator_tag*);
  std::bidirectional_iterator_tag f(bidirectional_iterator_tag*);
  std::random_access_iterator_tag f(random_access_iterator_tag*);
}

template <class T>
using stl2_to_std_iterator_category =
  decltype(stl2_to_std_iterator_category_::f((T*)nullptr));
}}} // namespace stl2::v1::detail

namespace std {
template <::stl2::WeakIterator Out>
  requires !::stl2::detail::MemberIteratorCategory<Out>
struct iterator_traits<Out> {
  using difference_type = ::stl2::DifferenceType<Out>;
  using value_type = meta::_t<::stl2::__std_out_value_type<Out>>;
  using reference = meta::_t<::stl2::__std_reference_type<Out>>;
  using pointer = meta::_t<::stl2::__std_pointer_type<Out>>;
  using iterator_category = std::output_iterator_tag;
};

template <::stl2::WeakInputIterator WI>
  requires ::stl2::DerivedFrom<typename WI::iterator_category,
                               ::stl2::weak_input_iterator_tag>() &&
    !::stl2::DerivedFrom<typename WI::iterator_category,
                               ::stl2::input_iterator_tag>()
struct iterator_traits<WI> {};

template <::stl2::InputIterator I>
  requires ::stl2::DerivedFrom<typename I::iterator_category,
                               ::stl2::input_iterator_tag>()
struct iterator_traits<I> {
  using difference_type = ::stl2::DifferenceType<I>;
  using value_type = ::stl2::ValueType<I>;
  using reference = meta::_t<
    ::stl2::__std_reference_type<I, ::stl2::ReferenceType<I>>>;
  using pointer = meta::_t<
    ::stl2::__std_pointer_type<I, typename ::stl2::iterator_traits<I>::pointer>>;
  using iterator_category = meta::if_<
    is_reference<::stl2::ReferenceType<I>>,
    ::stl2::detail::stl2_to_std_iterator_category<typename I::iterator_category>,
    std::input_iterator_tag>;
};
} // namespace std

#endif
