// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Eric Niebler
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_REVERSE_HPP
#define STL2_DETAIL_ALGORITHM_REVERSE_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// reverse [alg.reverse]
//
STL2_OPEN_NAMESPACE {
	struct __reverse_fn : private __niebloid {
		template<bidirectional_iterator I, sentinel_for<I> S>
		requires permutable<I>
		constexpr I operator()(I first, S last) const {
			auto bound = next(first, static_cast<S&&>(last));
			if constexpr (random_access_iterator<I>) {
				if (first != bound) {
					for (auto m = bound; first < --m; ++first) {
						iter_swap(first, m);
					}
				}
			} else {
				for (auto m = bound; bool(first != m) && bool(first != --m); ++first) {
					iter_swap(first, m);
				}
			}
			return bound;
		}

		template<bidirectional_range R>
		requires permutable<iterator_t<R>>
		constexpr safe_iterator_t<R> operator()(R&& r) const {
			return (*this)(begin(r), end(r));
		}
	};

	inline constexpr __reverse_fn reverse{};
} STL2_CLOSE_NAMESPACE

#endif
