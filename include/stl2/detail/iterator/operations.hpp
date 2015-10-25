// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
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
  constexpr void advance(I& i, difference_type_t<I> n)
    noexcept(noexcept(++i)) {
    while (n != 0) {
      --n;
      ++i;
    }
  }

  BidirectionalIterator{I}
  constexpr void advance(I& i, difference_type_t<I> n)
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
  constexpr void advance(I& i, difference_type_t<I> n)
  STL2_NOEXCEPT_RETURN(
    void(i += n)
  )

  Sentinel{S, I}
  constexpr void advance(I& i, S bound)
    noexcept(noexcept(bool(i != bound), ++i)) {
    while (i != bound) {
      ++i;
    }
  }

  // Don't use SizedIteratorRange{I, S} here: GCC PR 67545.
  template <class S, class I>
    requires SizedIteratorRange<I, S>()
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
  constexpr difference_type_t<I>
  advance(I& i, difference_type_t<I> n, S bound)
    noexcept(noexcept(bool(i != bound), ++i)) {
    while (i != bound && n != 0) {
      ++i;
      --n;
    }
    return n;
  }

  template <class S, class I>
    requires BidirectionalIterator<I>() && Sentinel<S, I>()
  constexpr difference_type_t<I>
  advance(I& i, difference_type_t<I> n, S bound)
    noexcept(noexcept(bool(i != bound), void(), ++i, void(), --i)) {
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

  // Don't use SizedIteratorRange{I, S} here: GCC PR 67545.
  template <class S, class I>
    requires SizedIteratorRange<I, S>()
  constexpr difference_type_t<I>
  advance(I& i, difference_type_t<I> n, S bound)
    noexcept(noexcept(
      difference_type_t<I>{bound - i},
      __stl2::advance(i, bound),
      __stl2::advance(i, n)))
  {
    const auto D = difference_type_t<I>{bound - i};
    if (n >= 0 ? n >= D : n <= D) {
      __stl2::advance(i, bound);
      return n - D;
    } else {
      __stl2::advance(i, n);
      return 0;
    }
  }

  // next
  WeakIterator{I}
  constexpr I next(I x, difference_type_t<I> n = 1)
  STL2_NOEXCEPT_RETURN(
    __stl2::advance(x, n), x
  )

  Sentinel{S, I}
  constexpr I next(I x, S bound)
  STL2_NOEXCEPT_RETURN(
    __stl2::advance(x, bound), x
  )

  Sentinel{S, I}
  constexpr I next(I x, difference_type_t<I> n, S bound)
  STL2_NOEXCEPT_RETURN(
    __stl2::advance(x, n, bound), x
  )

  // prev
  BidirectionalIterator{I}
  constexpr I prev(I x, difference_type_t<I> n = 1)
  STL2_NOEXCEPT_RETURN(
    __stl2::advance(x, -n), x
  )

  template <BidirectionalIterator I, Sentinel<I> S>
  constexpr I prev(I x, difference_type_t<I> n, S bound)
  STL2_NOEXCEPT_RETURN(
    __stl2::advance(x, -n, bound), x
  )

  namespace ext {
    Sentinel{S, I}
    constexpr tagged_pair<tag::count(difference_type_t<I>), tag::end(I)>
    enumerate(I first, S last)
      noexcept(noexcept(bool(first != last), void(), ++first)) {
      difference_type_t<I> n = 0;
      while (first != last) {
        ++n;
        ++first;
      }
      return {n, first};
    }

    // Don't use SizedIteratorRange{I, S} here: GCC PR 67545.
    template <class S, class I>
      requires SizedIteratorRange<I, S>()
    constexpr auto enumerate(I first, S last)
    STL2_NOEXCEPT_RETURN(
      tagged_pair<tag::count(difference_type_t<I>), tag::end(I)>{
        last - first,
         __stl2::next(__stl2::move(first), __stl2::move(last))
      }
    )
  }

  // distance
  Sentinel{S, I}
  constexpr difference_type_t<I> distance(I first, S last)
  STL2_NOEXCEPT_RETURN(
    ext::enumerate(__stl2::move(first), __stl2::move(last)).first
  )

  // Don't use SizedIteratorRange{I, S} here: GCC PR 67545.
  template <class S, class I>
    requires SizedIteratorRange<I, S>()
  constexpr difference_type_t<I> distance(I first, S last)
  STL2_NOEXCEPT_RETURN(
    difference_type_t<I>(last - first)
  )
} STL2_CLOSE_NAMESPACE

#endif
