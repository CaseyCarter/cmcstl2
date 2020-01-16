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
#ifndef STL2_DETAIL_ALGORITHM_REMOVE_IF_HPP
#define STL2_DETAIL_ALGORITHM_REMOVE_IF_HPP

#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// remove_if [alg.remove]
//
STL2_OPEN_NAMESPACE {
	struct __remove_if_fn : private __niebloid {
		template<permutable I, sentinel_for<I> S, class Proj = identity,
			indirect_unary_predicate<projected<I, Proj>> Pred>
		constexpr I
		operator()(I first, S last, Pred pred, Proj proj = {}) const {
			first = find_if(std::move(first), last, __stl2::ref(pred),
				__stl2::ref(proj));
			if (first != last) {
				for (auto m = next(first); m != last; ++m) {
					if (!__stl2::invoke(pred, __stl2::invoke(proj, *m))) {
						*first = iter_move(m);
						++first;
					}
				}
			}
			return first;
		}

		template<forward_range Rng, class Proj = identity,
			indirect_unary_predicate<projected<iterator_t<Rng>, Proj>> Pred>
		requires permutable<iterator_t<Rng>>
		constexpr safe_iterator_t<Rng>
		operator()(Rng&& rng, Pred pred, Proj proj = {}) const {
			return (*this)(begin(rng), end(rng), __stl2::ref(pred),
				__stl2::ref(proj));
		}
	};

	inline constexpr __remove_if_fn remove_if{};
} STL2_CLOSE_NAMESPACE

#endif
