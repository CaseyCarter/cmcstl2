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
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
#ifndef STL2_DETAIL_ALGORITHM_SORT_HEAP_HPP
#define STL2_DETAIL_ALGORITHM_SORT_HEAP_HPP

#include <stl2/detail/algorithm/pop_heap.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// sort_heap [sort.heap]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		struct __sort_heap_n_fn {
			template<random_access_iterator I, class Comp, class Proj>
			requires sortable<I, Comp, Proj>
			constexpr void operator()(I first, iter_difference_t<I> n,
				Comp comp, Proj proj) const
			{
				for (; n > 1; --n) {
					detail::pop_heap_n(first, n, __stl2::ref(comp),
						__stl2::ref(proj));
				}
			}
		};

		inline constexpr __sort_heap_n_fn sort_heap_n;
	} // namespace ext

	struct __sort_heap_fn : private __niebloid {
		template<random_access_iterator I, sentinel_for<I> S, class Comp = less,
			class Proj = identity>
		requires sortable<I, Comp, Proj>
		constexpr I
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
			auto n = distance(first, std::move(last));
			ext::sort_heap_n(first, n, __stl2::ref(comp), __stl2::ref(proj));
			return first + n;
		}

		template<random_access_range R, class Comp = less, class Proj = identity>
		requires sortable<iterator_t<R>, Comp, Proj>
		constexpr safe_iterator_t<R>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			auto n = distance(r);
			ext::sort_heap_n(begin(r), n, __stl2::ref(comp), __stl2::ref(proj));
			return begin(r) + n;
		}
	};

	inline constexpr __sort_heap_fn sort_heap;
} STL2_CLOSE_NAMESPACE

#endif
