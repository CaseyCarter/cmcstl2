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
#ifndef STL2_DETAIL_ALGORITHM_MISMATCH_HPP
#define STL2_DETAIL_ALGORITHM_MISMATCH_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// mismatch [mismatch]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I1, Sentinel<I1> S1,
            WeakInputIterator I2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
  [[deprecated]] tagged_pair<tag::in1(I1), tag::in2(I2)>
  mismatch(I1 first1, S1 last1, I2 first2, Pred&& pred_ = Pred{},
           Proj1&& proj1_ = Proj1{}, Proj2&& proj2_ = Proj2{})
  requires
    models::IndirectCallablePredicate<__f<Pred>,
      projected<I1, __f<Proj1>>,
      projected<I2, __f<Proj2>>>
  {
    auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
    auto proj1 = ext::make_callable_wrapper(__stl2::forward<Proj1>(proj1_));
    auto proj2 = ext::make_callable_wrapper(__stl2::forward<Proj2>(proj2_));

    for (; first1 != last1; ++first1, ++first2) {
      if (!pred(proj1(*first1), proj2(*first2))) {
        break;
      }
    }
    return {__stl2::move(first1), __stl2::move(first2)};
  }

  template <InputIterator I1, Sentinel<I1> S1,
            InputIterator I2, Sentinel<I2> S2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
  requires
    models::IndirectCallablePredicate<
      __f<Pred>, projected<I1, __f<Proj1>>, projected<I2, __f<Proj2>>>
  tagged_pair<tag::in1(I1), tag::in2(I2)>
  mismatch(I1 first1, S1 last1, I2 first2, S2 last2, Pred&& pred_ = Pred{},
           Proj1&& proj1_ = Proj1{}, Proj2&& proj2_ = Proj2{})
  {
    auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
    auto proj1 = ext::make_callable_wrapper(__stl2::forward<Proj1>(proj1_));
    auto proj2 = ext::make_callable_wrapper(__stl2::forward<Proj2>(proj2_));

    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
      if (!pred(proj1(*first1), proj2(*first2))) {
        break;
      }
    }
    return {__stl2::move(first1), __stl2::move(first2)};
  }

  template <InputRange Rng1, class I2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
  [[deprecated]]
  tagged_pair<tag::in1(safe_iterator_t<Rng1>), tag::in2(__f<I2>)>
  mismatch(Rng1&& rng1, I2&& first2, Pred&& pred = Pred{},
           Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{})
  requires
    !is_array<remove_reference_t<I2>>::value &&
    models::WeakInputIterator<__f<I2>> &&
    models::IndirectCallablePredicate<__f<Pred>,
      projected<iterator_t<Rng1>, __f<Proj1>>,
      projected<__f<I2>, __f<Proj2>>>
  {
    return __stl2::mismatch(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::forward<I2>(first2), __stl2::forward<Pred>(pred),
      __stl2::forward<Proj1>(proj1), __stl2::forward<Proj2>(proj2));
  }

  template <InputRange Rng1, InputRange Rng2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
  requires
    models::IndirectCallablePredicate<__f<Pred>,
      projected<iterator_t<Rng1>, __f<Proj1>>,
      projected<iterator_t<Rng2>, __f<Proj2>>>
  tagged_pair<tag::in1(safe_iterator_t<Rng1>), tag::in2(safe_iterator_t<Rng2>)>
  mismatch(Rng1&& rng1, Rng2&& rng2, Pred&& pred = Pred{},
           Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{})
  {
    return __stl2::mismatch(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::forward<Pred>(pred),
      __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  // Extension
  template <class E, InputRange Rng2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
  requires
    models::IndirectCallablePredicate<__f<Pred>,
      projected<const E*, __f<Proj1>>,
      projected<iterator_t<Rng2>, __f<Proj2>>>
  tagged_pair<tag::in1(dangling<const E*>), tag::in2(safe_iterator_t<Rng2>)>
  mismatch(std::initializer_list<E>&& rng1, Rng2&& rng2, Pred&& pred = Pred{},
           Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{})
  {
    return __stl2::mismatch(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::forward<Pred>(pred),
      __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  // Extension
  template <InputRange Rng1, class E, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
  requires
    models::IndirectCallablePredicate<__f<Pred>,
      projected<iterator_t<Rng1>, __f<Proj1>>,
      projected<const E*, __f<Proj2>>>
  tagged_pair<tag::in1(safe_iterator_t<Rng1>), tag::in2(dangling<const E*>)>
  mismatch(Rng1&& rng1, std::initializer_list<E>&& rng2, Pred&& pred = Pred{},
           Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{})
  {
    return __stl2::mismatch(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::forward<Pred>(pred),
      __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  // Extension
  template <class E1, class E2, class Pred = equal_to<>,
            class Proj1 = identity, class Proj2 = identity>
  requires
    models::IndirectCallablePredicate<__f<Pred>,
      projected<const E1*, __f<Proj1>>,
      projected<const E2*, __f<Proj2>>>
  tagged_pair<tag::in1(dangling<const E1*>), tag::in2(dangling<const E2*>)>
  mismatch(std::initializer_list<E1>&& rng1,
           std::initializer_list<E2>&& rng2, Pred&& pred = Pred{},
           Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{})
  {
    return __stl2::mismatch(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::forward<Pred>(pred),
      __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }
} STL2_CLOSE_NAMESPACE

#endif
