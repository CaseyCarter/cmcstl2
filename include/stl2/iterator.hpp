#ifndef STL2_ITERATOR_HPP
#define STL2_ITERATOR_HPP

#include <cassert>
#include <initializer_list>
#include <iterator>
#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/utility.hpp>

////////////////////////////////////////////
// Iterator concepts [iterator.requirements]
//
namespace stl2 { inline namespace v1 {

// Imports
using std::begin;
using std::end;
using std::iterator;

template <class C>
constexpr auto cbegin(const C& c)
  noexcept(noexcept(stl2::begin(c))) ->
  decltype(stl2::begin(c)) {
  return stl2::begin(c);
}

template <class C>
constexpr auto cend(const C& c)
  noexcept(noexcept(stl2::end(c))) ->
  decltype(stl2::end(c)) {
  return stl2::end(c);
}

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

template <detail::Dereferenceable R>
using __iter_move_t =
  meta::if_<
    std::is_reference<ReferenceType<R>>,
    std::remove_reference_t<ReferenceType<R>> &&,
    std::decay_t<ReferenceType<R>>>;

}

template <class R,
  detail::Dereferenceable _R = std::remove_reference_t<R>>
detail::__iter_move_t<_R> iter_move(R&& r)
  noexcept(noexcept(detail::__iter_move_t<_R>(stl2::move(*r))));

template <detail::Dereferenceable R>
  requires requires (R& r) {
    { iter_move(r) } -> auto&&;
  }
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

namespace ext {
template <class In, class Out>
constexpr bool is_nothrow_indirectly_movable_v = false;

IndirectlyMovable{In, Out}
constexpr bool is_nothrow_indirectly_movable_v =
  std::is_nothrow_constructible<ValueType<In>, RvalueReferenceType<In>>::value &&
  std::is_nothrow_assignable<ValueType<In> &, RvalueReferenceType<In>>::value &&
  std::is_nothrow_assignable<ReferenceType<Out>, RvalueReferenceType<In>>::value &&
  std::is_nothrow_assignable<ReferenceType<Out>, ValueType<In>>::value;

template <class In, class Out>
struct is_nothrow_indirectly_movable
  : meta::bool_<is_nothrow_indirectly_movable_v<In, Out>> {};

template <class In, class Out>
using is_nothrow_indirectly_movable_t = meta::_t<is_nothrow_indirectly_movable<In, Out>>;
}

// iter_swap2
template <class R1, class R2,
  Readable _R1 = std::remove_reference_t<R1>,
  Readable _R2 = std::remove_reference_t<R2>>
  requires Swappable<ReferenceType<_R1>, ReferenceType<_R2>>()
void iter_swap2(R1&& r1, R2&& r2)
  noexcept(ext::is_nothrow_swappable_v<ReferenceType<_R1>, ReferenceType<_R2>>);

template <class R1, class R2,
  Readable _R1 = std::remove_reference_t<R1>,
  Readable _R2 = std::remove_reference_t<R2>>
  requires IndirectlyMovable<_R1, _R2>() && IndirectlyMovable<_R2, _R1>() &&
    !Swappable<ReferenceType<_R1>, ReferenceType<_R2>>()
void iter_swap2(R1&& r1, R2&& r2)
  noexcept(ext::is_nothrow_indirectly_movable_v<_R1, _R2> &&
           ext::is_nothrow_indirectly_movable_v<_R2, _R1>);

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

namespace ext {
template <class R1, class R2>
struct is_nothrow_indirectly_swappable : meta::bool_<false> {};

IndirectlySwappable{R1, R2}
struct is_nothrow_indirectly_swappable<R1, R2> :
  meta::bool_<
    noexcept(iter_swap2(stl2::declval<R1>(), stl2::declval<R2>())) &&
    noexcept(iter_swap2(stl2::declval<R2>(), stl2::declval<R1>())) &&
    noexcept(iter_swap2(stl2::declval<R1>(), stl2::declval<R1>())) &&
    noexcept(iter_swap2(stl2::declval<R2>(), stl2::declval<R2>()))>
{};

template <class R1, class R2>
constexpr bool is_nothrow_indirectly_swappable_v =
  meta::_v<is_nothrow_indirectly_swappable<R1, R2>>;

template <class R1, class R2>
using is_nothrow_indirectly_swappable_t =
  meta::_t<is_nothrow_indirectly_swappable<R1, R2>>;
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

////////////////////////////////////////////
// Iterator primitives
//
template <class R,
  detail::Dereferenceable _R = std::remove_reference_t<R>>
detail::__iter_move_t<_R> iter_move(R&& r)
  noexcept(noexcept(detail::__iter_move_t<_R>(stl2::move(*r)))) {
  return stl2::move(*r);
}

// iter_swap2
template <class R1, class R2,
  Readable _R1 = std::remove_reference_t<R1>,
  Readable _R2 = std::remove_reference_t<R2>>
  requires Swappable<ReferenceType<_R1>, ReferenceType<_R2>>()
void iter_swap2(R1&& r1, R2&& r2)
  noexcept(ext::is_nothrow_swappable_v<ReferenceType<_R1>, ReferenceType<_R2>>) {
  swap(*r1, *r2);
}

template <class R1, class R2,
  Readable _R1 = std::remove_reference_t<R1>,
  Readable _R2 = std::remove_reference_t<R2>>
  requires IndirectlyMovable<_R1, _R2>() && IndirectlyMovable<_R2, _R1>() &&
    !Swappable<ReferenceType<_R1>, ReferenceType<_R2>>()
void iter_swap2(R1&& r1, R2&& r2)
  noexcept(ext::is_nothrow_indirectly_movable_v<_R1, _R2> &&
           ext::is_nothrow_indirectly_movable_v<_R2, _R1>) {
  ValueType<_R1> tmp = iter_move(r1);
  *r1 = iter_move(r2);
  *r2 = stl2::move(tmp);
}

// size
template <class T, std::size_t N>
constexpr std::size_t size(T(&)[N]) noexcept {
  return N;
}

template <class C>
  requires requires (const C& c) {
    STL2_DEDUCTION_CONSTRAINT(c.size(), Integral);
  }
constexpr auto /* Integral */ size(const C& c)
  noexcept(noexcept(c.size())) {
  return c.size();
}

///////////////////////////////////////////////////////////////////////////
// Range [iterable.iterables]
//
template <class T>
  requires requires (T& t) {
    STL2_DEDUCTION_CONSTRAINT(begin(t), Iterator);
  }
using IteratorType = decltype(begin(declval<T&>()));

template <class T>
  requires requires (T& t) {
    // { end(t) } -> Sentinel<IteratorType<T>>;
    requires Sentinel<decltype(end(t)), IteratorType<T>>();
  }
using SentinelType = decltype(end(declval<T&>()));

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

template <class T>
concept bool _Unqual = Same<T, detail::uncvref_t<T>>();

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
      STL2_DEDUCTION_CONSTRAINT(size(r), Integral);
      STL2_CONVERSION_CONSTRAINT(size(r), DifferenceType<IteratorType<R>>);
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
  return Range<T>() && ContiguousIterator<IteratorType<T>>();
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

}} // namespace ext::models

