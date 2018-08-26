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
#ifndef STL2_DETAIL_ALGORITHM_PUSH_HEAP_HPP
#define STL2_DETAIL_ALGORITHM_PUSH_HEAP_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/heap_sift.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// push_heap [push.heap]
//
STL2_OPEN_NAMESPACE {
	struct __push_heap_fn {
		template<RandomAccessIterator I, Sentinel<I> S, class Comp = ranges::less<>,
			class Proj = identity>
		requires Sortable<I, Comp, Proj>
		constexpr I operator()(I first, S last, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			auto n = __stl2::distance(first, std::forward<S>(last));
			detail::sift_up_n(first, n, std::ref(comp), std::ref(proj));
			return first + n;
		}

		template<RandomAccessRange R, class Comp = ranges::less<>, class Proj = identity>
		requires Sortable<iterator_t<R>, Comp, Proj>
		constexpr safe_iterator_t<R> operator()(R&& r, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			auto n = __stl2::distance(r);
			detail::sift_up_n(__stl2::begin(r), n, std::ref(comp), std::ref(proj));
			return __stl2::begin(r) + n;
		}
	};

	inline constexpr __push_heap_fn push_heap {};
} STL2_CLOSE_NAMESPACE

#endif
