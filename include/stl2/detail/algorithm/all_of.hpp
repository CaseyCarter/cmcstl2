// Adapted (stolen) from Range v3 library
//
//  Copyright Andrew Sutton 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_ALL_OF_HPP
#define STL2_DETAIL_ALGORITHM_ALL_OF_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/range.hpp>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>

namespace stl2 { inline namespace v1 {
template <InputIterator I, Sentinel<I> S, class F, class P = identity>
  requires IndirectCallablePredicate<F, Projected<I, P>>()
bool all_of(I first, S last, F pred, P proj = P{}) {
  /* Destructible */ auto &&ipred = as_function(pred);
  /* Destructible */ auto &&iproj = as_function(proj);
  while (first != last && ipred(iproj(*first))) {
    ++first;
  }
  return first == last;
}

// 20150801: Not to spec: the MoveConstructible requirements are implicit.
template <class Rng, MoveConstructible F, MoveConstructible P = identity,
  InputRange R = std::add_lvalue_reference_t<Rng>>
  requires IndirectCallablePredicate<F, Projected<IteratorType<R>, P>>()
bool all_of(Rng &&rng, F pred, P proj = P{}) {
  return all_of(begin(rng), end(rng), std::move(pred), std::move(proj));
}
}}

#endif
