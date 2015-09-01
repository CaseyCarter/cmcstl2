#ifndef STL2_DETAIL_ITERATOR_OPERATIONS_HPP
#define STL2_DETAIL_ITERATOR_OPERATIONS_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>

namespace stl2 { inline namespace v1 {

///////////////////////////////////////////////////////////////////////////
// Iterator operations [iterator.operations]
//
// advance
WeakIterator{I}
void advance(I& i, DifferenceType<I> n)
  noexcept(noexcept(++i)) {
  while (n != 0) {
    --n;
    ++i;
  }
}

BidirectionalIterator{I}
void advance(I& i, DifferenceType<I> n)
  noexcept(noexcept(++i, void(), --i)) {
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
void advance(I& i, DifferenceType<I> n)
STL2_NOEXCEPT_RETURN(
  void(i += n)
)

Sentinel{S, I}
void advance(I& i, S bound)
  noexcept(noexcept(i != bound, void(), ++i)) {
  while (i != bound) {
    ++i;
  }
}

SizedIteratorRange{I, S}
void advance(I& i, S bound)
STL2_NOEXCEPT_RETURN(
  stl2::advance(i, bound - i)
)

Iterator{I}
void advance(I& i, I bound)
STL2_NOEXCEPT_RETURN(
  void(i = bound)
)

Sentinel{S, I}
DifferenceType<I> advance(I& i, DifferenceType<I> n, S bound)
  noexcept(noexcept(i != bound, void(), ++i)) {
  while (i != bound && n != 0) {
    ++i;
    --n;
  }
  return n;
}

template <BidirectionalIterator I, Sentinel<I> S>
DifferenceType<I> advance(I& i, DifferenceType<I> n, S bound)
  noexcept(noexcept(i!= bound, void(), ++i, void(), --i)) {
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
DifferenceType<I> advance(I& i, DifferenceType<I> n, S bound)
  noexcept(noexcept(
    DifferenceType<I>{bound - i},
    stl2::advance(i, bound),
    stl2::advance(i, n)))
{
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
DifferenceType<I> distance(I first, S last)
  noexcept(noexcept(first != last, void(), ++first)) {
  DifferenceType<I> n = 0;
  while (first != last) {
    ++n;
    ++first;
  }
  return n;
}

template <class S, class I>
  requires SizedIteratorRange<I, S>()
DifferenceType<I> distance(I first, S last)
STL2_NOEXCEPT_RETURN(
  last - first
)

// next
WeakIterator{I}
I next(I x, DifferenceType<I> n = 1)
STL2_NOEXCEPT_RETURN(
  stl2::advance(x, n), x
)

Sentinel{S, I}
I next(I x, S bound)
STL2_NOEXCEPT_RETURN(
  stl2::advance(x, bound), x
)

Sentinel{S, I}
I next(I x, DifferenceType<I> n, S bound)
STL2_NOEXCEPT_RETURN(
  stl2::advance(x, n, bound), x
)

// prev
BidirectionalIterator{I}
I prev(I x, DifferenceType<I> n = 1)
STL2_NOEXCEPT_RETURN(
  stl2::advance(x, -n), x
)

template <BidirectionalIterator I, Sentinel<I> S>
I prev(I x, DifferenceType<I> n, S bound)
STL2_NOEXCEPT_RETURN(
  stl2::advance(x, -n, bound), x
)
}} // namespace stl2::v1

#endif
