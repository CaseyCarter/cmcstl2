// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_SHIFT_LEFT_HPP
#define STL2_DETAIL_ALGORITHM_SHIFT_LEFT_HPP

#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// shift_left [Extension]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		struct __shift_left_fn : private __niebloid {
			template <Permutable I, Sentinel<I> S>
			constexpr I
			operator()(I first, S last, iter_difference_t<I> n) const {
				if (n <= 0) return next(std::move(first), std::move(last));

				auto mid = first;
				if (advance(mid, n, last) > 0) return first;

				return move(std::move(mid), std::move(last), std::move(first)).out;
			}

			template <Range R>
			requires Permutable<iterator_t<R>>
			constexpr safe_iterator_t<R>
			operator()(R&& r, iter_difference_t<iterator_t<R>> n) const {
				return (*this)(begin(r), end(r), n);
			}
		};

		inline constexpr __shift_left_fn shift_left {};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
