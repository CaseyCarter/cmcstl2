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
#ifndef STL2_DETAIL_ALGORITHM_STABLE_PARTITION_HPP
#define STL2_DETAIL_ALGORITHM_STABLE_PARTITION_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/temporary_vector.hpp>
#include <stl2/detail/algorithm/find_if_not.hpp>
#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/algorithm/rotate.hpp>
#include <stl2/detail/algorithm/partition_move.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// stable_partition [alg.partitions]
//
STL2_OPEN_NAMESPACE {
  namespace detail {
    namespace stable_part {
      Readable{I}
      using buf_t = detail::temporary_buffer<value_type_t<I>>;

      template <ForwardIterator I, class Proj,
                IndirectCallablePredicate<projected<I, Proj>> Pred>
      void skip_true(I& first, difference_type_t<I>& n, Pred& pred, Proj& proj)
      {
        // advance "first" past values that satisfy the predicate.
        // Ensures: n == 0 || !pred(proj(*first))
        STL2_ASSUME(n >= 0);
        while (n != 0 && pred(proj(*first))) {
          ++first;
          --n;
        }
      }

      template <ForwardIterator I, class Proj,
                IndirectCallablePredicate<projected<I, Proj>> Pred>
      requires models::Permutable<I>
      tagged_pair<tag::begin(I), tag::end(I)>
      forward_buffer(I first, I next, difference_type_t<I> n,
                     buf_t<I>& buf, Pred& pred, Proj& proj)
      {
        // Precondition: !pred(proj(*first)))
        // Precondition: __stl2::next(first) == next
        STL2_ASSUME(n >= 2);
        STL2_ASSUME(n <= buf.size());

        auto&& vec = detail::make_temporary_vector(buf);
        vec.push_back(__stl2::iter_move(first));
        auto counted = __stl2::make_counted_iterator(
          ext::uncounted(next), n - 1);
        auto pp = __stl2::partition_move(
            __stl2::move(counted), default_sentinel{},
            __stl2::move(first), __stl2::back_inserter(vec),
            __stl2::ref(pred), __stl2::ref(proj)).out1();
        auto last = __stl2::move(vec, pp).out();
        return {__stl2::move(pp), __stl2::move(last)};
      }

      template <ForwardIterator I, class Proj,
                IndirectCallablePredicate<projected<I, Proj>> Pred>
      requires models::Permutable<I>
      tagged_pair<tag::begin(I), tag::end(I)>
      forward_reduce(I first, difference_type_t<I> n,
                     buf_t<I>& buf, Pred& pred, Proj& proj);

      template <ForwardIterator I, class Proj,
                IndirectCallablePredicate<projected<I, Proj>> Pred>
      requires models::Permutable<I>
      tagged_pair<tag::begin(I), tag::end(I)>
      forward(I first, difference_type_t<I> n,
              buf_t<I>& buf, Pred& pred, Proj& proj)
      {
        // Precondition: !pred(proj(*first)))
        STL2_ASSUME(n > 0);

        auto middle = __stl2::next(first);
        if (n == difference_type_t<I>(1)) {
          return {__stl2::move(first), __stl2::move(middle)};
        }
        // n >= 2

        if (n <= buf.size()) {
          return stable_part::forward_buffer(
            __stl2::move(first), __stl2::move(middle),
            n, buf, pred, proj);
        }

        const auto half_n = n / 2;
        auto res1 = stable_part::forward(__stl2::move(first), half_n, buf, pred, proj);
        auto res2 = stable_part::forward_reduce(res1.end(), n - half_n, buf, pred, proj);
        auto pp = __stl2::rotate(__stl2::move(res1.begin()),
                                 __stl2::move(res1.end()),
                                 __stl2::move(res2.begin())).begin();
        return {__stl2::move(pp), __stl2::move(res2.end())};
      }

      template <ForwardIterator I, class Proj,
                IndirectCallablePredicate<projected<I, Proj>> Pred>
      requires models::Permutable<I>
      tagged_pair<tag::begin(I), tag::end(I)>
      forward_reduce(I first, difference_type_t<I> n,
                     buf_t<I>& buf, Pred& pred, Proj& proj)
      {
        // Establish preconditions of stable_part::forward by reducing the
        // input range.
        stable_part::skip_true(first, n, pred, proj);
        if (n < difference_type_t<I>(2)) {
          auto last = __stl2::next(first, n);
          return {__stl2::move(first), __stl2::move(last)};
        }
        return stable_part::forward(__stl2::move(first), n, buf, pred, proj);
      }

      template <BidirectionalIterator I, class Proj,
                IndirectCallablePredicate<projected<I, Proj>> Pred>
      void skip_false(I& last, difference_type_t<I>& n, Pred& pred, Proj& proj)
      {
        // Move last backward past values that do not satisfy pred.
        // Precondition: pred(proj(*(last - n)))
        STL2_ASSUME(n > 0);
        // Ensures: n == 0 || pred(proj(*last))

        do {
          --last;
        } while (--n != 0 && !pred(proj(*last)));
      }

      template <BidirectionalIterator I, class Proj,
                IndirectCallablePredicate<projected<I, Proj>> Pred>
      requires models::Permutable<I>
      I bidirectional_buffer(I first, I last, difference_type_t<I> n,
                             buf_t<I>& buf, Pred& pred, Proj& proj)
      {
        // Precondition: !pred(proj(*first))
        // Precondition: pred(proj(*last))
        // Precondition: n == distance(first, last)
        STL2_ASSUME(n >= 2);
        STL2_ASSUME(n <= buf.size());

        // Move the false values into the temporary buffer
        // and the true values to the front of the sequence.
        auto&& vec = detail::make_temporary_vector(buf);
        vec.push_back(__stl2::iter_move(first));
        auto middle = __stl2::next(first);
        middle = __stl2::partition_move(__stl2::move(middle), last,
                                        __stl2::move(first),
                                        __stl2::back_inserter(vec),
                                        __stl2::ref(pred),
                                        __stl2::ref(proj)).out1();
        *middle = __stl2::iter_move(last);
        ++middle;
        __stl2::move(vec, middle);
        return middle;
      }

      template <BidirectionalIterator I, class Proj,
                IndirectCallablePredicate<projected<I, Proj>> Pred>
      requires models::Permutable<I>
      I bidirectional_reduce_front(I first, I last, difference_type_t<I> n,
                                   buf_t<I>& buf, Pred& pred, Proj& proj);

      template <BidirectionalIterator I, class Proj,
                IndirectCallablePredicate<projected<I, Proj>> Pred>
      requires models::Permutable<I>
      I bidirectional_reduce_back(I first, I last, difference_type_t<I> n,
                                  buf_t<I>& buf, Pred& pred, Proj& proj);

      template <BidirectionalIterator I, class Proj,
                IndirectCallablePredicate<projected<I, Proj>> Pred>
      requires models::Permutable<I>
      I bidirectional(I first, I last, difference_type_t<I> n,
                      buf_t<I>& buf, Pred& pred, Proj& proj)
      {
        // Precondition: !pred(proj(*first))
        // Precondition: pred(proj(*last))
        // Precondition: n == distance(first, last)
        STL2_ASSUME(n >= difference_type_t<I>(1));

        if (n == difference_type_t<I>(1)) {
          __stl2::iter_swap(first, last);
          return last;
        }
        // n >= 2
        if (n <= buf.size()) {
          return stable_part::bidirectional_buffer(
            __stl2::move(first), __stl2::move(last),
            n, buf, pred, proj);
        }

        const auto half_n = n / 2;
        auto middle = __stl2::next(first, half_n);
        auto pp1 = stable_part::bidirectional_reduce_back(
          __stl2::move(first), middle, half_n, buf, pred, proj);
        auto pp2 = stable_part::bidirectional_reduce_front(
          middle, __stl2::move(last), n - half_n, buf, pred, proj);

        return __stl2::rotate(__stl2::move(pp1), __stl2::move(middle),
                              __stl2::move(pp2)).begin();
      }

      template <BidirectionalIterator I, class Proj,
                IndirectCallablePredicate<projected<I, Proj>> Pred>
      requires models::Permutable<I>
      I bidirectional_reduce_front(I first, I last, difference_type_t<I> n,
                                   buf_t<I>& buf, Pred& pred, Proj& proj)
      {
        // Precondition: pred(proj(*last))
        // Precondition: n == distance(first, last)
        STL2_ASSUME(n >= difference_type_t<I>(0));

        stable_part::skip_true(first, n, pred, proj);
        if (n == difference_type_t<I>(0)) {
          return __stl2::move(++last);
        }
        return stable_part::bidirectional(first, last, n, buf, pred, proj);
      }

      template <BidirectionalIterator I, class Proj,
                IndirectCallablePredicate<projected<I, Proj>> Pred>
      requires models::Permutable<I>
      I bidirectional_reduce_back(I first, I last, difference_type_t<I> n,
                                  buf_t<I>& buf, Pred& pred, Proj& proj)
      {
        // Precondition: !pred(proj(*first))
        // Precondition: n == __stl2::distance(first, last)
        STL2_ASSUME(n >= difference_type_t<I>(1));

        stable_part::skip_false(last, n, pred, proj);
        if (n == difference_type_t<I>(0)) {
          return first;
        }
        return stable_part::bidirectional(first, last, n, buf, pred, proj);
      }
    }
  }

  namespace ext {
    template <ForwardIterator I, class Pred, class Proj = identity>
    requires
      models::Permutable<I> &&
      models::IndirectCallablePredicate<
        __f<Pred>, projected<I, __f<Proj>>>
    I stable_partition_n(I first, difference_type_t<I> n,
                         Pred&& pred_, Proj&& proj_ = Proj{})
    {
      auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
      auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));

      // Either prove all true or find first false
      detail::stable_part::skip_true(first, n, pred, proj);
      if (n < difference_type_t<I>(2)) {
        return first;
      }
      // We now have a reduced range [first, first + n)
      // *first is known to be false

      // might want to make this a function of trivial assignment
      static constexpr difference_type_t<I> alloc_threshold = 4;
      using buf_t = detail::stable_part::buf_t<I>;
      auto buf = n >= alloc_threshold ? buf_t{n} : buf_t{};
      return detail::stable_part::forward(
        first, n, buf, pred, proj).begin();
    }

    template <BidirectionalIterator I, class Pred, class Proj = identity>
    requires
      models::Permutable<I> &&
      models::IndirectCallablePredicate<
        __f<Pred>, projected<I, __f<Proj>>>
    I stable_partition_n(I first, I last, difference_type_t<I> n,
                         Pred&& pred_, Proj&& proj_ = Proj{})
    {
      // Precondition: n == distance(first, last);
      auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
      auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));

      // Either prove all true or find first false
      detail::stable_part::skip_true(first, n, pred, proj);
      if (n == difference_type_t<I>(0)) {
        return first;
      }

      // Either prove (first, last) is all false or find last true
      detail::stable_part::skip_false(last, n, pred, proj);
      if (n == difference_type_t<I>(0)) {
        return first;
      }
      // We now have a reduced range [first, last]
      // *first is known to be false
      // *last is known to be true

      // might want to make this a function of trivial assignment
      static constexpr difference_type_t<I> alloc_threshold = 4;
      using buf_t = detail::stable_part::buf_t<I>;
      buf_t buf = n >= alloc_threshold ? buf_t{n} : buf_t{};
      return detail::stable_part::bidirectional(
        first, last, n, buf, pred, proj);
    }

    template <BidirectionalIterator I, class Pred, class Proj = identity>
    requires
      models::Permutable<I> &&
      models::IndirectCallablePredicate<
        __f<Pred>, projected<I, __f<Proj>>>
    I stable_partition_n(I first, difference_type_t<I> n,
                         Pred&& pred, Proj&& proj = Proj{})
    {
      auto bound = __stl2::next(first, n);
      return ext::stable_partition_n(
        __stl2::move(first), __stl2::move(bound), n,
        __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
    }
  }

  template <ForwardIterator I, Sentinel<I> S,
            class Pred, class Proj = identity>
  requires
    models::Permutable<I> &&
    models::IndirectCallablePredicate<
      __f<Pred>, projected<I, __f<Proj>>>
  I stable_partition(I first, S last, Pred&& pred, Proj&& proj = Proj{})
  {
    auto n = __stl2::distance(first, __stl2::move(last));
    return ext::stable_partition_n(
      __stl2::move(first), n,
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }

  template <BidirectionalIterator I, Sentinel<I> S,
            class Pred, class Proj = identity>
  requires
    models::Permutable<I> &&
    models::IndirectCallablePredicate<
      __f<Pred>, projected<I, __f<Proj>>>
  I stable_partition(I first, S last, Pred&& pred, Proj&& proj = Proj{})
  {
    auto bound = ext::enumerate(first, __stl2::move(last));
    return ext::stable_partition_n(
      __stl2::move(first), __stl2::move(bound.end()), bound.count(),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }

  template <ForwardRange Rng, class Pred, class Proj = identity>
  requires
    models::Permutable<iterator_t<Rng>> &&
    models::IndirectCallablePredicate<
      __f<Pred>, projected<iterator_t<Rng>, __f<Proj>>>
  safe_iterator_t<Rng>
  stable_partition(Rng&& rng, Pred&& pred, Proj&& proj = Proj{})
  {
    return ext::stable_partition_n(
      __stl2::begin(rng), __stl2::distance(rng),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }

  template <BidirectionalRange Rng, class Pred, class Proj = identity>
  requires
    models::Permutable<iterator_t<Rng>> &&
    models::IndirectCallablePredicate<
      __f<Pred>, projected<iterator_t<Rng>, __f<Proj>>>
  safe_iterator_t<Rng>
  stable_partition(Rng&& rng, Pred&& pred, Proj&& proj = Proj{})
  {
    auto bound = ext::enumerate(rng);
    return ext::stable_partition_n(
      __stl2::begin(rng), __stl2::move(bound.end()), bound.count(),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