///////////////////////////////////////////////////////////////////////////
// Iterator operations [iterator.operations]
//
// advance
WeakIterator{I}
void advance(I& i, DifferenceType<I> n) {
  while (n != 0) {
    --n;
    ++i;
  }
}

BidirectionalIterator{I}
void advance(I& i, DifferenceType<I> n) {
  if (n > 0) {
    while (n != 0) {
      --n;
      ++i;
    }
  } else {
    while (n != 0) {
      ++n;
      --i;
    }
  }
}

RandomAccessIterator{I}
void advance(I& i, DifferenceType<I> n) {
  i += n;
}

Sentinel{S, I}
void advance(I& i, S bound) {
  while (i != bound) {
    ++i;
  }
}

SizedIteratorRange{I, S}
void advance(I& i, S bound) {
  stl2::advance(i, bound - i);
}

Iterator{I}
void advance(I& i, I bound) {
  i = bound;
}

Sentinel{S, I}
DifferenceType<I> advance(I& i, DifferenceType<I> n, S bound) {
  while (i != bound && n != 0) {
    ++i;
    --n;
  }
  return n;
}

template <BidirectionalIterator I, Sentinel<I> S>
DifferenceType<I> advance(I& i, DifferenceType<I> n, S bound) {
  if (n > 0) {
    while (i != bound && n != 0) {
      ++i;
      --n;
    }
  } else {
    while (i != bound && n != 0) {
      --i;
      ++n;
    }
  }
  return n;
}

