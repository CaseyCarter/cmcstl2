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
	template<RandomAccessIterator I, Sentinel<I> S, class Comp = less,
		class Proj = identity>
	requires
		Sortable<I, Comp, Proj>
	I sort(I first, S sent, Comp comp = {}, Proj proj = {})
	{
		if (first == sent) {
			return first;
		}
		auto last = __stl2::next(first, std::move(sent));
		auto n = __stl2::distance(first, last);
		detail::rsort::introsort_loop(first, last, detail::rsort::log2(n) * 2, comp, proj);
		detail::rsort::final_insertion_sort(first, last, comp, proj);
		return last;
	}

	template<RandomAccessRange Rng, class Comp = less, class Proj = identity>
	requires
		Sortable<iterator_t<Rng>, Comp, Proj>
	safe_iterator_t<Rng> sort(Rng&& rng, Comp comp = {}, Proj proj = {})
	{
		return __stl2::sort(begin(rng), end(rng),
			__stl2::ref(comp), __stl2::ref(proj));
	}

	namespace ext {
#if STL2_WORKAROUND_GCC_79591
		template<RandomAccessIterator I, Sentinel<I> S, class Comp = less,
			class Proj = identity>
		requires
			Sortable<I, Comp, Proj>
		I sort(I first, S sent, Comp comp = {}, Proj proj = {})
		{
			if (first == sent) {
				return first;
			}
			auto last = __stl2::next(first, std::move(sent));
			auto n = __stl2::distance(first, last);
			detail::rsort::introsort_loop(first, last, detail::rsort::log2(n) * 2, comp, proj);
			detail::rsort::final_insertion_sort(first, last, comp, proj);
			return last;
		}

		template<RandomAccessRange Rng, class Comp = less, class Proj = identity>
		requires
			Sortable<iterator_t<Rng>, Comp, Proj>
		safe_iterator_t<Rng> sort(Rng&& rng, Comp comp = {}, Proj proj = {})
		{
			return __stl2::sort(begin(rng), end(rng),
				__stl2::ref(comp), __stl2::ref(proj));
		}
#else  // STL2_WORKAROUND_GCC_79591
		using __stl2::sort;
#endif // STL2_WORKAROUND_GCC_79591

		template<ForwardIterator I, Sentinel<I> S, class Comp = less,
			class Proj = identity>
		requires
			Sortable<I, Comp, Proj>
		I sort(I first, S last, Comp comp = {}, Proj proj = {})
		{
			auto n = __stl2::distance(first, std::move(last));
			return detail::fsort::sort_n(std::move(first), n,
				__stl2::ref(comp), __stl2::ref(proj));
		}

		template<ForwardRange Rng, class Comp = less, class Proj = identity>
		requires
			Sortable<iterator_t<Rng>, Comp, Proj>
		safe_iterator_t<Rng> sort(Rng&& rng, Comp comp = {}, Proj proj = {})
		{
			return detail::fsort::sort_n(begin(rng), __stl2::distance(rng),
				__stl2::ref(comp), __stl2::ref(proj));
		}
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
