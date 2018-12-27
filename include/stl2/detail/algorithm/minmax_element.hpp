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
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/
//
#ifndef STL2_DETAIL_ALGORITHM_MINMAX_ELEMENT_HPP
#define STL2_DETAIL_ALGORITHM_MINMAX_ELEMENT_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// minmax_element [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	struct __minmax_element_fn : private __niebloid {
		template<ForwardIterator I, Sentinel<I> S, class Proj = identity,
			IndirectStrictWeakOrder<projected<I, Proj>> Comp = less>
		constexpr minmax_result<I>
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const
		{
			minmax_result<I> result{first, first};
			if (first == last || ++first == last) return result;

			auto pred = [&](auto&& lhs, auto&& rhs) -> bool {
				return __stl2::invoke(comp,
					__stl2::invoke(proj, static_cast<decltype(lhs)>(lhs)),
					__stl2::invoke(proj, static_cast<decltype(rhs)>(rhs)));
			};
			if (pred(*first, *result.max)) result.min = first;
			else result.max = first;

			while (true) {
				if (++first == last) return result;

				I tmp = first;
				if (++first == last) {
					if (pred(*tmp, *result.min)) result.min = tmp;
					else if (!pred(*tmp, *result.max)) result.max = tmp;
					return result;
				}

				if (pred(*first, *tmp)) {
					if (pred(*first, *result.min)) result.min = first;
					if (!pred(*tmp, *result.max)) result.max = tmp;
				} else {
					if (pred(*tmp, *result.min)) result.min = tmp;
					if (!pred(*first, *result.max)) result.max = first;
				}
			}
		}

		template<ForwardRange R, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<R>, Proj>> Comp = less>
		constexpr minmax_result<safe_iterator_t<R>>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			return (*this)(begin(r), end(r), __stl2::ref(comp), __stl2::ref(proj));
		}
	};

	inline constexpr __minmax_element_fn minmax_element {};
} STL2_CLOSE_NAMESPACE

#endif
