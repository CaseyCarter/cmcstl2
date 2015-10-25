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
#ifndef STL2_DETAIL_ALGORITHM_FIND_HPP
#define STL2_DETAIL_ALGORITHM_FIND_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// find [alg.find]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, class T, class Proj = identity>
    requires IndirectCallableRelation<equal_to<>, projected<I, Proj>, const T*>()
  I find(I first, S last, const T& value, Proj&& proj_ = Proj{}) {
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
    for (; first != last && proj(*first) != value; ++first) {
      ;
    }
    return first;
  }

  template <InputRange Rng, class T, class Proj = identity>
    requires IndirectCallableRelation<equal_to<>, projected<iterator_t<Rng>, Proj>, const T*>()
  safe_iterator_t<Rng> find(Rng&& rng, const T& value, Proj&& proj = Proj{}) {
    return __stl2::find(__stl2::begin(rng), __stl2::end(rng), value,
                        __stl2::forward<Proj>(proj));
  }

  // Extension
  template <class E, class T, class Proj = identity>
    requires IndirectCallableRelation<equal_to<>, projected<const E*, Proj>, const T*>()
  dangling<const E*> find(std::initializer_list<E>&& il, const T& value, Proj&& proj = Proj{}) {
    return __stl2::find(il.begin(), il.end(), value, __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
