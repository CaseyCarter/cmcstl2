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
#ifndef STL2_DETAIL_ALGORITHM_MIN_HPP
#define STL2_DETAIL_ALGORITHM_MIN_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// min [alg.min.max]
//
STL2_OPEN_NAMESPACE {
  // Extension: projection.
  template<class T, class Proj = identity,
           IndirectCallableStrictWeakOrder<
             Projected<const T*, Proj>> Comp = less<>>
  constexpr const T& min(const T& a, const T& b,
                         Comp comp_ = Comp{}, Proj proj_ = Proj{}) {
    auto&& comp = __stl2::as_function(comp_);
    auto&& proj = __stl2::as_function(proj_);
    return comp(proj(b), proj(a)) ? b : a;
  }

  // Not to spec: constexpr.
  // Extension: projection.
  template <InputRange Rng, class Proj = identity,
            IndirectCallableStrictWeakOrder<
              Projected<IteratorType<Rng>, Proj>> Comp = less<>>
    requires Copyable<ValueType<IteratorType<Rng>>>()
  constexpr ValueType<IteratorType<Rng>>
  min(Rng&& r, Comp comp_ = Comp{}, Proj proj_ = Proj{}) {
    auto first = __stl2::begin(r);
    auto last = __stl2::end(r);
    STL2_ASSUME(first != last);
    auto tmp = *first;
    auto&& comp = __stl2::as_function(comp_);
    auto&& proj = __stl2::as_function(proj_);
    while (++first != last) {
      if (comp(proj(*first), proj(tmp))) {
        tmp = *first;
      }
    }
    return tmp;
  }

  // Extension: projection.
  template <Copyable T, class Proj = identity,
           IndirectCallableStrictWeakOrder<
             Projected<const T*, Proj>> Comp = less<>>
  constexpr T min(std::initializer_list<T>&& rng,
                  Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
    return __stl2::min(rng, __stl2::forward<Comp>(comp),
                       __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
