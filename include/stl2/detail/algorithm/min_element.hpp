// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_MIN_ELEMENT_HPP
#define STL2_DETAIL_ALGORITHM_MIN_ELEMENT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// min_element [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	struct __min_element_fn : private __niebloid {
		template<ForwardIterator I, Sentinel<I> S, class Proj = identity,
			IndirectStrictWeakOrder<projected<I, Proj>> Comp = less>
		constexpr I operator()(I first, S last, Comp comp = {}, Proj proj = {}) const
		{
			if (first != last) {
				for (auto i = __stl2::next(first); i != last; ++i) {
					if (__stl2::invoke(comp, __stl2::invoke(proj, *i), __stl2::invoke(proj, *first))) {
						first = i;
					}
				}
			}
			return first;
		}

		template<ForwardRange R, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<R>, Proj>> Comp = less>
		constexpr safe_iterator_t<R>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const
		{
			return (*this)(__stl2::begin(r), __stl2::end(r),
				std::ref(comp), std::ref(proj));
		}
	};

	inline constexpr __min_element_fn min_element {};
} STL2_CLOSE_NAMESPACE

#endif
