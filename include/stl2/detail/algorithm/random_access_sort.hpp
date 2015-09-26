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
#ifndef STL2_DETAIL_ALGORITHM_RANDOM_ACCESS_SORT_HPP
#define STL2_DETAIL_ALGORITHM_RANDOM_ACCESS_SORT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/tuple.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/move_backward.hpp>
#include <stl2/detail/algorithm/partial_sort.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/fundamental.hpp>

///////////////////////////////////////////////////////////////////////////
// sort [sort]
//
STL2_OPEN_NAMESPACE {
  namespace detail {
    namespace rsort {
      template <RandomAccessIterator I, class Comp, class Proj>
        requires Sortable<I, Comp, Proj>()
      I choose_pivot(I first, I last, Comp& comp, Proj& proj) {
        STL2_ASSERT(first != last);
        I mid = first + DifferenceType<I>(last - first) / 2;
        --last;
        // Find the median:
        return [&](auto&& a, auto&& b, auto&& c) {
          return comp(a, b)
            ? (comp(b, c) ? mid   : (comp(a, c) ? last : first))
            : (comp(a, c) ? first : (comp(b, c) ? last : mid  ));
        }(proj(*first), proj(*mid), proj(*last));
      }

      template <RandomAccessIterator I, class Comp, class Proj>
        requires Sortable<I, Comp, Proj>()
      I unguarded_partition(I first, I last, Comp& comp, Proj& proj) {
        I pivot_pnt = rsort::choose_pivot(first, last, comp, proj);

        // Do the partition:
        while (true) {
          auto &&v = *pivot_pnt;
          auto &&pivot = proj((decltype(v) &&)v);
          while (comp(proj(*first), pivot)) {
            ++first;
          }
          while (comp(pivot, proj(*--last))) {
            ;
          }
          if (!(first < last)) {
            return first;
          }
          __stl2::iter_swap2(first, last);
          pivot_pnt = pivot_pnt == first ? last : (pivot_pnt == last ? first : pivot_pnt);
          ++first;
        }
      }

      template <RandomAccessIterator I, class Comp, class Proj>
        requires Sortable<I, Comp, Proj>()
      void unguarded_linear_insert(I last, ValueType<I> val, Comp& comp, Proj& proj) {
        I next = __stl2::prev(last);
        while (comp(proj(val), proj(*next))) {
          *last = __stl2::iter_move(next);
          last = next;
          --next;
        }
        *last = __stl2::move(val);
      }

      template <RandomAccessIterator I, class Comp, class Proj>
        requires Sortable<I, Comp, Proj>()
      void linear_insert(I first, I last, Comp& comp, Proj& proj)
      {
        ValueType<I> val = __stl2::iter_move(last);
        if (comp(proj(val), proj(*first))) {
          __stl2::move_backward(first, last, last + 1);
          *first = __stl2::move(val);
        } else {
          rsort::unguarded_linear_insert(last, __stl2::move(val), comp, proj);
        }
      }

      template <RandomAccessIterator I, class Comp, class Proj>
        requires Sortable<I, Comp, Proj>()
      void insertion_sort(I first, I last, Comp& comp, Proj& proj) {
        if (first != last) {
          for (I i = __stl2::next(first); i != last; ++i) {
            rsort::linear_insert(first, i, comp, proj);
          }
        }
      }

      template <RandomAccessIterator I, class Comp, class Proj>
        requires Sortable<I, Comp, Proj>()
      void unguarded_insertion_sort(I first, I last, Comp& comp, Proj& proj) {
        for (I i = first; i != last; ++i) {
          rsort::unguarded_linear_insert(i, __stl2::iter_move(i), comp, proj);
        }
      }

      constexpr std::ptrdiff_t introsort_threshold = 16;

      Integral{I}
      constexpr auto log2(I n) {
        I k = 0;
        for (; n != 1; n /= 2) {
          ++k;
        }
        return k;
      }

      template <RandomAccessIterator I, class Comp, class Proj>
        requires Sortable<I, Comp, Proj>()
      void introsort_loop(I first, I last, DifferenceType<I> depth_limit,
                          Comp& comp, Proj& proj) {
        while (__stl2::distance(first, last) > introsort_threshold) {
          if (depth_limit == 0) {
            __stl2::partial_sort(first, last, last, __stl2::ref(comp),
                                 __stl2::ref(proj));
            return;
          }
          I cut = rsort::unguarded_partition(first, last, comp, proj);
          rsort::introsort_loop(cut, last, --depth_limit, comp, proj);
          last = cut;
        }
      }

      template <RandomAccessIterator I, class Comp, class Proj>
        requires Sortable<I, Comp, Proj>()
      void final_insertion_sort(I first, I last, Comp &comp, Proj &proj)
      {
        if (__stl2::distance(first, last) > introsort_threshold) {
          rsort::insertion_sort(first, first + introsort_threshold,
                               comp, proj);
          rsort::unguarded_insertion_sort(first + introsort_threshold,
                                         last, comp, proj);
        } else {
          rsort::insertion_sort(first, last, comp, proj);
        }
      }
    }
  }
} STL2_CLOSE_NAMESPACE

#endif
