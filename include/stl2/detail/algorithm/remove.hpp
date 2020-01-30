// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_REMOVE_HPP
#define STL2_DETAIL_ALGORITHM_REMOVE_HPP

#include <stl2/detail/algorithm/find.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// remove [alg.remove]
//
STL2_OPEN_NAMESPACE {
	struct __remove_fn : private __niebloid {
		template<permutable I, sentinel_for<I> S, class T, class Proj = identity>
		requires indirect_relation<equal_to, projected<I, Proj>, const T*>
		constexpr I
		operator()(I first, S last, const T& value, Proj proj = {}) const {
			first = find(std::move(first), last, value, __stl2::ref(proj));
			if (first != last) {
				for (auto m = next(first); m != last; ++m) {
					if (__stl2::invoke(proj, *m) != value) {
						*first = iter_move(m);
						++first;
					}
				}
			}
			return first;
		}

		template<forward_range Rng, class T, class Proj = identity>
		requires permutable<iterator_t<Rng>> &&
			indirect_relation<equal_to, projected<iterator_t<Rng>, Proj>, const T*>
		constexpr safe_iterator_t<Rng>
		operator()(Rng&& rng, const T& value, Proj proj = {}) const {
			return (*this)(begin(rng), end(rng), value, __stl2::ref(proj));
		}
	};

	inline constexpr __remove_fn remove{};
} STL2_CLOSE_NAMESPACE

#endif
