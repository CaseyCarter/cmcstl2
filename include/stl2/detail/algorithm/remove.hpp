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
#ifndef STL2_DETAIL_ALGORITHM_REMOVE_HPP
#define STL2_DETAIL_ALGORITHM_REMOVE_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/find.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// remove [alg.remove]
//
STL2_OPEN_NAMESPACE {
	struct __remove_fn {
		template<ForwardIterator I, Sentinel<I> S, class T, class Proj = identity>
		requires Permutable<I> &&
			IndirectRelation<ranges::equal_to<>, projected<I, Proj>, const T*>
		constexpr I operator()(I first, S last, const T& value, Proj proj = Proj{}) const
		{
			first = __stl2::find(std::move(first), last, value, std::ref(proj));
			if (first != last) {
				for (auto m = __stl2::next(first); m != last; ++m) {
					if (__stl2::invoke(proj, *m) != value) {
						*first = __stl2::iter_move(m);
						++first;
					}
				}
			}
			return first;
		}

		template<ForwardRange Rng, class T, class Proj = identity>
		requires Permutable<iterator_t<Rng>> &&
			IndirectRelation<ranges::equal_to<>, projected<iterator_t<Rng>, Proj>, const T*>
		constexpr safe_iterator_t<Rng> operator()(Rng&& rng, const T& value, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), value, std::ref(proj));
		}
	};

	inline constexpr __remove_fn remove {};
} STL2_CLOSE_NAMESPACE

#endif
