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
	struct __sort_fn {
		template<RandomAccessIterator I, Sentinel<I> S, class Comp = ranges::less<>,
			class Proj = identity>
		requires Sortable<I, Comp, Proj>
		constexpr I operator()(I first, S sent, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			if (first == sent) {
				return first;
			}
			auto last = __stl2::next(first, std::move(sent));
			auto n = __stl2::distance(first, last);
			__stl2::detail::rsort::introsort_loop(first, last, detail::rsort::log2(n) * 2, comp, proj);
			__stl2::detail::rsort::final_insertion_sort(first, last, comp, proj);
			return last;
		}

		template<RandomAccessRange R, class Comp = ranges::less<>, class Proj = identity>
		requires Sortable<iterator_t<R>, Comp, Proj>
		constexpr safe_iterator_t<R> operator()(R&& r, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(r), __stl2::end(r), std::ref(comp), std::ref(proj));
		}
	};

	inline constexpr __sort_fn sort {};

	namespace ext {
		struct __sort_fn : __stl2::__sort_fn {
			using __stl2::__sort_fn::operator();

			template <ForwardIterator I, Sentinel<I> S, class Comp = less<>,
				class Proj = identity>
			requires Sortable<I, Comp, Proj>
			constexpr I operator()(I first, S last, Comp comp = Comp{}, Proj proj = Proj{}) const
			{
				auto n = __stl2::distance(first, std::move(last));
				return detail::fsort::sort_n(std::move(first), n,
					std::ref(comp), std::ref(proj));
			}

			template <ForwardRange R, class Comp = less<>, class Proj = identity>
			requires Sortable<iterator_t<R>, Comp, Proj>
			constexpr safe_iterator_t<R> operator()(R&& r, Comp comp = Comp{}, Proj proj = Proj{}) const
			{
				return detail::fsort::sort_n(__stl2::begin(r), __stl2::distance(r),
					std::ref(comp), std::ref(proj));
			}
		};

		inline constexpr __sort_fn sort {};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
