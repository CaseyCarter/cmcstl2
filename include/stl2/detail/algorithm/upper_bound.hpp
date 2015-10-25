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
#ifndef STL2_DETAIL_ALGORITHM_UPPER_BOUND_HPP
#define STL2_DETAIL_ALGORITHM_UPPER_BOUND_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/partition_point.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// upper_bound [upper.bound]
//
STL2_OPEN_NAMESPACE {
  template <class C, class T>
  struct __upper_bound_fn {
    ext::callable_wrapper<decay_t<C>> comp_;
    const T& value_;

    constexpr __upper_bound_fn(C&& comp, const T& value) :
      comp_(__stl2::forward<C>(comp)), value_(value) {}

    constexpr bool operator()(auto&& i) const {
      return !comp_(value_, i);
    }
  };

  namespace ext {
    template <ForwardIterator I, class T, class Proj = identity,
              IndirectCallableStrictWeakOrder<const T*,
                                              projected<I, Proj>> Comp = less<>>
    I upper_bound_n(I first, difference_type_t<I> n, const T& value,
                    Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
      return __stl2::ext::partition_point_n(__stl2::move(first), n,
        __upper_bound_fn<Comp, T>{__stl2::forward<Comp>(comp), value},
        __stl2::forward<Proj>(proj));
    }
  }

  template <ForwardIterator I, Sentinel<I> S, class T, class Proj = identity,
            IndirectCallableStrictWeakOrder<const T*, projected<I, Proj>> Comp = less<>>
  I upper_bound(I first, S last, const T& value, Comp&& comp = Comp{},
                Proj&& proj = Proj{}) {
    return __stl2::partition_point(__stl2::move(first), __stl2::move(last),
      __upper_bound_fn<Comp, T>{__stl2::forward<Comp>(comp), value},
      __stl2::forward<Proj>(proj));
  }

  template <ForwardIterator I, Sentinel<I> S, class T, class Proj = identity,
            IndirectCallableStrictWeakOrder<const T*, projected<I, Proj>> Comp = less<>>
    requires SizedIteratorRange<I, S>()
  I upper_bound(I first, S last, const T& value, Comp&& comp = Comp{},
                Proj&& proj = Proj{}) {
    auto n = __stl2::distance(first, __stl2::move(last));
    return __stl2::ext::upper_bound_n(__stl2::move(first), n, value,
      __stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
  }

  template <ForwardRange Rng, class T, class Proj = identity,
            IndirectCallableStrictWeakOrder<const T*,
              projected<iterator_t<Rng>, Proj>> Comp = less<>>
  safe_iterator_t<Rng>
  upper_bound(Rng&& rng, const T& value, Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
    return __stl2::upper_bound(__stl2::begin(rng), __stl2::end(rng), value,
      __stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
  }

  template <ForwardRange Rng, class T, class Proj = identity,
            IndirectCallableStrictWeakOrder<const T*,
              projected<iterator_t<Rng>, Proj>> Comp = less<>>
    requires SizedRange<Rng>()
  safe_iterator_t<Rng>
  upper_bound(Rng&& rng, const T& value, Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
    return __stl2::ext::upper_bound_n(__stl2::begin(rng), __stl2::distance(rng), value,
      __stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
