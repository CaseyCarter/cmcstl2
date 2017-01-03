// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2013-2014 (range-v3/include/numeric/partial_sum.hpp)
//  Copyright Casey Carter 2016
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_NUMERIC_PARTIAL_SUM_HPP
#define STL2_DETAIL_NUMERIC_PARTIAL_SUM_HPP

#include <stl2/functional.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/number.hpp>
#include <stl2/detail/concepts/object/semiregular.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
   template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O,
             class Proj = identity,
             class __Arg = projected<I, Proj>,
             IndirectRegularCallable<__Arg, __Arg> Op = plus<>>
   requires
      models::IndirectlyCopyable<__Arg, O> &&
      models::Writable<O, value_type_t<__Arg>> &&
      models::MoveConstructible<value_type_t<__Arg>> && // formally necessary, but are these two
      models::CopyConstructible<value_type_t<__Arg>> && // actually necessary?
      models::Constructible<value_type_t<__Arg>, indirect_result_of_t<Op&(__Arg, __Arg)>> &&
      models::RegularNumber<value_type_t<O>, value_type_t<__Arg>>
   tagged_pair<tag::in(I), tag::out(O)>
   partial_sum(I first, S last, O result, Op op = Op{}, Proj proj = Proj{})
   {
      for (auto acc = value_type_t<__Arg>{}; first != last; ++first, (void)++result) {
         acc = __stl2::invoke(op, acc, __stl2::invoke(proj, *first));
         *result = acc;
      }

      return {__stl2::move(first), __stl2::move(result)};
   }

   template <InputRange Rng, WeaklyIncrementable O,
             class Proj = identity,
             class __Arg = projected<iterator_t<Rng>, Proj>,
             IndirectRegularCallable<__Arg, __Arg> Op = plus<>>
   requires
      models::IndirectlyCopyable<__Arg, O> &&
      models::Writable<O, value_type_t<__Arg>> &&
      models::MoveConstructible<value_type_t<__Arg>> && // formally necessary, but are these two
      models::CopyConstructible<value_type_t<__Arg>> && // actually necessary?
      models::Constructible<value_type_t<__Arg>, indirect_result_of_t<Op&(__Arg, __Arg)>> &&
      models::RegularNumber<value_type_t<O>, value_type_t<__Arg>>
   tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
   partial_sum(Rng&& rng, O result, Op op = Op{}, Proj proj = Proj{})
   {
      return __stl2::partial_sum(__stl2::begin(rng), __stl2::end(rng), __stl2::move(result),
                                 __stl2::move(op), __stl2::move(proj));
   }
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_NUMERIC_PARTIAL_SUM_HPP
