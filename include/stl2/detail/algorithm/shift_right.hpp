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
#ifndef STL2_DETAIL_ALGORITHM_SHIFT_RIGHT_HPP
#define STL2_DETAIL_ALGORITHM_SHIFT_RIGHT_HPP

#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/algorithm/move_backward.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// shift_right [Extension, from P0769]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		struct __shift_right_fn : private __niebloid {
			template <Permutable I, Sentinel<I> S>
			constexpr I
			operator()(I first, S last, const iter_difference_t<I> n) const {
				if (n <= 0) return first;

				if constexpr (BidirectionalIterator<I> && Same<I, S>) {
					auto mid = last;
					if (advance(mid, -n, first) != 0) return first;
					return move_backward(first, mid, end).out;
				} else {
					const auto left = first;
					if (advance(first, n, last) != 0) return first;

					const auto right = first;
					// [left, right) is the prefix of n elements that are left
					// valid-but-unspecified; we'll use it as scratch space

					auto probe = first;
					if (const auto d = advance(probe, n, last)) {
						return move(left, next(left, n - d), first).out;
					}

					auto mid = left;
					do {
						iter_swap(first, mid);
						if (++mid == right) {
							mid = left;
						}
						++first;
					} while (++probe != last);

					while (true) {
						*first = iter_move(mid);
						if (++first == last) {
							break;
						}
						if (++mid == right) {
							mid = left;
						}
					}
					return first;
				}
			}

			template <Range R>
			requires Permutable<iterator_t<R>>
			constexpr safe_iterator_t<R>
			operator()(R&& r, const iter_difference_t<iterator_t<R>> n) const {
				return (*this)(begin(r), end(r), n);
			}
		};

		inline constexpr __shift_right_fn shift_right {};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
