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
#ifndef STL2_DETAIL_ALGORITHM_RANDOM_ACCESS_SORT_HPP
#define STL2_DETAIL_ALGORITHM_RANDOM_ACCESS_SORT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/move_backward.hpp>
#include <stl2/detail/algorithm/partial_sort.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/fundamental.hpp>

///////////////////////////////////////////////////////////////////////////
// sort [sort]
//
STL2_OPEN_NAMESPACE {
	namespace detail {
		struct rsort {
			template<BidirectionalIterator I, class Comp, class Proj>
			requires Sortable<I, Comp, Proj>
			static constexpr void
			unguarded_linear_insert(I last, iter_value_t<I> val, Comp& comp, Proj& proj)
			{
				I next = prev(last);
				while (__stl2::invoke(comp, __stl2::invoke(proj, val), __stl2::invoke(proj, *next))) {
					*last = iter_move(next);
					last = next;
					--next;
				}
				*last = std::move(val);
			}
			template<BidirectionalIterator I, class Comp, class Proj>
			requires Sortable<I, Comp, Proj>
			static constexpr void insertion_sort(I first, I last, Comp& comp, Proj& proj)
			{
				if (first != last) {
					for (I i = next(first); i != last; ++i) {
						linear_insert(first, i, comp, proj);
					}
				}
			}
		private:
			template<BidirectionalIterator I, class Comp, class Proj>
			requires Sortable<I, Comp, Proj>
			static constexpr void linear_insert(I first, I last, Comp& comp, Proj& proj)
			{
				iter_value_t<I> val = iter_move(last);
				if (__stl2::invoke(comp, __stl2::invoke(proj, val), __stl2::invoke(proj, *first))) {
					move_backward(first, last, last + 1);
					*first = std::move(val);
				} else {
					unguarded_linear_insert(last, std::move(val), comp, proj);
				}
			}
		};
	}
} STL2_CLOSE_NAMESPACE

#endif
