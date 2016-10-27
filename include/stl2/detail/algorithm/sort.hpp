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
#ifndef STL2_DETAIL_ALGORITHM_SORT_HPP
#define STL2_DETAIL_ALGORITHM_SORT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/forward_sort.hpp>
#include <stl2/detail/algorithm/random_access_sort.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// sort [sort]
//
STL2_OPEN_NAMESPACE {
	template <ForwardIterator I, Sentinel<I> S, class Comp = less<>,
		class Proj = identity>
	requires
		models::Sortable<I, __f<Comp>, __f<Proj>>
	I sort(I first, S last, Comp&& comp = Comp{}, Proj&& proj = Proj{})
	{
		auto n = __stl2::distance(first, __stl2::move(last));
		return detail::fsort::sort_n(__stl2::move(first), n,
			__stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
	}

	template <RandomAccessIterator I, Sentinel<I> S, class Comp = less<>,
		class Proj = identity>
	requires
		models::Sortable<I, __f<Comp>, __f<Proj>>
	I sort(I first, S sent, Comp&& comp = Comp{}, Proj&& proj = Proj{})
	{
		if (first == sent) {
			return first;
		}
		I last = __stl2::next(first, __stl2::move(sent));
		auto n = difference_type_t<I>(last - first);
		detail::rsort::introsort_loop(first, last, detail::rsort::log2(n) * 2,
			comp, proj);
		detail::rsort::final_insertion_sort(first, last, comp, proj);
		return last;
	}

	template <ForwardRange Rng, class Comp = less<>, class Proj = identity>
	requires
		models::Sortable<iterator_t<Rng>, __f<Comp>, __f<Proj>>
	safe_iterator_t<Rng>
	sort(Rng&& rng, Comp&& comp = Comp{}, Proj&& proj = Proj{})
	{
		return detail::fsort::sort_n(__stl2::begin(rng), __stl2::distance(rng),
			__stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
	}

	template <RandomAccessRange Rng, class Comp = less<>, class Proj = identity>
	requires
		models::Sortable<iterator_t<Rng>, __f<Comp>, __f<Proj>>
	safe_iterator_t<Rng>
	sort(Rng&& rng, Comp&& comp = Comp{}, Proj&& proj = Proj{})
	{
		return __stl2::sort(__stl2::begin(rng), __stl2::end(rng),
			__stl2::forward<Comp>(comp), __stl2::forward<Proj>(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
