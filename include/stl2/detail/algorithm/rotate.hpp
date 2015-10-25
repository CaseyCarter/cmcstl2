// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
#ifndef STL2_DETAIL_ALGORITHM_ROTATE_HPP
#define STL2_DETAIL_ALGORITHM_ROTATE_HPP

#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/algorithm/move_backward.hpp>
#include <stl2/detail/algorithm/swap_ranges.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/fundamental.hpp>

///////////////////////////////////////////////////////////////////////////
// rotate [alg.rotate]
//
STL2_OPEN_NAMESPACE {
  template <Iterator I>
  using __tagged_range = tagged_pair<tag::begin(I), tag::end(I)>;

  Permutable{I}
  __tagged_range<I> __rotate_left(I first, I last) {
    value_type_t<I> tmp = __stl2::iter_move(first);
    I lm1 = __stl2::move(__stl2::next(first), last, first).second;
    *lm1 = __stl2::move(tmp);
    return {lm1, last};
  }

  template <BidirectionalIterator I>
    requires Permutable<I>()
  __tagged_range<I> __rotate_right(I first, I last) {
    I lm1 = __stl2::prev(last);
    value_type_t<I> tmp = __stl2::iter_move(lm1);
    I fp1 = __stl2::move_backward(first, lm1, last).second;
    *first = __stl2::move(tmp);
    return {fp1, last};
  }

  template <Permutable I, Sentinel<I> S>
  __tagged_range<I> __rotate_forward(I first, I middle, S last) {
    I i = middle;
    while (true) {
      __stl2::iter_swap(first, i);
      ++first;
      if (++i == last) {
        break;
      }
      if (first == middle) {
        middle = i;
      }
    }
    I r = first;
    if (first != middle) {
      I j = middle;
      while (true) {
        __stl2::iter_swap(first, j);
        ++first;
        if (++j == last) {
          if (first == middle) {
            break;
          }
          j = middle;
        } else if(first == middle) {
          middle = j;
        }
      }
    }
    return {r, i};
  }

  constexpr Integral __gcd(Integral x, Integral y) {
    do {
      auto t = x % y;
      x = y;
      y = t;
    } while(y);
    return x;
  }

  template<RandomAccessIterator I>
    requires Permutable<I>()
  __tagged_range<I> __rotate_gcd(I first, I middle, I last) {
    auto const m1 = middle - first;
    auto const m2 = last - middle;
    if (m1 == m2) {
      __stl2::swap_ranges(first, middle, middle);
      return {middle, last};
    }
    auto const g = __stl2::__gcd(m1, m2);
    for (I p = first + g; p != first;) {
      value_type_t<I> t = __stl2::iter_move(--p);
      I p1 = p;
      I p2 = p1 + m1;
      do {
        *p1 = __stl2::iter_move(p2);
        p1 = p2;
        auto const d = last - p2;
        if (m1 < d) {
          p2 += m1;
        } else {
          p2 = first + (m1 - d);
        }
      } while(p2 != p);
      *p1 = __stl2::move(t);
    }
    return {first + m2, last};
  }

  template <Permutable I, Sentinel<I> S>
  __tagged_range<I> __rotate(I first, I middle, S last) {
    return __stl2::__rotate_forward(
      __stl2::move(first), __stl2::move(middle), __stl2::move(last));
  }

  ForwardIterator{I}
  __tagged_range<I> __rotate(I first, I middle, I last) {
    if (is_trivially_move_assignable<value_type_t<I>>()) {
      if (__stl2::next(first) == middle) {
        return __stl2::__rotate_left(__stl2::move(first), __stl2::move(last));
      }
    }
    return __stl2::__rotate_forward(
      __stl2::move(first), __stl2::move(middle), __stl2::move(last));
  }

  BidirectionalIterator{I}
  __tagged_range<I> __rotate(I first, I middle, I last) {
    if (is_trivially_move_assignable<value_type_t<I>>()) {
      if (__stl2::next(first) == middle) {
        return __stl2::__rotate_left(__stl2::move(first), __stl2::move(last));
      }
      if (__stl2::next(middle) == last) {
        return __stl2::__rotate_right(__stl2::move(first), __stl2::move(last));
      }
    }
    return __stl2::__rotate_forward(
      __stl2::move(first), __stl2::move(middle), __stl2::move(last));
  }

  RandomAccessIterator{I}
  __tagged_range<I> __rotate(I first, I middle, I last) {
    if (is_trivially_move_assignable<value_type_t<I>>()) {
      if (__stl2::next(first) == middle) {
        return __stl2::__rotate_left(__stl2::move(first), __stl2::move(last));
      }
      if (__stl2::next(middle) == last) {
        return __stl2::__rotate_right(__stl2::move(first), __stl2::move(last));
      }
      return __stl2::__rotate_gcd(
        __stl2::move(first), __stl2::move(middle), __stl2::move(last));
    }
    return __stl2::__rotate_forward(
      __stl2::move(first), __stl2::move(middle), __stl2::move(last));
  }

  template <Permutable I, Sentinel<I> S>
  __tagged_range<I> rotate(I first, I middle, S last) {
    if (first == middle) {
      first = __stl2::next(__stl2::move(first), __stl2::move(last));
      return {first, first};
    }
    if (middle == last) {
      return {first, middle};
    }
    return __stl2::__rotate(
      __stl2::move(first), __stl2::move(middle), __stl2::move(last));
  }

  template <ForwardRange Rng>
    requires Permutable<iterator_t<Rng>>()
  __tagged_range<safe_iterator_t<Rng>> rotate(Rng&& rng, iterator_t<Rng> middle) {
    return __stl2::rotate(__stl2::begin(rng), __stl2::move(middle), __stl2::end(rng));
  }
} STL2_CLOSE_NAMESPACE

#endif
