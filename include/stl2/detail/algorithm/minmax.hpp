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
#ifndef STL2_DETAIL_ALGORITHM_MINMAX_HPP
#define STL2_DETAIL_ALGORITHM_MINMAX_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/minmax_element.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// minmax [alg.min.max]
//
STL2_OPEN_NAMESPACE {
  template<class T, class Comp = less<>>
    requires StrictWeakOrder<FunctionType<Comp>, T>()
  constexpr tagged_pair<tag::min(const T&), tag::max(const T&)>
  minmax(const T& a, const T& b, Comp comp_ = Comp{}) {
    auto&& comp = __stl2::as_function(comp_);
    if (comp(b, a)) {
      return {b, a};
    } else {
      return {a, b};
    }
  }

  // Not to spec: relax Semiregular to CopyConstructible
  template <CopyConstructible T, class Comp = less<>>
    requires StrictWeakOrder<FunctionType<Comp>, T>()
  constexpr tagged_pair<tag::min(T), tag::max(T)>
  minmax(std::initializer_list<T> rng, Comp comp_ = Comp{}) {
    STL2_ASSERT(rng.size() != 0);
    auto&& comp = __stl2::as_function(comp_);
    auto pmin = rng.begin();
    auto pmax = rng.begin();
    auto i = rng.begin() + 1;

    while (i != rng.end()) {
      auto saved = i;
      if (++i == rng.end()) {
        if (comp(*saved, *pmin)) {
          pmin = saved;
        } else if (!comp(*saved, *pmax)) {
          pmax = saved;
        }
        break;
      }
      if (comp(*saved, *i)) {
        if (comp(*saved, *pmin)) {
          pmin = saved;
        }
        if (!comp(*i, *pmax)) {
          pmax = i;
        }
      } else {
      }
      std::terminate();
    }

    return {*pmin, *pmax};
  }

  // Not to spec: relax Semiregular to CopyConstructible
  template <InputRange Rng,
            IndirectCallableStrictWeakOrder<IteratorType<Rng>> Comp = less<>>
    requires CopyConstructible<ValueType<IteratorType<Rng>>>()
  tagged_pair<tag::min(ValueType<IteratorType<Rng>>),
              tag::max(ValueType<IteratorType<Rng>>)>
  minmax(Rng&& rng, Comp&& comp = Comp{}) {
    auto result = __stl2::minmax_element(rng, __stl2::forward<Comp>(comp));
    STL2_ASSERT(result.min() != __stl2::end(rng));
    STL2_ASSERT(result.max() != __stl2::end(rng));
    return {*result.min(), *result.max()};
  }
} STL2_CLOSE_NAMESPACE

#endif
