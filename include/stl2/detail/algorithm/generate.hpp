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
#ifndef STL2_DETAIL_ALGORITHM_GENERATE_HPP
#define STL2_DETAIL_ALGORITHM_GENERATE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/function.hpp>

///////////////////////////////////////////////////////////////////////////
// generate [alg.generate]
//
STL2_OPEN_NAMESPACE {
	struct __generate_fn {
		template<Iterator O, Sentinel<O> S, CopyConstructible F>
		requires Invocable<F&> && Writable<O, invoke_result_t<F&>>
		constexpr O operator()(O first, S last, F gen) const
		{
			for (; first != last; ++first) {
				*first = gen();
			}
			return first;
		}

		template<class Rng, CopyConstructible F>
		requires Invocable<F&> && OutputRange<Rng, invoke_result_t<F&>>
		constexpr safe_iterator_t<Rng> operator()(Rng&& rng, F gen) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), std::ref(gen));
		}
	};

	inline constexpr __generate_fn generate {};
} STL2_CLOSE_NAMESPACE

#endif
