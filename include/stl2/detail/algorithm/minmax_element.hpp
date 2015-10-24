// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/
//
#ifndef STL2_DETAIL_ALGORITHM_MINMAX_ELEMENT_HPP
#define STL2_DETAIL_ALGORITHM_MINMAX_ELEMENT_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// minmax_element [alg.min.max]
//
STL2_OPEN_NAMESPACE {
  template <ForwardIterator I, Sentinel<I> S, class Proj = identity,
            IndirectCallableStrictWeakOrder<Projected<I, Proj>> Comp = less<>>
  STL2_CONSTEXPR_EXT tagged_pair<tag::min(I), tag::max(I)>
  minmax_element(I first, S last, Comp&& comp_ = Comp{}, Proj&& proj_ = Proj{}) {
    tagged_pair<tag::min(I), tag::max(I)> result{first, first};
    if (first == last || ++first == last) {
      return result;
    }
    auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
    if (comp(proj(*first), proj(*result.first))) {
      result.first = first;
    } else {
      result.second = first;
    }
    while (++first != last) {
      I tmp = first;
      if (++first == last) {
        if (comp(proj(*tmp), proj(*result.first))) {
          result.first = tmp;
        } else if (!comp(proj(*tmp), proj(*result.second))) {
          result.second = tmp;
        }
        break;
      }

      if (comp(proj(*first), proj(*tmp))) {
        if (comp(proj(*first), proj(*result.first))) {
          result.first = first;
        }
        if (!comp(proj(*tmp), proj(*result.second))) {
          result.second = tmp;
        }
      } else {
        if (comp(proj(*tmp), proj(*result.first))) {
          result.first = tmp;
        }
        if (!comp(proj(*first), proj(*result.second))) {
          result.second = first;
        }
      }
    }
    return result;
  }

  template <ForwardRange Rng, class Proj = identity,
            IndirectCallableStrictWeakOrder<Projected<IteratorType<Rng>, Proj>> Comp = less<>>
  STL2_CONSTEXPR_EXT tagged_pair<tag::min(safe_iterator_t<Rng>),
                                 tag::max(safe_iterator_t<Rng>)>
  minmax_element(Rng&& rng, Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
    return __stl2::minmax_element(__stl2::begin(rng), __stl2::end(rng),
                                  __stl2::forward<Comp>(comp),
                                  __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