SizedIteratorRange{I, S}
DifferenceType<I> advance(I& i, DifferenceType<I> n, S bound) {
  const auto D = DifferenceType<I>{bound - i};
  if (n >= 0 ? n >= D : n <= D) {
    stl2::advance(i, bound);
    return n - D;
  } else {
    stl2::advance(i, n);
    return 0;
  }
}

// distance
Sentinel{S, I}
DifferenceType<I> distance(I first, S last) {
  DifferenceType<I> n = 0;
  while (first != last) {
    ++n;
    ++first;
  }
  return n;
}

SizedIteratorRange{I, S}
DifferenceType<I> distance(I first, S last) {
  return last - first;
}

Range{R}
DifferenceType<R> distance(R&& r) {
  return stl2::distance(begin(r), end(r));
}

SizedRange{R}
DifferenceType<R> distance(R&& r) {
  return size(r);
}

// next
WeakIterator{I}
I next(I x, DifferenceType<I> n = 1) {
  stl2::advance(x, n);
  return x;
}

Sentinel{S, I}
I next(I x, S bound) {
  stl2::advance(x, bound);
  return x;
}

Sentinel{S, I}
I next(I x, DifferenceType<I> n, S bound) {
  stl2::advance(x, n, bound);
  return x;
}

// prev
BidirectionalIterator{I}
I prev(I x, DifferenceType<I> n = 1) {
  stl2::advance(x, -n);
  return x;
}

template <BidirectionalIterator I, Sentinel<I> S>
I prev(I x, DifferenceType<I> n, S bound) {
  stl2::advance(x, -n, bound);
  return x;
}

///////////////////////////////////////////////////////////////////////////
// reverse_iterator [reverse.iterator]
//
template <BidirectionalIterator I>
class reverse_iterator {
public:
  using iterator_type = I;
  using difference_type = DifferenceType<I>;
  using value_type = ValueType<I>;
  using iterator_category = IteratorCategory<I>;
  using reference = ReferenceType<I>;
  using pointer = I;

  reverse_iterator() = default;
  explicit reverse_iterator(I x) :
    current{stl2::move(x)} {}

  template <ConvertibleTo<I> U>
  reverse_iterator(const reverse_iterator<U>& u) :
    current{u.current} {}

  template <ConvertibleTo<I> U>
  reverse_iterator& operator=(const reverse_iterator<U>& u) & {
    current = u.current;
    return *this;
  }

  I base() const {
    return current;
  }
  // 20150802: Extension
  explicit operator const I&() const& {
    return current;
  }
  // 20150802: Extension
  explicit operator I&&() && {
    return stl2::move(current);
  }

  reference operator*() const {
    return *stl2::prev(current);
  }

  pointer operator->() const
#if 0 // FIXME: this is hard erroring when I is a pointer.
    requires _Is<I, std::is_pointer> ||
      requires (const I& i) {
        i.operator->();
      }
#endif
  {
    return stl2::prev(current);
  }

  reverse_iterator& operator++() & {
    --current;
    return *this;
  }
  reverse_iterator operator++(int) & {
    auto tmp = *this;
    ++*this;
    return tmp;
  }

  reverse_iterator& operator--() & {
    ++current;
    return *this;
  }
  reverse_iterator operator--(int) & {
    auto tmp = *this;
    --*this;
    return tmp;
  }

