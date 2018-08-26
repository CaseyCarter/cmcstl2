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
#ifndef STL2_DETAIL_ALGORITHM_PARTIAL_SORT_COPY_HPP
#define STL2_DETAIL_ALGORITHM_PARTIAL_SORT_COPY_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/tuple.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/algorithm/heap_sift.hpp>
#include <stl2/detail/algorithm/make_heap.hpp>
#include <stl2/detail/algorithm/sort_heap.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// partial_sort_copy [partial.sort.copy]
//
STL2_OPEN_NAMESPACE {
	struct __partial_sort_copy_fn {
		template<InputIterator I1, Sentinel<I1> S1, RandomAccessIterator I2, Sentinel<I2> S2,
			class Comp = ranges::less<>, class Proj1 = identity, class Proj2 = identity>
		requires IndirectlyCopyable<I1, I2> &&
			Sortable<I2, Comp, Proj2> &&
			IndirectStrictWeakOrder<Comp, projected<I1, Proj1>, projected<I2, Proj2>>
		constexpr I2 operator()(I1 first, S1 last, I2 result_first, S2 result_last,
			Comp comp = Comp{}, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			auto r = result_first;
			if(r != result_last) {
				{
					auto result = ext::copy(std::move(first), last, std::move(r), result_last);
					first = std::move(result.in);
					r = std::move(result.out);
				}

				__stl2::make_heap(result_first, r, std::ref(comp), std::ref(proj2));
				const auto len = __stl2::distance(result_first, r);
				for(; first != last; ++first) {
					iter_reference_t<I1>&& x = *first;
					if(__stl2::invoke(comp, __stl2::invoke(proj1, x), __stl2::invoke(proj2, *result_first))) {
						*result_first = std::forward<iter_reference_t<I1>>(x);
						__stl2::detail::sift_down_n(result_first, len, result_first,
							std::ref(comp), std::ref(proj2));
					}
				}
				__stl2::sort_heap(result_first, r, std::ref(comp), std::ref(proj2));
			}
			return r;
		}

		template<InputRange R1, RandomAccessRange R2, class Comp = ranges::less<>,
			class Proj1 = identity, class Proj2 = identity>
		requires IndirectlyCopyable<iterator_t<R1>, iterator_t<R2>> &&
			Sortable<iterator_t<R2>, Comp, Proj2> &&
			IndirectStrictWeakOrder<Comp, projected<iterator_t<R1>, Proj1>,
		projected<iterator_t<R2>, Proj2>>
		constexpr safe_iterator_t<R2> operator()(R1&& r1, R2&& result_range, Comp comp = Comp{},
			Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			return (*this)(__stl2::begin(r1), __stl2::end(r1),
				__stl2::begin(result_range), __stl2::end(result_range),
				std::ref(comp),
				std::ref(proj1), std::ref(proj2));
		}
	};

	inline constexpr __partial_sort_copy_fn partial_sort_copy {};
} STL2_CLOSE_NAMESPACE

#endif
