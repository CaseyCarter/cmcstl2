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
// Copyright (c) 2009 Alexander Stepanov and Paul McJones
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without
// fee, provided that the above copyright notice appear in all copies
// and that both that copyright notice and this permission notice
// appear in supporting documentation. The authors make no
// representations about the suitability of this software for any
// purpose. It is provided "as is" without express or implied
// warranty.
//
// Algorithms from
// Elements of Programming
// by Alexander Stepanov and Paul McJones
// Addison-Wesley Professional, 2009
//
#ifndef STL2_DETAIL_ALGORITHM_FORWARD_SORT_HPP
#define STL2_DETAIL_ALGORITHM_FORWARD_SORT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/temporary_vector.hpp>
#include <stl2/detail/algorithm/lower_bound.hpp>
#include <stl2/detail/algorithm/merge_move.hpp>
#include <stl2/detail/algorithm/rotate.hpp>
#include <stl2/detail/algorithm/upper_bound.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// sort [Extension]
//
// Algorithm for forward iterators from EoP.
//
STL2_OPEN_NAMESPACE {
  namespace detail {
    namespace fsort {
      template <class I>
      using buf_t = temporary_buffer<ValueType<I>>;

      Sortable{I, Comp, Proj}
      inline I merge_n_with_buffer(I f0, DifferenceType<I> n0,
                                   I f1, DifferenceType<I> n1,
                                   buf_t<I>& buf, Comp& comp, Proj& proj) {
        STL2_ASSUME(n0 <= buf.size());
        auto&& vec = make_temporary_vector(buf);
        __stl2::move(__stl2::make_counted_iterator(f0, n0),
                     __stl2::default_sentinel{},
                     __stl2::back_inserter(vec));
        return __stl2::merge_move(
          vec.begin(), vec.end(),
          __stl2::make_counted_iterator(__stl2::move(f1), n1),
          __stl2::default_sentinel{},
          __stl2::move(f0), __stl2::ref(comp),
          __stl2::ref(proj), __stl2::ref(proj)).out();
      }

      Sortable{I, Comp, Proj}
      inline void merge_n_step_0(I f0, DifferenceType<I> n0,
                                 I f1, DifferenceType<I> n1,
                                 Comp& comp, Proj& proj,
                                 I& f0_0, DifferenceType<I>& n0_0,
                                 I& f0_1, DifferenceType<I>& n0_1,
                                 I& f1_0, DifferenceType<I>& n1_0,
                                 I& f1_1, DifferenceType<I>& n1_1) {
        f0_0 = f0;
        n0_0 = n0 / 2;
        f0_1 = __stl2::next(f0_0, n0_0);
        f1_1 = __stl2::ext::lower_bound_n(f1, n1, proj(*f0_1),
                                          __stl2::ref(comp), __stl2::ref(proj));
        f1_0 = __stl2::rotate(f0_1, f1, f1_1).begin();
        n0_1 = __stl2::distance(f0_1, f1_0);
        f1_0 = __stl2::next(f1_0);
        n1_0 = n0 - n0_0 - 1;
        n1_1 = n1 - n0_1;
      }

      Sortable{I, Comp, Proj}
      inline void merge_n_step_1(I f0, DifferenceType<I> n0,
                                 I f1, DifferenceType<I> n1,
                                 Comp& comp, Proj& proj,
                                 I& f0_0, DifferenceType<I>& n0_0,
                                 I& f0_1, DifferenceType<I>& n0_1,
                                 I& f1_0, DifferenceType<I>& n1_0,
                                 I& f1_1, DifferenceType<I>& n1_1) {
        f0_0 = f0;
        n0_1 = n1 / 2;
        f1_1 = __stl2::next(f1, n0_1);
        f0_1 = __stl2::ext::upper_bound_n(f0, n0, proj(*f1_1),
                                          __stl2::ref(comp), __stl2::ref(proj));
        f1_1 = __stl2::next(f1_1);
        f1_0 = __stl2::rotate(f0_1, f1, f1_1).begin();
        n0_0 = __stl2::distance(f0_0, f0_1);
        n1_0 = n0 - n0_0;
        n1_1 = n1 - n0_1 - 1;
      }

      Sortable{I, Comp, Proj}
      I merge_n_adaptive(I f0, DifferenceType<I> n0,
                         I f1, DifferenceType<I> n1,
                         buf_t<I>& buf, Comp& comp, Proj& proj) {
        if (n0 <= buf.size()) {
          if (!n0 || !n1) {
            return __stl2::next(f0, n0 + n1);
          }
          return fsort::merge_n_with_buffer(f0, n0, f1, n1, buf, comp, proj);
        }
        I f0_0, f0_1, f1_0, f1_1;
        DifferenceType<I> n0_0, n0_1, n1_0, n1_1;

        if (n0 < n1) {
                 fsort::merge_n_step_0(f0, n0, f1, n1, comp, proj,
                                       f0_0, n0_0, f0_1, n0_1,
                                       f1_0, n1_0, f1_1, n1_1);
        } else {
                 fsort::merge_n_step_1(f0, n0, f1, n1, comp, proj,
                                       f0_0, n0_0, f0_1, n0_1,
                                       f1_0, n1_0, f1_1, n1_1);
        }
               fsort::merge_n_adaptive(f0_0, n0_0, f0_1, n0_1,
                                       buf, comp, proj);
        return fsort::merge_n_adaptive(f1_0, n1_0, f1_1, n1_1,
                                       buf, comp, proj);
      }

      Sortable{I, Comp, Proj}
      I sort_n_adaptive(I first, const DifferenceType<I> n, buf_t<I>& buf,
                        Comp& comp, Proj& proj) {
        auto half_n = n / 2;
        if (!half_n) {
          return __stl2::next(first, n);
        }
        I middle = fsort::sort_n_adaptive(first, half_n, buf, comp, proj);
        fsort::sort_n_adaptive(middle, n - half_n, buf, comp, proj);
        return fsort::merge_n_adaptive(first, half_n, middle, n - half_n,
                                       buf, comp, proj);
      }

      template <class I, class Comp = less<>, class Proj = identity>
        requires Sortable<I, Comp, Proj>()
      inline I sort_n(I first, const DifferenceType<I> n,
                      Comp comp_ = Comp{}, Proj proj_ = Proj{}) {
        auto&& comp = __stl2::as_function(comp_);
        auto&& proj = __stl2::as_function(proj_);
        auto ufirst = ext::uncounted(first);
        using buf_t = temporary_buffer<ValueType<decltype(ufirst)>>;
        // TODO: tune this threshold.
        auto buf = n / 2 >= 16 ? buf_t{n / 2} : buf_t{};
        auto last = detail::fsort::sort_n_adaptive(ufirst, n, buf, comp, proj);
        return ext::recounted(first, __stl2::move(last), n);
      }
    }
  }
} STL2_CLOSE_NAMESPACE

#endif
