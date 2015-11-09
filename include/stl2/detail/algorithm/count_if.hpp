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
#ifndef STL2_DETAIL_ALGORITHM_COUNT_IF_HPP
#define STL2_DETAIL_ALGORITHM_COUNT_IF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// count_if [alg.count]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S,
            class Pred, class Proj = identity>
  requires
    IndirectCallablePredicate<__f<Pred>, projected<I, __f<Proj>>>()
  difference_type_t<I> count_if(I first, S last,
                                Pred&& pred_, Proj&& proj_ = Proj{})
  {
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
    auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
    auto n = difference_type_t<I>{0};
    for (; first != last; ++first) {
      if (pred(proj(*first))) {
        ++n;
      }
    }
    return n;
  }

  template <InputRange Rng, class Pred, class Proj = identity>
  requires
    IndirectCallablePredicate<
      __f<Pred>, projected<iterator_t<Rng>, __f<Proj>>>()
  difference_type_t<iterator_t<Rng>>
  count_if(Rng&& rng, Pred&& pred, Proj&& proj = Proj{})
  {
    return __stl2::count_if(__stl2::begin(rng), __stl2::end(rng),
                            __stl2::forward<Pred>(pred),
                            __stl2::forward<Proj>(proj));
  }

  template <class E, class Pred, class Proj = identity>
  requires
    IndirectCallablePredicate<
      __f<Pred>, projected<const E*, __f<Proj>>>()
  std::ptrdiff_t
  count_if(std::initializer_list<E>&& rng,
           Pred&& pred, Proj&& proj = Proj{})
  {
    return __stl2::count_if(rng, __stl2::forward<Pred>(pred),
                            __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
