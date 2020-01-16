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
#ifndef STL2_DETAIL_ALGORITHM_SWAP_RANGES_HPP
#define STL2_DETAIL_ALGORITHM_SWAP_RANGES_HPP

#include <stl2/detail/swap.hpp>
#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// swap_ranges [alg.swap]
//
STL2_OPEN_NAMESPACE {
	template<class I1, class I2>
	using swap_ranges_result = __in_in_result<I1, I2>;

	struct __swap_ranges3_fn {
		template<forward_iterator I1, sentinel_for<I1> S1, forward_iterator I2>
		requires indirectly_swappable<I1, I2>
		constexpr swap_ranges_result<I1, I2>
		operator()(I1 first1, S1 last1, I2 first2) const {
			for (; first1 != last1; (void) ++first1, (void) ++first2) {
				iter_swap(first1, first2);
			}
			return {std::move(first1), std::move(first2)};
		}
	};

	inline constexpr __swap_ranges3_fn __swap_ranges3{};

	struct __swap_ranges_fn : private __niebloid {
		template<forward_iterator I1, sentinel_for<I1> S1, forward_iterator I2,
			sentinel_for<I2> S2>
		requires indirectly_swappable<I1, I2>
		constexpr swap_ranges_result<I1, I2>
		operator()(I1 first1, S1 last1, I2 first2, S2 last2) const {
			for (; bool(first1 != last1) && bool(first2 != last2);
			     (void) ++first1, (void) ++first2)
			{
				iter_swap(first1, first2);
			}
			return {std::move(first1), std::move(first2)};
		}

		template<forward_range R1, forward_range R2>
		requires indirectly_swappable<iterator_t<R1>, iterator_t<R2>>
		constexpr swap_ranges_result<safe_iterator_t<R1>, safe_iterator_t<R2>>
		operator()(R1&& r1, R2&& r2) const {
			return (*this)(begin(r1), end(r1), begin(r2), end(r2));
		}
	};

	inline constexpr __swap_ranges_fn swap_ranges{};
} STL2_CLOSE_NAMESPACE

#endif
