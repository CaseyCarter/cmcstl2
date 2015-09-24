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
  template<class T, class Comp = less<>>
    requires StrictWeakOrder<FunctionType<Comp>, T>()
  constexpr const T& max(const T& a, const T& b, Comp comp_ = Comp{}) {
    auto&& comp = __stl2::as_function(comp_);
    return !comp(a, b) ? a : b;
  }

  // Not to spec: relax Semiregular to CopyConstructible
  template <CopyConstructible T, class Comp = less<>>
    requires StrictWeakOrder<FunctionType<Comp>, T>()
  constexpr T max(std::initializer_list<T> rng, Comp comp_ = Comp{}) {
    STL2_ASSERT(rng.size() != 0);
    auto&& comp = __stl2::as_function(comp_);
    auto result = rng.begin();
    for (auto i = result + 1, e = rng.end(); i != e; ++i) {
      if (!comp(*i, *result)) {
        result = i;
      }
    }
    return *result;
  }

  // Not to spec: relax Semiregular to CopyConstructible
  template <InputRange Rng,
            IndirectCallableStrictWeakOrder<IteratorType<Rng>> Comp = less<>>
    requires CopyConstructible<ValueType<IteratorType<Rng>>>()
  ValueType<IteratorType<Rng>> max(Rng&& rng, Comp&& comp = Comp{}) {
    auto result = __stl2::max_element(rng, __stl2::forward<Comp>(comp));
    STL2_ASSERT(result != __stl2::end(rng));
    return *result;
  }
} STL2_CLOSE_NAMESPACE

#endif
