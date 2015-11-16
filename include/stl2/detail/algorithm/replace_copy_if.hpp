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
#ifndef STL2_DETAIL_ALGORITHM_REPLACE_COPY_IF_HPP
#define STL2_DETAIL_ALGORITHM_REPLACE_COPY_IF_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// replace_copy_if [alg.replace]
//
STL2_OPEN_NAMESPACE {
  template <InputIterator I, Sentinel<I> S, class Pred, class T,
            OutputIterator<T> O, class Proj = identity>
  requires
    models::IndirectlyCopyable<I, O> &&
    models::IndirectCallablePredicate<
      __f<Pred>, projected<I, __f<Proj>>>
  tagged_pair<tag::in(I), tag::out(O)>
  replace_copy_if(I first, S last, O result, Pred&& pred_,
                  const T& new_value, Proj&& proj_ = Proj{})
  {
    auto pred = ext::make_callable_wrapper(__stl2::forward<Pred>(pred_));
    auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));

    for (; first != last; ++first, ++result) {
      auto&& v = *first;
      if (pred(proj(v))) {
        *result = new_value;
      } else {
        *result = __stl2::forward<decltype(v)>(v);
      }
    }
    return {__stl2::move(first), __stl2::move(result)};
  }

  template <InputRange Rng, class Pred, class T,
            class O, class Proj = identity>
  requires
    models::OutputIterator<__f<O>, T> &&
    models::IndirectlyCopyable<iterator_t<Rng>, __f<O>> &&
    models::IndirectCallablePredicate<
      __f<Pred>, projected<iterator_t<Rng>, __f<Proj>>>
  tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
  replace_copy_if(Rng&& rng, O&& result, Pred&& pred, const T& new_value,
                  Proj&& proj = Proj{})
  {
    return __stl2::replace_copy_if(
      __stl2::begin(rng), __stl2::end(rng), __stl2::forward<O>(result),
      __stl2::forward<Pred>(pred), new_value, __stl2::forward<Proj>(proj));
  }

  // Extension
  template <class E, class Pred, class T,
            class O, class Proj = identity>
  requires
    models::OutputIterator<__f<O>, T> &&
    models::IndirectlyCopyable<const E*, __f<O>> &&
    models::IndirectCallablePredicate<
      __f<Pred>, projected<const E*, __f<Proj>>>
  tagged_pair<tag::in(dangling<const E*>), tag::out(__f<O>)>
  replace_copy_if(std::initializer_list<E>&& rng, O&& result, Pred&& pred,
                  const T& new_value, Proj&& proj = Proj{})
  {
    return __stl2::replace_copy_if(
      __stl2::begin(rng), __stl2::end(rng), __stl2::forward<O>(result),
      __stl2::forward<Pred>(pred), new_value, __stl2::forward<Proj>(proj));
  }
} STL2_CLOSE_NAMESPACE

#endif
