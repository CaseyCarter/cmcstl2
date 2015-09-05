#ifndef STL2_DETAIL_ITERATOR_OPERATIONS_HPP
#define STL2_DETAIL_ITERATOR_OPERATIONS_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// Iterator operations [iterator.operations]
//
STL2_OPEN_NAMESPACE {
  // advance
  WeakIterator{I}
  constexpr void advance(I& i, DifferenceType<I> n)
    noexcept(noexcept(++i)) {
    while (n != 0) {
      --n;
      ++i;
    }
  }
  
  BidirectionalIterator{I}
  constexpr void advance(I& i, DifferenceType<I> n)
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
  constexpr void advance(I& i, DifferenceType<I> n)
  STL2_NOEXCEPT_RETURN(
    void(i += n)
  )
  
  Sentinel{S, I}
  constexpr void advance(I& i, S bound)
    noexcept(noexcept(i != bound, void(), ++i)) {
    while (i != bound) {
      ++i;
    }
  }
  
  SizedIteratorRange{I, S}
  constexpr void advance(I& i, S bound)
  STL2_NOEXCEPT_RETURN(
    __stl2::advance(i, bound - i)
  )
  
  Iterator{I}
  constexpr void advance(I& i, I bound)
  STL2_NOEXCEPT_RETURN(
    void(i = bound)
  )
  
  Sentinel{S, I}
  constexpr DifferenceType<I>
  advance(I& i, DifferenceType<I> n, S bound)
    noexcept(noexcept(i != bound, void(), ++i)) {
    while (i != bound && n != 0) {
      ++i;
      --n;
    }
    return n;
  }
  
  template <BidirectionalIterator I, Sentinel<I> S>
  constexpr DifferenceType<I>
  advance(I& i, DifferenceType<I> n, S bound)
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
  constexpr DifferenceType<I>
  advance(I& i, DifferenceType<I> n, S bound)
    noexcept(noexcept(
      DifferenceType<I>{bound - i},
      __stl2::advance(i, bound),
      __stl2::advance(i, n)))
  {
    const auto D = DifferenceType<I>{bound - i};
    if (n >= 0 ? n >= D : n <= D) {
      __stl2::advance(i, bound);
      return n - D;
    } else {
      __stl2::advance(i, n);
      return 0;
    }
  }
  
  // distance
  Sentinel{S, I}
  constexpr DifferenceType<I> distance(I first, S last)
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
  constexpr DifferenceType<I> distance(I first, S last)
  STL2_NOEXCEPT_RETURN(
    last - first
  )
  
  // next
  WeakIterator{I}
  constexpr I next(I x, DifferenceType<I> n = 1)
  STL2_NOEXCEPT_RETURN(
    __stl2::advance(x, n), x
  )
  
  Sentinel{S, I}
  constexpr I next(I x, S bound)
  STL2_NOEXCEPT_RETURN(
    __stl2::advance(x, bound), x
  )
  
  Sentinel{S, I}
  constexpr I next(I x, DifferenceType<I> n, S bound)
  STL2_NOEXCEPT_RETURN(
    __stl2::advance(x, n, bound), x
  )
  
  // prev
  BidirectionalIterator{I}
  constexpr I prev(I x, DifferenceType<I> n = 1)
  STL2_NOEXCEPT_RETURN(
    __stl2::advance(x, -n), x
  )
  
  template <BidirectionalIterator I, Sentinel<I> S>
  constexpr I prev(I x, DifferenceType<I> n, S bound)
  STL2_NOEXCEPT_RETURN(
    __stl2::advance(x, -n, bound), x
  )
} STL2_CLOSE_NAMESPACE

#endif
