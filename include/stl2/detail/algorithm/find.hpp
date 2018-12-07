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
#ifndef STL2_DETAIL_ALGORITHM_FIND_HPP
#define STL2_DETAIL_ALGORITHM_FIND_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// find [alg.find]
//
STL2_OPEN_NAMESPACE {
	struct __find_fn {
		template<InputIterator I, Sentinel<I> S, class T, class Proj = identity>
		requires IndirectRelation<equal_to, projected<I, Proj>, const T*>
		constexpr I operator()(I first, S last, const T& value, Proj proj = {}) const {
			for (; first != last; ++first) {
				if (__stl2::invoke(proj, *first) == value) {
					break;
				}
			}
			return first;
		}

		template<InputRange R, class T, class Proj = identity>
		requires IndirectRelation<equal_to, projected<iterator_t<R>, Proj>, const T*>
		constexpr safe_iterator_t<R>
		operator()(R&& r, const T& value, Proj proj = {}) const {
			return (*this)(__stl2::begin(r), __stl2::end(r), value, std::ref(proj));
		}
	};

	inline constexpr __find_fn find {};
} STL2_CLOSE_NAMESPACE

#endif
