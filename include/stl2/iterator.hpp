#ifndef STL2_ITERATOR_HPP
#define STL2_ITERATOR_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/iterator.hpp>
#include <stl2/utility.hpp>

namespace stl2 { inline namespace v1 {

template <class RR, class R = std::remove_reference_t<RR>>
  requires detail::Dereferencable<R>
detail::__iter_move_t<R> iter_move2(RR& r)
  noexcept(noexcept(detail::__iter_move_t<R>(stl2::move(*r)))) {
  return stl2::move(*r);
}

// iter_swap2
template <class RR1, class RR2,
          class R1 = std::remove_reference_t<RR1>,
          class R2 = std::remove_reference_t<RR2>>
  requires Readable<R1>() && Readable<R2>() &&
    Swappable<ReferenceType<R1>, ReferenceType<R2>>()
void iter_swap2(RR1&& r1, RR2&& r2)
  noexcept(is_nothrow_swappable_v<ReferenceType<R1>, ReferenceType<R2>>) {
  swap(*r1, *r2);
}

template <class RR1, class RR2,
          class R1 = std::remove_reference_t<RR1>,
          class R2 = std::remove_reference_t<RR2>>
  requires detail::IterSwappable<R1, R2> &&
    !Swappable<ReferenceType<R1>, ReferenceType<R2>>()
void iter_swap2(RR1&& r1, RR2&& r2)
  noexcept(detail::is_nothrow_iter_swappable_v<R1, R2>) {
  ValueType<R1> tmp(iter_move2(r1));
  *r1 = iter_move2(r2);
  *r2 = stl2::move(tmp);
}

}} // namespace stl2::v1

#endif // STL2_ITERATOR_HPP
