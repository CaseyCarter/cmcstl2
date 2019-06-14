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
#ifndef STL2_DETAIL_ALGORITHM_UNIQUE_HPP
#define STL2_DETAIL_ALGORITHM_UNIQUE_HPP

#include <stl2/detail/algorithm/adjacent_find.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// unique [alg.unique]
//
STL2_OPEN_NAMESPACE {
	struct __unique_fn : private __niebloid {
		template<Permutable I, Sentinel<I> S, class Proj = identity,
			IndirectBinaryPredicate<projected<I, Proj>, projected<I, Proj>> Comp = equal_to>
		constexpr I
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
			first = adjacent_find(std::move(first), last, __stl2::ref(comp),
				__stl2::ref(proj));
			if (first != last) {
				for (auto m = next(first, 2); m != last; ++m) {
					if (!__stl2::invoke(comp, __stl2::invoke(proj, *first),
							__stl2::invoke(proj, *m)))
					{
						*++first = iter_move(m);
					}
				}
				++first;
			}
			return first;
		}

		template<ForwardRange R, class Proj = identity,
			IndirectBinaryPredicate<projected<iterator_t<R>, Proj>, projected<iterator_t<R>, Proj>> Comp = equal_to>
		requires Permutable<iterator_t<R>>
		constexpr safe_iterator_t<R>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			return (*this)(begin(r), end(r), __stl2::ref(comp),
				__stl2::ref(proj));
		}
	};

	inline constexpr __unique_fn unique {};
} STL2_CLOSE_NAMESPACE

#endif
