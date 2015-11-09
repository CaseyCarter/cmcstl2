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
#ifndef STL2_DETAIL_ALGORITHM_NTH_ELEMENT_HPP
#define STL2_DETAIL_ALGORITHM_NTH_ELEMENT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/min_element.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// nth_element [alg.nth.element]
//
STL2_OPEN_NAMESPACE {
  namespace detail {
    // stable, 2-3 compares, 0-2 swaps
    Sortable{I, C, P}
    unsigned sort3(I x, I y, I z, C& comp, P& proj)
    {
      if (!comp(proj(*y), proj(*x))) {      // if x <= y
        if (!comp(proj(*z), proj(*y))) {    // if y <= z
          return 0;                         // x <= y && y <= z
        }
                                            // x <= y && y > z
        __stl2::iter_swap(y, z);           // x <= z && y < z
        if (comp(proj(*y), proj(*x))) {     // if x > y
          __stl2::iter_swap(x, y);         // x < y && y <= z
          return 2;
        }
        return 1;                           // x <= y && y < z
      }
      if (comp(proj(*z), proj(*y))) {       // x > y, if y > z
        __stl2::iter_swap(x, z);           // x < y && y < z
        return 1;
      }
      __stl2::iter_swap(x, y);             // x > y && y <= z
                                            // x < y && x <= z
      if (comp(proj(*z), proj(*y))) {       // if y > z
        __stl2::iter_swap(y, z);           // x <= y && y < z
        return 2;
      }
      return 1;
    }

    template <BidirectionalIterator I, class C, class P>
    requires Sortable<I, C, P>()
    void selection_sort(I begin, I end, C &comp, P &proj)
    {
      STL2_ASSUME(begin != end);
      for (I lm1 = __stl2::prev(end); begin != lm1; ++begin) {
        I i = __stl2::min_element(begin, end, __stl2::ref(comp), __stl2::ref(proj));
        if (i != begin) {
          __stl2::iter_swap(begin, i);
        }
      }
    }
  }

  // TODO: refactor this monstrosity.
  template <RandomAccessIterator I, Sentinel<I> S, class Comp = less<>,
            class Proj = identity>
  requires Sortable<I, __f<Comp>, __f<Proj>>()
  I nth_element(I first, I nth, S last, Comp&& comp_ = Comp{}, Proj&& proj_ = Proj{})
  {
    auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
    I end = __stl2::next(nth, last), end_orig = end;
    static constexpr difference_type_t<I> limit = 7;
    while (true) {
    restart:
      if (nth == end) {
        return end_orig;
      }
      difference_type_t<I> len = end - first;
      switch (len) {
      case 0:
      case 1:
        return end_orig;
      case 2:
        if (comp(proj(*--end), proj(*first))) {
          __stl2::iter_swap(first, end);
        }
        return end_orig;
      case 3:
        {
          I m = first;
          detail::sort3(first, ++m, --end, comp, proj);
          return end_orig;
        }
      }
      if (len <= limit) {
        detail::selection_sort(first, end, comp, proj);
        return end_orig;
      }
      static_assert(limit >= 3);
      // len > limit >= 3
      I m = first + len/2;
      I lm1 = end;
      unsigned n_swaps = detail::sort3(first, m, --lm1, comp, proj);
      // *m is median
      // partition [first, m) < *m and *m <= [m, end)
      //(this inhibits tossing elements equivalent to m around unnecessarily)
      I i = first;
      I j = lm1;
      // j points beyond range to be tested, *lm1 is known to be <= *m
      // The search going up is known to be guarded but the search coming down isn't.
      // Prime the downward search with a guard.
      if (!comp(proj(*i), proj(*m))) {  // if *first == *m
        // *first == *m, *first doesn't go in first part
        // manually guard downward moving j against i
        while (true) {
          if (i == --j) {
            // *first == *m, *m <= all other elements
            // Parition instead into [first, i) == *first and *first < [i, end)
            ++i;  // first + 1
            j = end;
            if (!comp(proj(*first), proj(*--j))) {  // we need a guard if *first == *(end-1)
              while (true) {
                if (i == j) {
                  return end_orig;  // [first, end) all equivalent elements
                }
                if (comp(proj(*first), proj(*i))) {
                  __stl2::iter_swap(i, j);
                  ++n_swaps;
                  ++i;
                  break;
                }
                ++i;
              }
            }
            // [first, i) == *first and *first < [j, end) and j == end - 1
            if (i == j) {
              return end_orig;
            }
            while (true) {
              while (!comp(proj(*first), proj(*i)))
                ++i;
              while (comp(proj(*first), proj(*--j)))
                ;
              if (i >= j)
                break;
              __stl2::iter_swap(i, j);
              ++n_swaps;
              ++i;
            }
            // [first, i) == *first and *first < [i, end)
            // The first part is sorted,
            if (nth < i) {
              return end_orig;
            }
            // nth_element the second part
            // nth_element<C>(i, nth, end, comp);
            first = i;
            goto restart;
          }
          if (comp(proj(*j), proj(*m))) {
            __stl2::iter_swap(i, j);
            ++n_swaps;
            break;  // found guard for downward moving j, now use unguarded partition
          }
        }
      }
      ++i;
      // j points beyond range to be tested, *lm1 is known to be <= *m
      // if not yet partitioned...
      if (i < j) {
        // known that *(i - 1) < *m
        while (true) {
          // m still guards upward moving i
          while (comp(proj(*i), proj(*m))) {
            ++i;
          }
          // It is now known that a guard exists for downward moving j
          while (!comp(proj(*--j), proj(*m))) {
            ;
          }
          if (i >= j) {
            break;
          }
          __stl2::iter_swap(i, j);
          ++n_swaps;
          // It is known that m != j
          // If m just moved, follow it
          if (m == i) {
            m = j;
          }
          ++i;
        }
      }
      // [first, i) < *m and *m <= [i, end)
      if (i != m && comp(proj(*m), proj(*i))) {
        __stl2::iter_swap(i, m);
        ++n_swaps;
      }
      // [first, i) < *i and *i <= [i+1, end)
      if (nth == i) {
        return end_orig;
      }
      if (n_swaps == 0) {
        // We were given a perfectly partitioned sequence.  Coincidence?
        if (nth < i) {
          // Check for [first, i) already sorted
          j = m = first;
          while (++j != i) {
            if (comp(proj(*j), proj(*m))) {
              // not yet sorted, so sort
              goto not_sorted;
            }
            m = j;
          }
          // [first, i) sorted
          return end_orig;
        } else {
          // Check for [i, end) already sorted
          j = m = i;
          while (++j != end) {
            if (comp(proj(*j), proj(*m))) {
              // not yet sorted, so sort
              goto not_sorted;
            }
            m = j;
          }
          // [i, end) sorted
          return end_orig;
        }
      }
    not_sorted:
      // nth_element on range containing nth
      if (nth < i) {
        // nth_element<C>(first, nth, i, comp);
        end = i;
      } else {
        // nth_element<C>(i+1, nth, end, comp);
        first = ++i;
      }
    }
    return end_orig;
  }

  template <RandomAccessRange Rng, class Comp = less<>, class Proj = identity>
  requires Sortable<iterator_t<Rng>, __f<Comp>, __f<Proj>>()
  safe_iterator_t<Rng>
  nth_element(Rng&& rng, iterator_t<Rng> nth,
              Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
    return __stl2::nth_element(
      __stl2::begin(rng), __stl2::move(nth), __stl2::end(rng),
      __stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
