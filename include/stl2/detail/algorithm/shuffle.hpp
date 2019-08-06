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

#include <stl2/random.hpp>
#include <stl2/detail/randutils.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// shuffle [alg.random.shuffle]
//
STL2_OPEN_NAMESPACE {
	struct __shuffle_fn : private __niebloid {
		template<random_access_iterator I, sentinel_for<I> S,
			class Gen = detail::default_random_engine&>
		requires permutable<I> &&
			uniform_random_bit_generator<std::remove_reference_t<Gen>>
		constexpr I operator()(I const first, S const last,
			Gen&& g = detail::get_random_engine()) const
		{
			auto mid = first;
			if (mid == last) return mid;
			using D = iter_difference_t<I>;
			auto dist = std::uniform_int_distribution<D>{};
			using param_t =
				typename std::uniform_int_distribution<D>::param_type;
			while (++mid != last) {
				if (auto const i = dist(g, param_t{0, mid - first})) {
					iter_swap(mid - i, mid);
				}
			}
			return mid;
		}

		template<random_access_range Rng,
			class Gen = detail::default_random_engine&>
		requires permutable<iterator_t<Rng>> &&
			uniform_random_bit_generator<std::remove_reference_t<Gen>>
		constexpr safe_iterator_t<Rng>
		operator()(Rng&& rng, Gen&& g = detail::get_random_engine()) const {
			return (*this)(begin(rng), end(rng), std::forward<Gen>(g));
		}
	};

	inline constexpr __shuffle_fn shuffle {};
} STL2_CLOSE_NAMESPACE

#endif
