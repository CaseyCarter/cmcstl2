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
   template <InputIterator I, Sentinel<I> S, OutputIterator<value_type_t<I>&> O,
             class Proj = identity,
             class Arg = projected<I, Proj>,
             IndirectRegularCallable<Arg, Arg> BinaryOp = plus<>>
      tagged_pair<tag::in(I), tag::out(O)> // I don't think these need to be wrapped in __f
   partial_sum(I first, S last, O result, BinaryOp binary_op = BinaryOp{}, Proj proj = Proj{})
   requires models::Constructible<value_type_t<I>, decltype(*first)> &&
      models::ConvertibleTo<indirect_result_of_t<BinaryOp&(Arg, Arg)>, value_type_t<I>> &&
      models::Writable<O, value_type_t<I>> //&&
      //models::RegularNumber<value_type_t<I>, value_type_t<O>>
   {
      for (auto acc = value_type_t<I>{}; first != last; ++first, (void)++result) {
         acc = __stl2::invoke(binary_op, __stl2::invoke(proj, acc), __stl2::invoke(proj, *first));
         *result = acc;
      }

      return {__stl2::move(first), __stl2::move(result)};
   }

   template <InputRange Rng, OutputIterator<value_type_t<iterator_t<Rng>>&> O,
             class Proj = identity,
             class Arg = projected<Proj, iterator_t<Rng>>,
             IndirectRegularCallable<Arg, Arg> BinaryOp = plus<>>
      tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
   partial_sum(Rng&& rng, O result, BinaryOp binary_op = BinaryOp{}, Proj proj = Proj{})
   requires models::Constructible<value_type_t<iterator_t<Rng>>, decltype(*__stl2::begin(rng))> &&
      models::ConvertibleTo<indirect_result_of_t<BinaryOp&(Arg, Arg)>, value_type_t<iterator_t<Rng>>> &&
      models::Writable<O, value_type_t<iterator_t<Rng>>> //&&
      //models::RegularNumber<value_type_t<iterator_t<Rng>>, value_type_t<O>>
   {
      return __stl2::partial_sum(__stl2::begin(rng), __stl2::end(rng), __stl2::move(binary_op),
                                 __stl2::move(proj));
   }
} STL2_CLOSE_NAMESPACE
#endif // STL2_DETAIL_NUMERIC_PARTIAL_SUM_HPP
