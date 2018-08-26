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
#include <stl2/detail/algorithm/return_types.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// merge [alg.merge]
//
STL2_OPEN_NAMESPACE {
	template<class I1, class I2, class O>
	struct merge_result : __in1_in2_out_result<I1, I2, O> {};

	struct __merge_fn {
		template<InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
			WeaklyIncrementable O, class Comp = ranges::less<>, class Proj1 = identity,
			class Proj2 = identity>
		requires Mergeable<I1, I2, O, Comp, Proj1, Proj2>
		constexpr merge_result<I1, I2, O>
		operator()(I1 first1, S1 last1, I2 first2, S2 last2, O result,
			Comp comp = Comp{}, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			while (true) {
				if (first1 == last1) {
					auto copy_result = __stl2::copy(
						std::move(first2), std::move(last2), std::move(result));
						first2 = std::move(copy_result.in);
						result = std::move(copy_result.out);
					break;
				}
				if (first2 == last2) {
					auto copy_result = __stl2::copy(
						std::move(first1), std::move(last1), std::move(result));
					first1 = std::move(copy_result.in);
					result = std::move(copy_result.out);
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

		template<InputRange R1, InputRange R2, WeaklyIncrementable O, class Comp = ranges::less<>,
			class Proj1 = identity, class Proj2 = identity>
		requires Mergeable<iterator_t<R1>, iterator_t<R2>, O, Comp, Proj1, Proj2>
		constexpr merge_result<safe_iterator_t<R1>, safe_iterator_t<R2>, O>
		operator()(R1&& r1, R2&& r2, O result,
		Comp comp = Comp{}, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			return (*this)(
				__stl2::begin(r1), __stl2::end(r1),
				__stl2::begin(r2), __stl2::end(r2),
				std::move(result), std::ref(comp),
				std::ref(proj1), std::ref(proj2));
		}
	};

	inline constexpr __merge_fn merge {};
} STL2_CLOSE_NAMESPACE

#endif
