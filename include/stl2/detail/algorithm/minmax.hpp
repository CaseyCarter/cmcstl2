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
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// minmax [alg.min.max]
//
STL2_OPEN_NAMESPACE {
  // Extension: projection.
  template<class T, class Proj = identity,
            IndirectCallableStrictWeakOrder<
              Projected<const T*, Proj>> Comp = less<>>
  constexpr tagged_pair<tag::min(const T&), tag::max(const T&)>
  minmax(const T& a, const T& b, Comp comp_ = Comp{}, Proj proj_ = Proj{}) {
    auto&& comp = __stl2::as_function(comp_);
    auto&& proj = __stl2::as_function(proj_);
    if (comp(proj(b), proj(a))) {
      return {b, a};
    } else {
      return {a, b};
    }
  }

  // Not to spec: constexpr.
  // Extension: projection.
  template <InputRange Rng, class Proj = identity,
            IndirectCallableStrictWeakOrder<
              Projected<IteratorType<Rng>, Proj>> Comp = less<>>
    requires Copyable<ValueType<IteratorType<Rng>>>()
  constexpr tagged_pair<tag::min(ValueType<IteratorType<Rng>>),
                        tag::max(ValueType<IteratorType<Rng>>)>
  minmax(Rng&& rng, Comp comp_ = Comp{}, Proj proj_ = Proj{}) {
    using value_type = ValueType<IteratorType<Rng>>;
    auto first = __stl2::begin(rng);
    auto last = __stl2::end(rng);
    STL2_ASSERT(first != last);
    auto result = tagged_pair<tag::min(value_type), tag::max(value_type)>{
      *first, *first
    };
    if (++first != last) {
      auto&& comp = __stl2::as_function(comp_);
      auto&& proj = __stl2::as_function(proj_);
      if (comp(proj(*first), proj(result.first))) {
        result.first = *first;
      } else {
        result.second = *first;
      }
      while (++first != last) {
        auto tmp = value_type{*first};
        if (++first == last) {
          if (comp(proj(tmp), proj(result.first))) {
            result.first = __stl2::move(tmp);
          } else if (!comp(proj(tmp), proj(result.second))) {
            result.second = __stl2::move(tmp);
          }
          break;
        }

        if (comp(proj(*first), proj(tmp))) {
          if (comp(proj(*first), proj(result.first))) {
            result.first = *first;
          }
          if (!comp(proj(tmp), proj(result.second))) {
            result.second = __stl2::move(tmp);
          }
        } else {
          if (comp(proj(tmp), proj(result.first))) {
            result.first = __stl2::move(tmp);
          }
          if (!comp(proj(*first), proj(result.second))) {
            result.second = *first;
          }
        }
      }
    }
    return result;
  }

  // Extension: projection
  template <Copyable T, class Proj = identity,
            IndirectCallableStrictWeakOrder<
              Projected<const T*, Proj>> Comp = less<>>
  constexpr tagged_pair<tag::min(T), tag::max(T)>
    minmax(std::initializer_list<T>&& rng, Comp&& comp = Comp{}, Proj&& proj = Proj{}) {
    return __stl2::minmax(rng, __stl2::forward<Comp>(comp),
                          __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
