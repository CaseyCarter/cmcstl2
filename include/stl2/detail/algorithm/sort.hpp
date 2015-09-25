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
#ifndef STL2_DETAIL_ALGORITHM_SORT_HPP
#define STL2_DETAIL_ALGORITHM_SORT_HPP

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
// TODO:
// * ForwardRanges
//
STL2_OPEN_NAMESPACE {
  namespace detail {
    namespace sort {
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
        I pivot_pnt = sort::choose_pivot(first, last, comp, proj);

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
          sort::unguarded_linear_insert(last, __stl2::move(val), comp, proj);
        }
      }

      template <RandomAccessIterator I, class Comp, class Proj>
        requires Sortable<I, Comp, Proj>()
      void insertion_sort(I first, I last, Comp& comp, Proj& proj) {
        if (first != last) {
          for (I i = __stl2::next(first); i != last; ++i) {
            sort::linear_insert(first, i, comp, proj);
          }
        }
      }

      template <RandomAccessIterator I, class Comp, class Proj>
        requires Sortable<I, Comp, Proj>()
      void unguarded_insertion_sort(I first, I last, Comp& comp, Proj& proj) {
        for (I i = first; i != last; ++i) {
          sort::unguarded_linear_insert(i, __stl2::iter_move(i), comp, proj);
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
          I cut = sort::unguarded_partition(first, last, comp, proj);
          sort::introsort_loop(cut, last, --depth_limit, comp, proj);
          last = cut;
        }
      }

      template <RandomAccessIterator I, class Comp, class Proj>
        requires Sortable<I, Comp, Proj>()
      void final_insertion_sort(I first, I last, Comp &comp, Proj &proj)
      {
        if (__stl2::distance(first, last) > introsort_threshold) {
          sort::insertion_sort(first, first + introsort_threshold,
                               comp, proj);
          sort::unguarded_insertion_sort(first + introsort_threshold,
                                         last, comp, proj);
        } else {
          sort::insertion_sort(first, last, comp, proj);
        }
      }
    }
  }

  template <RandomAccessIterator I, Sentinel<I> S, class Comp = less<>,
            class Proj = identity>
    requires Sortable<I, Comp, Proj>()
  I sort(I first, S sent, Comp comp_ = Comp{}, Proj proj_ = Proj{}) {
    if (first == sent) {
      return first;
    }
    I last = __stl2::next(first, __stl2::move(sent));
    auto&& comp = __stl2::as_function(comp_);
    auto&& proj = __stl2::as_function(proj_);
    auto n = DifferenceType<I>(last - first);
    detail::sort::introsort_loop(first, last, detail::sort::log2(n) * 2,
                                 comp, proj);
    detail::sort::final_insertion_sort(first, last, comp, proj);
    return last;
  }

  template <RandomAccessRange Rng, class Comp = less<>, class Proj = identity>
    requires Sortable<IteratorType<Rng>, Comp, Proj>()
  safe_iterator_t<Rng>
  sort(Rng&& rng, Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
    return __stl2::sort(__stl2::begin(rng), __stl2::end(rng),
      __stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
