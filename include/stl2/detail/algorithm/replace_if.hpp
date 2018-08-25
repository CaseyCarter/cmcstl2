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
#ifndef STL2_DETAIL_ALGORITHM_REPLACE_IF_HPP
#define STL2_DETAIL_ALGORITHM_REPLACE_IF_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// replace_if [alg.replace]
//
STL2_OPEN_NAMESPACE {
	struct __replace_if_fn {
		template<InputIterator I, Sentinel<I> S, class T, class Proj = identity,
			IndirectUnaryPredicate<projected<I, Proj>> Pred>
		requires Writable<I, const T&>
		constexpr I operator()(I first, S last, Pred pred, const T& new_value, Proj proj = Proj{}) const
		{
			if (first != last) {
				do {
					if (__stl2::invoke(pred, __stl2::invoke(proj, *first))) {
						*first = new_value;
					}
				} while (++first != last);
			}
			return first;
		}

		template<InputRange R, class T, class Proj = identity,
			IndirectUnaryPredicate<projected<iterator_t<R>, Proj>> Pred>
		requires Writable<iterator_t<R>, const T&>
		constexpr safe_iterator_t<R> operator()(R&& r, Pred pred, const T& new_value, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(r), __stl2::end(r), std::ref(pred), new_value, std::ref(proj));
		}
	};

	inline constexpr __replace_if_fn replace_if {};
} STL2_CLOSE_NAMESPACE

#endif
