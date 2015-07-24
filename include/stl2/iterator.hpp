#ifndef STL2_ITERATOR_HPP
#define STL2_ITERATOR_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/iterator.hpp>
#include <stl2/utility.hpp>

namespace stl2 { inline namespace v1 {

template <class R, detail::Dereferencable _R = std::remove_reference_t<R>>
detail::__iter_move_t<_R> iter_move(R&& r)
  noexcept(noexcept(detail::__iter_move_t<R>(stl2::move(*r)))) {
  return stl2::move(*r);
}

namespace detail {
  // __iter_swap2
  template <class R1, class R2,
    Readable _R1 = std::remove_reference_t<R1>,
    Readable _R2 = std::remove_reference_t<R2>>
    requires Swappable<ReferenceType<_R1>, ReferenceType<_R2>>()
  void __iter_swap2(R1&& r1, R2&& r2, int)
    noexcept(ext::is_nothrow_swappable_v<ReferenceType<_R1>, ReferenceType<_R2>>) {
    swap(*r1, *r2);
  }

  template <class R1, class R2,
    Readable _R1 = std::remove_reference_t<R1>,
    Readable _R2 = std::remove_reference_t<R2>>
    requires IndirectlyMovable<_R1, _R2>() && IndirectlyMovable<_R2, _R1>()
  void __iter_swap2(R1&& r1, R2&& r2, long)
    noexcept(ext::is_nothrow_indirectly_movable_v<_R1, _R2> &&
             ext::is_nothrow_indirectly_movable_v<_R2, _R1>) {
    ValueType<_R1> tmp = iter_move(r1);
    *r1 = iter_move(r2);
    *r2 = std::move(tmp);
  }
}

// iter_swap2
template <class R1, class R2,
  Readable _R1 = std::remove_reference_t<R1>,
  Readable _R2 = std::remove_reference_t<R2>>
  requires Swappable<ReferenceType<_R1>, ReferenceType<_R2>>() ||
    (IndirectlyMovable<_R1, _R2>() && IndirectlyMovable<_R2, _R1>())
void iter_swap2(R1&& r1, R2&& r2)
  noexcept(noexcept(detail::__iter_swap2((R1&&)r1, (R2&&)r2, 0))) {
  detail::__iter_swap2((R1&&)r1, (R2&&)r2, 0);
}

}} // namespace stl2::v1

#endif // STL2_ITERATOR_HPP
