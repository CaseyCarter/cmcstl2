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
#ifndef STL2_DETAIL_ALGORITHM_SEARCH_N_HPP
#define STL2_DETAIL_ALGORITHM_SEARCH_N_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>

///////////////////////////////////////////////////////////////////////////
// search_n [alg.search]
//
STL2_OPEN_NAMESPACE {
  namespace __search_n {
    template <ForwardIterator I, Sentinel<I> S, class T,
              class Pred, class Proj>
    requires
      models::IndirectlyComparable<I, const T*, __f<Pred>, __f<Proj>>
    I unsized(I first, S last, difference_type_t<I> count,
              const T& value, Pred&& pred_, Proj&& proj_)
    {
      if (count <= 0) {
        return first;
      }

      auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
      auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));

      for (; first != last; ++first) {
        if (pred(proj(*first), value)) {
          auto saved = first;
          difference_type_t<I> n = 0;
          do {
            if (++n == count) {
              return saved;
            }
            if (++first == last) {
              return first;
            }
          } while(pred(proj(*first), value));
        }
      }

      return first;
    }

    template <ForwardIterator I, Sentinel<I> S, class T,
              class Pred, class Proj>
    requires
      models::IndirectlyComparable<I, const T*, __f<Pred>, __f<Proj>>
    I sized(I first_, S last, difference_type_t<I> d_,
            difference_type_t<I> count,
            const T& value, Pred&& pred_, Proj&& proj_)
    {
      if (count <= 0) {
        return first_;
      }

      auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
      auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));

      auto d = d_;
      auto first = ext::uncounted(first_);

      for (; d >= count; ++first, --d) {
        if (pred(proj(*first), value)) {
          // *first matches val, now match elements after here
          auto saved = first;
          difference_type_t<I> n = 0;
          do {
            if (++n == count) {
              // Pattern exhausted, saved is the answer
              // (works for 1 element pattern)
              return ext::recounted(first_, __stl2::move(saved), d_ - d);
            }
          } while (pred(proj(*++first), value));
          d -= n;
        }
      }

      return __stl2::next(ext::recounted(first_, __stl2::move(first), d_ - d),
                          __stl2::move(last));
    }
  }

  template <ForwardIterator I, Sentinel<I> S, class T,
            class Pred = equal_to<>, class Proj = identity>
  requires
    models::IndirectlyComparable<I, const T*, __f<Pred>, __f<Proj>>
  I search_n(I first, S last, difference_type_t<I> count,
             const T& value, Pred&& pred = Pred{}, Proj&& proj = Proj{})
  {
    return __search_n::unsized(__stl2::move(first), __stl2::move(last),
                               count, value,
                               __stl2::forward<Pred>(pred),
                               __stl2::forward<Proj>(proj));
  }

  template <ForwardIterator I, Sentinel<I> S, class T,
            class Pred = equal_to<>, class Proj = identity>
  requires
    SizedIteratorRange<I, S>() &&
    models::IndirectlyComparable<I, const T*, __f<Pred>, __f<Proj>>
  I search_n(I first, S last, difference_type_t<I> count,
             const T& value, Pred&& pred = Pred{}, Proj&& proj = Proj{})
  {
    return __search_n::sized(__stl2::move(first), __stl2::move(last),
                             __stl2::distance(first, last),
                             count, value,
                             __stl2::forward<Pred>(pred),
                             __stl2::forward<Proj>(proj));
  }

  template <ForwardRange Rng, class T,
            class Pred = equal_to<>, class Proj = identity>
  requires
    models::IndirectlyComparable<
      iterator_t<Rng>, const T*, __f<Pred>, __f<Proj>>
  safe_iterator_t<Rng>
  search_n(Rng&& rng, difference_type_t<iterator_t<Rng>> count,
           const T& value, Pred&& pred = Pred{}, Proj&& proj = Proj{})
  {
    return __search_n::unsized(
      __stl2::begin(rng), __stl2::end(rng), count, value,
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }

  template <ForwardRange Rng, class T,
            class Pred = equal_to<>, class Proj = identity>
  requires
    SizedRange<Rng>() &&
    models::IndirectlyComparable<
      iterator_t<Rng>, const T*, __f<Pred>, __f<Proj>>
  safe_iterator_t<Rng>
  search_n(Rng&& rng, difference_type_t<iterator_t<Rng>> count,
           const T& value, Pred&& pred = Pred{}, Proj&& proj = Proj{})
  {
    return __search_n::sized(
      __stl2::begin(rng), __stl2::end(rng),
      __stl2::distance(rng), count, value,
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }

  template <class E, class T,
            class Pred = equal_to<>, class Proj = identity>
  requires
    models::IndirectlyComparable<
      const E*, const T*, __f<Pred>, __f<Proj>>
  dangling<const E*>
  search_n(std::initializer_list<E>&& rng, std::ptrdiff_t count,
           const T& value, Pred&& pred = Pred{}, Proj&& proj = Proj{})
  {
    return __search_n::sized(
      __stl2::begin(rng), __stl2::end(rng),
      __stl2::distance(rng), count, value,
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