  reverse_iterator operator+(difference_type n) const
    requires RandomAccessIterator<I>() {
    return reverse_iterator{current - n};
  }
  reverse_iterator& operator+=(difference_type n) &
    requires RandomAccessIterator<I>() {
    current -= n;
    return *this;
  }
  reverse_iterator operator-(difference_type n) const
    requires RandomAccessIterator<I>() {
    return reverse_iterator{current + n};
  }
  reverse_iterator& operator-=(difference_type n) &
    requires RandomAccessIterator<I>() {
    current += n;
    return *this;
  }

  decltype(auto) operator[](difference_type n) const
    requires RandomAccessIterator<I>() {
    return current[-n - 1];
  }

protected:
  I current{};
};

EqualityComparable{I1, I2}
bool operator==(const reverse_iterator<I1>& x,
                const reverse_iterator<I2>& y) {
  return static_cast<const I1&>(x) == static_cast<const I2&>(y);
}

EqualityComparable{I1, I2}
bool operator!=(const reverse_iterator<I1>& x,
                const reverse_iterator<I2>& y) {
  return static_cast<const I1&>(x) != static_cast<const I2&>(y);
}

TotallyOrdered{I1, I2}
bool operator<(const reverse_iterator<I1>& x,
               const reverse_iterator<I2>& y) {
  return static_cast<const I1&>(x) > static_cast<const I2&>(y);
}

TotallyOrdered{I1, I2}
bool operator>(const reverse_iterator<I1>& x,
               const reverse_iterator<I2>& y) {
  return static_cast<const I1&>(x) < static_cast<const I2&>(y);
}

TotallyOrdered{I1, I2}
bool operator<=(const reverse_iterator<I1>& x,
                const reverse_iterator<I2>& y) {
  return static_cast<const I1&>(x) >= static_cast<const I2&>(y);
}

TotallyOrdered{I1, I2}
bool operator>=(const reverse_iterator<I1>& x,
                const reverse_iterator<I2>& y) {
  return static_cast<const I1&>(x) <= static_cast<const I2&>(y);
}

SizedIteratorRange{I2, I1}
DifferenceType<I2> operator-(const reverse_iterator<I1>& x,
                             const reverse_iterator<I2>& y) {
  return static_cast<const I2&>(y) - static_cast<const I1&>(x);
}

template <class I>
reverse_iterator<I> operator+(DifferenceType<I> n,
                              const reverse_iterator<I>& x) {
  return reverse_iterator<I>{static_cast<const I&>(x) - n};
}

BidirectionalIterator{I}
auto make_reverse_iterator(I i) {
  return reverse_iterator<I>{stl2::move(i)};
}

template <class C>
constexpr auto rbegin(C& c) -> decltype(c.rbegin()) {
  return c.rbegin();
}
template <class C>
constexpr auto rbegin(const C& c) -> decltype(c.rbegin()) {
  return c.rbegin();
}

template <class C>
constexpr auto rend(C& c) -> decltype(c.rend()) {
  return c.rend();
}

template <class C>
constexpr auto rend(const C& c) -> decltype(c.rend()) {
  return c.rend();
}

template <class T, size_t N>
reverse_iterator<T*> rbegin(T (&array)[N]) {
  return reverse_iterator<T*>{stl2::end(array)};
}

template <class T, size_t N>
reverse_iterator<T*> rend(T (&array)[N]) {
  return reverse_iterator<T*>{stl2::begin(array)};
}

template <class E>
reverse_iterator<const E*> rbegin(std::initializer_list<E> il) {
  return reverse_iterator<const E*>{il.end()};
}

template <class E>
reverse_iterator<const E*> rend(std::initializer_list<E> il) {
  return reverse_iterator<const E*>{il.begin()};
}

template <class C>
constexpr auto crbegin(const C& c) -> decltype(stl2::rbegin(c)) {
  return stl2::rbegin(c);
}

template <class C>
constexpr auto crend(const C& c) -> decltype(stl2::rend(c)) {
  return stl2::rend(c);
}


template <class Derived, class Container>
  requires requires { typename Container::value_type; }
class __insert_iterator_base {
public:
  using container_type = Container;
  using difference_type = std::ptrdiff_t;
  using iterator_category = output_iterator_tag;

  // Extension
  using value_type = typename Container::value_type;

