// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright LLVM Contributors
//  Copyright Eric Niebler
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 AND Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_POP_HEAP_HPP
#define STL2_DETAIL_ALGORITHM_POP_HEAP_HPP

#include <stl2/detail/algorithm/heap_sift.hpp>
#include <stl2/detail/range/dangling.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// pop_heap [pop.heap]
//
STL2_OPEN_NAMESPACE {
	namespace detail {
		struct __pop_heap_n_fn {
			template<random_access_iterator I, class Proj, class Comp>
			requires sortable<I, Comp, Proj>
			constexpr void
			operator()(I first, iter_difference_t<I> n, Comp comp, Proj proj) const {
				if (n > 1) {
					iter_swap(first, first + (n - 1));
					sift_down_n(first, n - 1, first, __stl2::ref(comp),
						__stl2::ref(proj));
				}
			}
		};

		inline constexpr __pop_heap_n_fn pop_heap_n{};
	}

	struct __pop_heap_fn : private __niebloid {
		template<random_access_iterator I, sentinel_for<I> S, class Comp = less,
			class Proj = identity>
		requires sortable<I, Comp, Proj>
		constexpr I
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
			auto n = distance(first, std::move(last));
			detail::pop_heap_n(first, n, __stl2::ref(comp), __stl2::ref(proj));
			return first + n;
		}

		template<random_access_range R, class Comp = less, class Proj = identity>
		requires sortable<iterator_t<R>, Comp, Proj>
		constexpr safe_iterator_t<R>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			auto n = distance(r);
			detail::pop_heap_n(begin(r), n, __stl2::ref(comp), __stl2::ref(proj));
			return begin(r) + n;
		}
	};

	inline constexpr __pop_heap_fn pop_heap{};
} STL2_CLOSE_NAMESPACE

#endif
