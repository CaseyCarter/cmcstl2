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
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
#ifndef STL2_DETAIL_ALGORITHM_ROTATE_HPP
#define STL2_DETAIL_ALGORITHM_ROTATE_HPP

#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/algorithm/move_backward.hpp>
#include <stl2/detail/algorithm/swap_ranges.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/view/subrange.hpp>

///////////////////////////////////////////////////////////////////////////
// rotate [alg.rotate]
//
STL2_OPEN_NAMESPACE {
	struct __sean_parent_fn : private __niebloid {
		template<permutable I, sentinel_for<I> S>
		constexpr subrange<I> operator()(I first, I middle, S last) const {
			if (first == middle) {
				first = next(std::move(first), std::move(last));
				return {first, std::move(first)};
			}
			if (middle == last) {
				return {std::move(first), std::move(middle)};
			}
			if constexpr (std::is_trivially_move_assignable_v<iter_value_t<I>>) {
				if (next(first) == middle) {
					return __rotate_left(std::move(first), std::move(last));
				}
				if constexpr (same_as<I, S>) {
					if constexpr (bidirectional_iterator<I>) {
						if (next(middle) == last) {
							return __rotate_right(std::move(first), std
								::move(last));
						}
					}
					if constexpr (random_access_iterator<I>) {
						return __rotate_gcd(std::move(first), std::move(middle),
							std::move(last));
					}
				}
			}
			return __rotate_forward(std::move(first), std::move(middle),
				std::move(last));
		}

		template<forward_range R>
		requires permutable<iterator_t<R>>
		constexpr safe_subrange_t<R>
		operator()(R&& r, iterator_t<R> middle) const {
			return (*this)(begin(r), std::move(middle), end(r));
		}
	private:
		template<permutable I, sentinel_for<I> S>
		static constexpr subrange<I> __rotate_left(I first, S sent) {
			STL2_EXPECT(first != sent);
			iter_value_t<I> tmp = iter_move(first);
			auto [last, last_but_one] = move(next(first), sent, first);
			*last_but_one = std::move(tmp);
			return {std::move(last_but_one), std::move(last)};
		}

		template<permutable I>
		requires bidirectional_iterator<I>
		static constexpr subrange<I> __rotate_right(I first, I last) {
			STL2_EXPECT(first != last);
			I last_but_one = prev(last);
			iter_value_t<I> tmp = iter_move(last_but_one);
			I fp1 = move_backward(first, std::move(last_but_one), last).out;
			*first = std::move(tmp);
			return {std::move(fp1), std::move(last)};
		}

		template<permutable I, sentinel_for<I> S>
		static constexpr subrange<I> __rotate_forward(I first, I middle, S last) {
			STL2_EXPECT(first != middle);
			STL2_EXPECT(middle != last);
			I i = middle;
			while (true) {
				iter_swap(first, i);
				++first;
				if (++i == last) {
					break;
				}
				if (first == middle) {
					middle = i;
				}
			}
			I r = first;
			if (first != middle) {
				I j = middle;
				while (true) {
					iter_swap(first, j);
					++first;
					if (++j == last) {
						if (first == middle) {
							break;
						}
						j = middle;
					} else if(first == middle) {
						middle = j;
					}
				}
			}
			return {std::move(r), std::move(i)};
		}

		template<integral I>
		static constexpr I __gcd(I x, I y) {
			do {
				auto t = x % y;
				x = y;
				y = t;
			} while(y);
			return x;
		}

		template<permutable I>
		requires random_access_iterator<I>
		static constexpr subrange<I> __rotate_gcd(I first, I middle, I last) {
			using D = iter_difference_t<I>;
			D const m1 = middle - first;
			D const m2 = last - middle;
			if (m1 == m2) {
				__swap_ranges3(first, middle, middle);
				return {std::move(middle), std::move(last)};
			}
			auto const g = __gcd(m1, m2);
			for (I p = first + g; p != first;) {
				iter_value_t<I> t = iter_move(--p);
				I p1 = p;
				I p2 = p1 + m1;
				do {
					*p1 = iter_move(p2);
					p1 = p2;
					D const d = last - p2;
					if (m1 < d) {
						p2 += m1;
					} else {
						p2 = first + D{m1 - d};
					}
				} while(p2 != p);
				*p1 = std::move(t);
			}
			first += m2;
			return {std::move(first), std::move(last)};
		}
	};

	inline constexpr __sean_parent_fn rotate;
} STL2_CLOSE_NAMESPACE

#endif
