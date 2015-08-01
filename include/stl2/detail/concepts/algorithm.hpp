#ifndef STL2_DETAIL_CONCEPTS_ALGORITHM_HPP
#define STL2_DETAIL_CONCEPTS_ALGORITHM_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>

///////////////////////////////////////////////////////////////////////////
// Common algorithm requirements [commmonalgoreq]
//
namespace stl2 { inline namespace v1 {
///////////////////////////////////////////////////////////////////////////
// IndirectlyComparable [indirectlycomparable.commmonalgoreq]
//
template <class I1, class I2,
  class R = equal_to<>, class P1 = identity, class P2 = identity>
concept bool IndirectlyComparable() {
  return IndirectCallableRelation<R, Projected<I1, P1>, Projected<I2, P2>>();
}

///////////////////////////////////////////////////////////////////////////
// Permutable [permutable.commmonalgoreq]
//
template <class I>
concept bool Permutable() {
  return ForwardIterator<I>() &&
    Movable<ValueType<I>>() &&
    IndirectlyMovable<I, I>();
}

///////////////////////////////////////////////////////////////////////////
// Mergeable [mergeable.commmonalgoreq]
//
template <class I1, class I2, class Out,
  class R = less<>, class P1 = identity, class P2 = identity>
concept bool Mergeable() {
  return InputIterator<I1>() &&
    InputIterator<I2>() &&
    WeaklyIncrementable<Out>() &&
    IndirectlyCopyable<I1, Out>() &&
    IndirectlyCopyable<I2, Out>() &&
    IndirectCallableStrictWeakOrder<R, Projected<I1, P1>, Projected<I2, P2>>();
}

///////////////////////////////////////////////////////////////////////////
// MergeMovable [mergemovable.commmonalgoreq]
//

template <class I1, class I2, class Out,
  class R = less<>, class P1 = identity, class P2 = identity>
concept bool MergeMovable() {
  return InputIterator<I1>() &&
    InputIterator<I2>() &&
    WeaklyIncrementable<Out>() &&
    IndirectlyMovable<I1, Out>() &&
    IndirectlyMovable<I2, Out>() &&
    IndirectCallableStrictWeakOrder<R, Projected<I1, P1>, Projected<I2, P2>>();
}

///////////////////////////////////////////////////////////////////////////
// Sortable [sortable.commmonalgoreq]
//
template <class I, class R = less<>, class P = identity>
concept bool Sortable() {
  return ForwardIterator<I> &&
    Permutable<I>() &&
    IndirectCallableStrictWeakOrder<R, Projected<I, P>>();
}
}}

#endif
