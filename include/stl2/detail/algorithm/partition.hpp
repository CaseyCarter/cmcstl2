// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright LLVM Contributors
//  Copyright Eric Niebler
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 AND Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_PARTITION_HPP
#define STL2_DETAIL_ALGORITHM_PARTITION_HPP

#include <stl2/detail/algorithm/find_if_not.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// partition [alg.partitions]
//
STL2_OPEN_NAMESPACE {
	struct __partition_fn : private __niebloid {
		template<permutable I, sentinel_for<I> S, class Proj = identity,
			indirect_unary_predicate<projected<I, Proj>> Pred>
		constexpr I operator()(I first, S last_, Pred pred, Proj proj = {}) const {
			if constexpr (bidirectional_iterator<I>) {
				auto last = next(first, std::move(last_));

				for (; first != last; ++first) {
					if (!__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
						while (true) {
							if (--last == first) {
								return first;
							}
							if (__stl2::invoke(pred, __stl2::invoke(proj, *last))) {
								iter_swap(first, last);
								break;
							}
						}
					}
				}
			} else {
				first = find_if_not(std::move(first), last_, __stl2::ref(pred),
					__stl2::ref(proj));
				if (first != last_) {
					for (auto m = first; ++m != last_;) {
						if (__stl2::invoke(pred, __stl2::invoke(proj, *m))) {
							iter_swap(first, m);
							++first;
						}
					}
				}
			}
			return first;
		}

		template<forward_range R, class Proj = identity,
			indirect_unary_predicate<projected<iterator_t<R>, Proj>> Pred>
		requires permutable<iterator_t<R>>
		constexpr safe_iterator_t<R>
		operator()(R&& r, Pred pred, Proj proj = {}) const {
			return (*this)(begin(r), end(r), __stl2::ref(pred),
				__stl2::ref(proj));
		}
	};

	inline constexpr __partition_fn partition{};
} STL2_CLOSE_NAMESPACE

#endif
