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
#ifndef STL2_DETAIL_ALGORITHM_SET_INTERSECTION_HPP
#define STL2_DETAIL_ALGORITHM_SET_INTERSECTION_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// set_intersection [set.intersection]
//
STL2_OPEN_NAMESPACE {
	template<class I1, class I2, class O>
	using set_intersection_result = __in_in_out_result<I1, I2, O>;

	struct __set_intersection_fn : private __niebloid {
		template<InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
			WeaklyIncrementable O, class Comp = less, class Proj1 = identity, class Proj2 = identity>
		requires Mergeable<I1, I2, O, Comp, Proj1, Proj2>
		constexpr set_intersection_result<I1, I2, O>
		operator()(I1 first1, S1 last1, I2 first2, S2 last2, O result,
			Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			while (first1 != last1 && first2 != last2) {
				iter_reference_t<I1>&& v1 = *first1;
				iter_reference_t<I2>&& v2 = *first2;
				auto&& p1 = __stl2::invoke(proj1, v1);
				auto&& p2 = __stl2::invoke(proj2, v2);
				if (__stl2::invoke(comp, p1, p2)) {
					++first1;
				} else if (__stl2::invoke(comp, p2, p1)) {
					++first2;
				} else {
					*result = std::forward<iter_reference_t<I1>>(v1);
					++result;
					++first1;
					++first2;
				}
			}
			return {std::move(first1), std::move(first2), std::move(result)};
		}

		template<InputRange R1, InputRange R2, WeaklyIncrementable O,
			class Comp = less, class Proj1 = identity, class Proj2 = identity>
		requires Mergeable<iterator_t<R1>, iterator_t<R2>, O, Comp, Proj1, Proj2>
		constexpr set_intersection_result<safe_iterator_t<R1>, safe_iterator_t<R2>, O>
		operator()(R1&& r1, R2&& r2, O result, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			return (*this)(begin(r1), end(r1), begin(r2), end(r2), std::move(result),
				__stl2::ref(comp), __stl2::ref(proj1), __stl2::ref(proj2));
		}
	};

	inline constexpr __set_intersection_fn set_intersection {};
} STL2_CLOSE_NAMESPACE

#endif
