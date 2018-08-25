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

#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/algorithm/return_types.hpp>

///////////////////////////////////////////////////////////////////////////
// swap_ranges [alg.swap]
//
STL2_OPEN_NAMESPACE {
	template <class I1, class I2>
	struct swap_ranges_result : __in1_in2_result<I1, I2> {};

	struct __swap_ranges_fn {
		template<InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2>
		requires IndirectlySwappable<I1, I2>
		constexpr swap_ranges_result<I1, I2>
		operator()(I1 first1, S1 last1, I2 first2, S2 last2) const
		{
			for (; first1 != last1 && first2 != last2; ++first1, void(++first2)) {
				__stl2::iter_swap(first1, first2);
			}
			return {std::move(first1), std::move(first2)};
		}

		template<InputRange Rng1, InputRange Rng2>
		requires IndirectlySwappable<iterator_t<Rng1>, iterator_t<Rng2>>
		constexpr swap_ranges_result<safe_iterator_t<Rng1>, safe_iterator_t<Rng2>>
		operator()(Rng1&& rng1, Rng2&& rng2) const
		{
			return (*this)(
				__stl2::begin(rng1), __stl2::end(rng1),
				__stl2::begin(rng2), __stl2::end(rng2));
		}
	};

	inline constexpr __swap_ranges_fn swap_ranges {};
} STL2_CLOSE_NAMESPACE

#endif
