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
#ifndef STL2_DETAIL_ALGORITHM_FIND_FIRST_OF_HPP
#define STL2_DETAIL_ALGORITHM_FIND_FIRST_OF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// find_first_of [alg.find.first.of]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I1, Sentinel<I1> S1,
            ForwardIterator I2, Sentinel<I2> S2,
            class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
  requires
    IndirectCallablePredicate<__f<Pred>,
      projected<I1, __f<Proj1>>,
      projected<I2, __f<Proj2>>>()
  I1 find_first_of(I1 first1, S1 last1, I2 first2, S2 last2,
                   Pred&& pred_ = Pred{}, Proj1&& proj1_ = Proj1{},
                   Proj2&& proj2_ = Proj2{})
  {
    auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
    auto proj1 = ext::make_callable_wrapper(__stl2::forward<Proj1>(proj1_));
    auto proj2 = ext::make_callable_wrapper(__stl2::forward<Proj2>(proj2_));
    for (; first1 != last1; ++first1) {
      for (auto pos = first2; pos != last2; ++pos) {
        if (pred(proj1(*first1), proj2(*pos))) {
          return first1;
        }
      }
    }
    return first1;
  }

  template <InputRange Rng1, ForwardRange Rng2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
  requires
    IndirectCallablePredicate<__f<Pred>,
      projected<iterator_t<Rng1>, __f<Proj1>>,
      projected<iterator_t<Rng2>, __f<Proj2>>>()
  safe_iterator_t<Rng1>
  find_first_of(Rng1&& rng1, Rng2&& rng2, Pred&& pred = Pred{},
                Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{})
  {
    return __stl2::find_first_of(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  // Extension
  template <class E, ForwardRange Rng2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
  requires
    IndirectCallablePredicate<__f<Pred>,
      projected<const E*, __f<Proj1>>,
      projected<iterator_t<Rng2>, __f<Proj2>>>()
  dangling<const E*>
  find_first_of(std::initializer_list<E>&& rng1, Rng2&& rng2,
                Pred&& pred = Pred{},
                Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{})
  {
    return __stl2::find_first_of(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  // Extension
  template <InputRange Rng1, class E, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
  requires
    IndirectCallablePredicate<__f<Pred>,
      projected<iterator_t<Rng1>, __f<Proj1>>,
      projected<const E*, __f<Proj2>>>()
  safe_iterator_t<Rng1>
  find_first_of(Rng1&& rng1, std::initializer_list<E>&& rng2,
                Pred&& pred = Pred{},
                Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{})
  {
    return __stl2::find_first_of(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  // Extension
  template <class E1, class E2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
  requires
    IndirectCallablePredicate<__f<Pred>,
      projected<const E1*, __f<Proj1>>,
      projected<const E2*, __f<Proj2>>>()
  dangling<const E1*>
  find_first_of(std::initializer_list<E1>&& rng1,
                std::initializer_list<E2>&& rng2, Pred&& pred = Pred{},
                Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{})
  {
    return __stl2::find_first_of(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }
} STL2_CLOSE_NAMESPACE

#endif
