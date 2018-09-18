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
#ifndef STL2_DETAIL_ALGORITHM_MERGE_HPP
#define STL2_DETAIL_ALGORITHM_MERGE_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/tuple.hpp>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// merge [alg.merge]
//
STL2_OPEN_NAMESPACE {
	template<InputIterator I1, Sentinel<I1> S1,
		InputIterator I2, Sentinel<I2> S2,
		class O, class Comp = less<>,
		class Proj1 = identity, class Proj2 = identity>
	requires
		Mergeable<I1, I2, O, Comp, Proj1, Proj2>
	tagged_tuple<tag::in1(I1), tag::in2(I2), tag::out(O)>
	merge(I1 first1, S1 last1, I2 first2, S2 last2, O result,
				Comp comp = Comp{}, Proj1 proj1 = Proj1{},
				Proj2 proj2 = Proj2{})
	{
		while (true) {
			if (first1 == last1) {
				std::tie(first2, result) = __stl2::copy(
					std::move(first2), std::move(last2), std::move(result));
				break;
			}
			if (first2 == last2) {
				std::tie(first1, result) = __stl2::copy(
					std::move(first1), std::move(last1), std::move(result));
				break;
			}
			iter_reference_t<I1>&& v1 = *first1;
			iter_reference_t<I2>&& v2 = *first2;
			if (__stl2::invoke(comp, __stl2::invoke(proj1, v1), __stl2::invoke(proj2, v2))) {
				*result = std::forward<iter_reference_t<I1>>(v1);
				++first1;
			} else {
				*result = std::forward<iter_reference_t<I2>>(v2);
				++first2;
			}
			++result;
		}
		return {std::move(first1), std::move(first2), std::move(result)};
	}

	template<InputRange Rng1, InputRange Rng2, class O, class Comp = less<>,
		class Proj1 = identity, class Proj2 = identity>
	requires
		Mergeable<
			iterator_t<Rng1>, iterator_t<Rng2>, __f<O>,
			Comp, Proj1, Proj2>
	tagged_tuple<tag::in1(safe_iterator_t<Rng1>), tag::in2(safe_iterator_t<Rng2>),
		tag::out(__f<O>)>
	merge(Rng1&& rng1, Rng2&& rng2, O&& result, Comp comp = Comp{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
	{
		return __stl2::merge(
			__stl2::begin(rng1), __stl2::end(rng1),
			__stl2::begin(rng2), __stl2::end(rng2),
			std::forward<O>(result), std::ref(comp),
			std::ref(proj1), std::ref(proj2));
	}
} STL2_CLOSE_NAMESPACE

#endif
