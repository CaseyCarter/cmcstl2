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
#ifndef STL2_DETAIL_ALGORITHM_MAX_ELEMENT_HPP
#define STL2_DETAIL_ALGORITHM_MAX_ELEMENT_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// max_element [alg.min.max]
//
STL2_OPEN_NAMESPACE {
  template <ForwardIterator I, Sentinel<I> S, class Proj = identity,
            IndirectCallableStrictWeakOrder<projected<I, Proj>> Comp = less<>>
  I max_element(I first, S last, Comp&& comp_ = Comp{}, Proj&& proj_ = Proj{}) {
    if (first != last) {
      auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
      auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
      for (auto i = __stl2::next(first); i != last; ++i) {
        if (!comp(proj(*i), proj(*first))) {
          first = i;
        }
      }
    }
    return first;
  }

  template <ForwardRange Rng, class Proj = identity,
            IndirectCallableStrictWeakOrder<projected<iterator_t<Rng>, Proj>> Comp = less<>>
  safe_iterator_t<Rng>
  max_element(Rng&& rng, Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
    return __stl2::max_element(__stl2::begin(rng), __stl2::end(rng),
                               __stl2::forward<Comp>(comp),
                               __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
