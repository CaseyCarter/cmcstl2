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
  STL2_CONSTEXPR_EXT void advance(I& i, difference_type_t<I> n) {
    STL2_ASSUME(0 <= n);
    while (n != 0) {
      --n;
      ++i;
    }
  }

  BidirectionalIterator{I}
  STL2_CONSTEXPR_EXT void advance(I& i, difference_type_t<I> n) {
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
  STL2_CONSTEXPR_EXT void advance(I& i, difference_type_t<I> n) {
    i += n;
  }

  // Pre: bound is reachable from i
  template <Iterator I, Sentinel<I> S>
  STL2_CONSTEXPR_EXT void advance(I& i, S bound) {
    while (i != bound) {
      ++i;
    }
  }

  template <Iterator I, Sentinel<I> S>
  requires
    models::Assignable<I&, S&&>
  STL2_CONSTEXPR_EXT void advance(I& i, S bound) {
    i = __stl2::move(bound);
  }

  // Pre: 0 <= (bound - i) || BidirectionalIterator<I>()
  template <Iterator I, Sentinel<I> S>
  requires
    !models::Assignable<I&, S&&> &&
    models::SizedIteratorRange<I, S>
  STL2_CONSTEXPR_EXT void advance(I& i, S bound) {
    __stl2::advance(i, bound - i);
  }

  Sentinel{S, I}
  STL2_CONSTEXPR_EXT difference_type_t<I>
  advance(I& i, difference_type_t<I> n, S bound) {
    while (i != bound && n != 0) {
      ++i;
      --n;
    }
    return n;
  }

  template <class S, class I>
    requires BidirectionalIterator<I>() && Sentinel<S, I>()
  STL2_CONSTEXPR_EXT difference_type_t<I>
  advance(I& i, difference_type_t<I> n, S bound) {
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
  STL2_CONSTEXPR_EXT difference_type_t<I>
  advance(I& i, difference_type_t<I> n, S bound) {
    const auto D = difference_type_t<I>{bound - i};
    if (n >= 0 ? n >= D : n <= D) {
      __stl2::advance(i, __stl2::move(bound));
      return n - D;
    } else {
      __stl2::advance(i, n);
      return 0;
    }
  }

  // next
  WeakIterator{I}
  STL2_CONSTEXPR_EXT I next(I x, difference_type_t<I> n = 1) {
    __stl2::advance(x, n);
    return x;
  }

  Sentinel{S, I}
  STL2_CONSTEXPR_EXT I next(I x, S bound) {
    __stl2::advance(x, __stl2::move(bound));
    return x;
  }

  Sentinel{S, I}
  STL2_CONSTEXPR_EXT I next(I x, difference_type_t<I> n, S bound) {
    __stl2::advance(x, n, __stl2::move(bound));
    return x;
  }

  // prev
  BidirectionalIterator{I}
  STL2_CONSTEXPR_EXT I prev(I x, difference_type_t<I> n = 1) {
    __stl2::advance(x, -n);
    return x;
  }

  template <BidirectionalIterator I, Sentinel<I> S>
  STL2_CONSTEXPR_EXT I prev(I x, difference_type_t<I> n, S bound) {
    __stl2::advance(x, -n, __stl2::move(bound));
    return x;
  }

  namespace ext {
    Sentinel{S, I}
    STL2_CONSTEXPR_EXT tagged_pair<tag::count(difference_type_t<I>), tag::end(I)>
    enumerate(I first, S last) {
      difference_type_t<I> n = 0;
      while (first != last) {
        ++n;
        ++first;
      }
      return {n, __stl2::move(first)};
    }

    // Don't use SizedIteratorRange{I, S} here: GCC PR 67545.
    template <class S, class I>
    requires
      SizedIteratorRange<I, S>()
    STL2_CONSTEXPR_EXT tagged_pair<tag::count(difference_type_t<I>), tag::end(I)>
    enumerate(I first, S last) {
      return {
        last - first,
         __stl2::next(__stl2::move(first), __stl2::move(last))
      };
    }
  }

  // distance
  Sentinel{S, I}
  STL2_CONSTEXPR_EXT difference_type_t<I> distance(I first, S last) {
    return ext::enumerate(__stl2::move(first), __stl2::move(last)).first;
  }

  // Don't use SizedIteratorRange{I, S} here: GCC PR 67545.
  template <class S, class I>
    requires SizedIteratorRange<I, S>()
  STL2_CONSTEXPR_EXT difference_type_t<I> distance(I first, S last) {
    return last - first;
  }
} STL2_CLOSE_NAMESPACE

#endif
