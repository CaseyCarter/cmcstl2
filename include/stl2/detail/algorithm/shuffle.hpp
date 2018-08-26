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
#ifndef STL2_DETAIL_ALGORITHM_SHUFFLE_HPP
#define STL2_DETAIL_ALGORITHM_SHUFFLE_HPP

#include <stl2/iterator.hpp>
#include <stl2/random.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/randutils.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/core.hpp>

///////////////////////////////////////////////////////////////////////////
// shuffle [alg.random.shuffle]
//
STL2_OPEN_NAMESPACE {
	struct __shuffle_fn {
		template<RandomAccessIterator I, Sentinel<I> S, class Gen>
		requires Permutable<I> &&
			UniformRandomBitGenerator<remove_reference_t<Gen>> &&
			ConvertibleTo<invoke_result_t<Gen&>, iter_difference_t<I>>
		I operator()(I first, S last, Gen&& g) const
		{
			auto mid = first;
			if (mid == last) {
				return mid;
			}
			auto dist = uniform_int_distribution<iter_difference_t<I>>{};
			using param_t = typename uniform_int_distribution<iter_difference_t<I>>::param_type;
			while (++mid != last) {
				if (auto const i = dist(g, param_t{0, mid - first})) {
					__stl2::iter_swap(mid - i, mid);
				}
			}
			return mid;
		}

		template<RandomAccessRange Rng, class Gen>
		requires Permutable<iterator_t<Rng>> &&
			UniformRandomBitGenerator<remove_reference_t<Gen>> &&
			ConvertibleTo<invoke_result_t<Gen&>, iter_difference_t<iterator_t<Rng>>>
		safe_iterator_t<Rng> operator()(Rng&& rng, Gen&& g) const
		{
			return  (*this)(__stl2::begin(rng), __stl2::end(rng),
				std::forward<Gen>(g));
		}
	};

	inline constexpr __shuffle_fn shuffle {};
} STL2_CLOSE_NAMESPACE

#endif
