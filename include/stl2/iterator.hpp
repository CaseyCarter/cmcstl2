#ifndef STL2_ITERATOR_HPP
#define STL2_ITERATOR_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/iterator.hpp>
#include <stl2/utility.hpp>

namespace stl2 { inline namespace v1 {

template <detail::Dereferencable R>
detail::__iter_move_t<R> iter_move2(R r)
  noexcept(noexcept(detail::__iter_move_t<R>(stl2::move(*r)))) {
  return stl2::move(*r);
}

// iter_swap2
template <Readable R1, Readable R2>
  requires Swappable<ReferenceType<R1>, ReferenceType<R2>>()
void iter_swap2(R1 r1, R2 r2)
  noexcept(is_nothrow_swappable_v<ReferenceType<R1>, ReferenceType<R2>>) {
  swap(*r1, *r2);
}

template <Readable R1, Readable R2>
  requires (!Swappable<ReferenceType<R1>, ReferenceType<R2>>() /*&&
    detail::IndirectlyMovable<R1, R2> &&
    detail::IndirectlyMovable<R2, R1>*/)
void iter_swap2(R1 r1, R2 r2)
  noexcept(detail::is_nothrow_indirectly_movable_v<R1, R2> &&
           detail::is_nothrow_indirectly_movable_v<R2, R1>) {
  ValueType<R1> tmp(iter_move2(r1));
  *r1 = iter_move2(r2);
  *r2 = stl2::move(tmp);
}

}} // namespace stl2::v1

#endif // STL2_ITERATOR_HPP
