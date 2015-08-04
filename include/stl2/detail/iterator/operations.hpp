#ifndef STL2_DETAIL_ITERATOR_OPERATIONS_HPP
#define STL2_DETAIL_ITERATOR_OPERATIONS_HPP

#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/concepts/range.hpp>

namespace stl2 { inline namespace v1 {

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

}} // namespace stl2::v1

#endif