  __insert_iterator_base() = default;
  explicit __insert_iterator_base(Container& x) :
    container{std::addressof(x)} {}

  Derived& operator*() {
    return static_cast<Derived&>(*this);
  }

  Derived& operator++() & {
    return static_cast<Derived&>(*this);
    return *this;
  }
  Derived operator++(int) & {
    return static_cast<Derived&>(*this);
  }

protected:
  Container* container = nullptr;
};

///////////////////////////////////////////////////////////////////////////
// back_insert_iterator [back.insert.iterator]
//
namespace detail {
template <class C, class V>
concept bool back_insertable =
  requires (C& c, V&& v) {
    c.push_back((V&&)v);
  };
}

template <class Container>
  requires requires { typename Container::value_type; }
class back_insert_iterator :
  public __insert_iterator_base<back_insert_iterator<Container>, Container> {
  using __base_t = __insert_iterator_base<back_insert_iterator<Container>, Container>;

public:
  using typename __base_t::value_type;
  using __base_t::__base_t;

  back_insert_iterator&
  operator=(const value_type& value) &
    requires detail::back_insertable<Container, const value_type&> {
    assert(this->container);
    this->container->push_back(value);
    return *this;
  }

  back_insert_iterator&
  operator=(value_type&& value) &
    requires detail::back_insertable<Container, value_type&&> {
    assert(this->container);
    this->container->push_back(stl2::move(value));
    return *this;
  }
};

template <class Container>
auto back_inserter(Container& x) {
  return back_insert_iterator<Container>{x};
}

///////////////////////////////////////////////////////////////////////////
// front_insert_iterator [front.insert.iterator]
//
namespace detail {
template <class C, class V>
concept bool front_insertable =
  requires (C& c, V&& v) {
    c.push_front((V&&)v);
  };
}

template <class Container>
  requires requires { typename Container::value_type; }
class front_insert_iterator :
  public __insert_iterator_base<front_insert_iterator<Container>, Container> {
  using __base_t = __insert_iterator_base<front_insert_iterator<Container>, Container>;
public:
  using typename __base_t::value_type;
  using __base_t::__base_t;

  front_insert_iterator&
  operator=(const value_type& value) &
    requires detail::front_insertable<Container, const value_type&> {
    assert(this->container);
    this->container->push_front(value);
    return *this;
  }

  front_insert_iterator&
  operator=(value_type&& value) &
    requires detail::front_insertable<Container, value_type&&> {
    assert(this->container);
    this->container->push_front(stl2::move(value));
    return *this;
  }
};

template <class Container>
auto front_inserter(Container& x) {
  return front_insert_iterator<Container>{x};
}

///////////////////////////////////////////////////////////////////////////
// insert_iterator [insert.iterator]
//
namespace detail {
template <class C, class V>
concept bool insertable =
  requires (C& c, typename C::iterator i, V&& v) {
    {  c.insert(i, (V&&)v) } -> typename C::iterator;
  };
}

template <class Container>
  requires requires {
    typename Container::value_type;
    typename Container::iterator;
  }
class insert_iterator :
  public __insert_iterator_base<insert_iterator<Container>, Container> {
  using __base_t = __insert_iterator_base<insert_iterator<Container>, Container>;
public:
  using typename __base_t::value_type;
  // Extension
  using iterator_type = typename Container::iterator;

  insert_iterator() = default;
  explicit insert_iterator(Container& x, iterator_type i):
    __base_t{x}, iter{stl2::move(i)} {}

  insert_iterator&
  operator=(const value_type& value) &
    requires detail::insertable<Container, const value_type&> {
    assert(this->container);
    iter = this->container->insert(iter, value);
    ++iter;
    return *this;
  }

  insert_iterator&
  operator=(value_type&& value) &
    requires detail::insertable<Container, value_type&&> {
    assert(this->container);
    iter = this->container->insert(iter, stl2::move(value));
    ++iter;
    return *this;
  }

protected:
  iterator_type iter{};
};

template <class Container>
auto inserter(Container& x, typename Container::iterator i) {
  return insert_iterator<Container>{x, stl2::move(i)};
}

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
}}}

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
}

#endif
