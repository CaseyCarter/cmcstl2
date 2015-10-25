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
#ifndef STL2_DETAIL_ALGORITHM_MAX_HPP
#define STL2_DETAIL_ALGORITHM_MAX_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/max_element.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// max [alg.min.max]
//
STL2_OPEN_NAMESPACE {
  template <InputRange Rng, class Proj = identity,
            IndirectCallableStrictWeakOrder<
              projected<iterator_t<Rng>, Proj>> Comp = less<>>
    requires Copyable<value_type_t<iterator_t<Rng>>>()
  constexpr value_type_t<iterator_t<Rng>>
  __max(Rng&& r, Comp&& comp_ = Comp{}, Proj&& proj_ = Proj{}) {
    auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
    auto first = __stl2::begin(r);
    auto last = __stl2::end(r);
    STL2_ASSUME(first != last);
    auto tmp = *first;
    while (++first != last) {
      if (comp(proj(tmp), proj(*first))) {
        tmp = *first;
      }
    }
    return tmp;
  }

  template <class T, class Proj = identity,
            IndirectCallableStrictWeakOrder<
              projected<const T*, Proj>> Comp = less<>>
  constexpr const T& max(const T& a, const T& b,
                         Comp&& comp_ = Comp{},
                         Proj&& proj_ = Proj{}) {
    auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
    return !comp(proj(a), proj(b)) ? a : b;
  }

  template <InputRange Rng, class Proj = identity,
            IndirectCallableStrictWeakOrder<
              projected<iterator_t<Rng>, Proj>> Comp = less<>>
    requires Copyable<value_type_t<iterator_t<Rng>>>()
  STL2_CONSTEXPR_EXT value_type_t<iterator_t<Rng>>
  max(Rng&& r, Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
    return __stl2::__max(__stl2::forward<Rng>(r),
                         __stl2::forward<Comp>(comp),
                         __stl2::forward<Proj>(proj));
  }

  template <Copyable T, class Proj = identity,
            IndirectCallableStrictWeakOrder<
              projected<const T*, Proj>> Comp = less<>>
  constexpr T max(std::initializer_list<T>&& rng,
                  Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
    return __stl2::__max(rng, __stl2::forward<Comp>(comp),
                         __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
