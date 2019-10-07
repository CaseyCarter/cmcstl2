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
#ifndef STL2_DETAIL_ALGORITHM_NTH_ELEMENT_HPP
#define STL2_DETAIL_ALGORITHM_NTH_ELEMENT_HPP

#include <stl2/detail/algorithm/min_element.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// nth_element [alg.nth.element]
//
STL2_OPEN_NAMESPACE {
	struct __nth_element_fn : private __niebloid {
		// TODO: refactor this monstrosity.
		template<random_access_iterator I, sentinel_for<I> S, class Comp = less,
			class Proj = identity>
		requires sortable<I, Comp, Proj>
		constexpr I operator()(I first, I nth, S last, Comp comp = {},
			Proj proj = {}) const
		{
			constexpr iter_difference_t<I> limit = 7;
			static_assert(limit >= 3);

			auto pred = [&](auto&& lhs, auto&& rhs) -> bool {
				return __stl2::invoke(comp,
					__stl2::invoke(proj, static_cast<decltype(lhs)>(lhs)),
					__stl2::invoke(proj, static_cast<decltype(rhs)>(rhs)));
			};

			I end_orig = next(nth, last);
			I end = end_orig;
			while (true) {
				if (nth == end) return end_orig;

				iter_difference_t<I> len = end - first;
				STL2_EXPECT(len >= 0);
				switch (len) {
				case 0:
				case 1:
					return end_orig;
				case 2:
					if (pred(*--end, *first)) iter_swap(first, end);
					return end_orig;
				case 3:
					{
						I m = first;
						sort3(first, ++m, --end, comp, proj);
						return end_orig;
					}
				default: break;
				}
				if (len <= limit) {
					selection_sort(first, end, comp, proj);
					return end_orig;
				}
				// Post: len > limit

				I m = first + len / 2;
				I lm1 = end;
				unsigned n_swaps = sort3(first, m, --lm1, comp, proj);
				// Post: *m is median

				// partition [first, m) < *m and *m <= [m, end)
				//(this inhibits tossing elements equivalent to m around unnecessarily)
				I i = first;
				I j = lm1;
				// j points beyond range to be tested, *lm1 is known to be <= *m
				// The search going up is known to be guarded but the search coming down isn't.
				// Prime the downward search with a guard.
				if (!bool(pred(*i, *m))) {  // if *first == *m
					// *first == *m, *first doesn't go in first part
					// manually guard downward moving j against i
					bool restart = false;
					while (true) {
						if (i == --j) {
							// *first == *m, *m <= all other elements
							// Parition instead into [first, i) == *first and *first < [i, end)
							++i;  // first + 1
							j = end;
							if (!bool(pred(*first, *--j))) {  // we need a guard if *first == *(end-1)
								while (true) {
									if (i == j) {
										return end_orig;  // [first, end) all equivalent elements
									}
									if (pred(*first, *i)) {
										iter_swap(i, j);
										++n_swaps;
										++i;
										break;
									}
									++i;
								}
							}
							// [first, i) == *first and *first < [j, end) and j == end - 1
							if (i == j) return end_orig;

							while (true) {
								while (!bool(pred(*first, *i))) { ++i; }
								while (pred(*first, *--j)) {}
								if (i >= j) break;
								iter_swap(i, j);
								++n_swaps;
								++i;
							}
							// [first, i) == *first and *first < [i, end)
							// The first part is sorted,
							if (nth < i) return end_orig;

							// nth_element the second part
							// nth_element<C>(i, nth, end, comp);
							restart = true;
							break;
						}
						if (pred(*j, *m)) {
							iter_swap(i, j);
							++n_swaps;
							break;  // found guard for downward moving j, now use unguarded partition
						}
					}
					if (restart) {
						first = i;
						continue;
					}
				}
				++i;
				// j points beyond range to be tested, *lm1 is known to be <= *m
				// if not yet partitioned...
				if (i < j) {
					// known that *(i - 1) < *m
					while (true) {
						// m still guards upward moving i
						while (pred(*i, *m)) { ++i; }
						// It is now known that a guard exists for downward moving j
						while (!bool(pred(*--j, *m))) {}
						if (i >= j) break;
						iter_swap(i, j);
						++n_swaps;
						// It is known that m != j
						// If m just moved, follow it
						if (m == i) m = j;
						++i;
					}
				}
				// [first, i) < *m and *m <= [i, end)
				if (bool(i != m) && bool(pred(*m, *i))) {
					iter_swap(i, m);
					++n_swaps;
				}
				// [first, i) < *i and *i <= [i+1, end)
				if (nth == i) return end_orig;

				if (n_swaps == 0) {
					// We were given a perfectly partitioned sequence.  Coincidence?
					if (nth < i) {
						// Check for [first, i) already sorted
						j = m = first;
						while (true) {
							if (++j == i) {
								// [first, i) sorted
								return end_orig;
							}
							if (pred(*j, *m)) {
								// not yet sorted, so sort
								break;
							}
							m = j;
						}
					} else {
						// Check for [i, end) already sorted
						j = m = i;
						while (true) {
							if (++j == end) {
								// [i, end) sorted
								return end_orig;
							}
							if (pred(*j, *m)) {
								// not yet sorted, so sort
								break;
							}
							m = j;
						}
					}
				}

				// nth_element on range containing nth
				if (nth < i) {
					// nth_element<C>(first, nth, i, comp);
					end = i;
				} else {
					// nth_element<C>(i+1, nth, end, comp);
					first = ++i;
				}
			}
			return end_orig;
		}

		template<random_access_range Rng, class Comp = less, class Proj = identity>
		requires sortable<iterator_t<Rng>, Comp, Proj>
		constexpr safe_iterator_t<Rng> operator()(Rng&& rng, iterator_t<Rng> nth,
			Comp comp = {}, Proj proj = {}) const
		{
			return (*this)(begin(rng), std::move(nth), end(rng),
				__stl2::ref(comp), __stl2::ref(proj));
		}
	private:
		// stable, 2-3 compares, 0-2 swaps
		template<class I, class C, class P>
		requires sortable<I, C, P>
		static constexpr unsigned sort3(I x, I y, I z, C& comp, P& proj) {
			auto pred = [&](auto&& lhs, auto&& rhs) -> bool {
				return __stl2::invoke(comp,
					__stl2::invoke(proj, static_cast<decltype(lhs)>(lhs)),
					__stl2::invoke(proj, static_cast<decltype(rhs)>(rhs)));
			};

			if (!bool(pred(*y, *x))) {      // if x <= y
				if (!bool(pred(*z, *y))) {  // if y <= z
					return 0;         // x <= y && y <= z
				}
				                      // x <= y && y > z
				iter_swap(y, z);      // x <= z && y < z
				if (pred(*y, *x)) {   // if x > y
					iter_swap(x, y);  // x < y && y <= z
					return 2;
				}
				return 1;             // x <= y && y < z
			}
			if (pred(*z, *y)) {       // x > y, if y > z
				iter_swap(x, z);      // x < y && y < z
				return 1;
			}
			iter_swap(x, y);          // x > y && y <= z
			                          // x < y && x <= z
			if (pred(*z, *y)) {       // if y > z
				iter_swap(y, z);      // x <= y && y < z
				return 2;
			}
			return 1;
		}

		template<bidirectional_iterator I, class C, class P>
		requires sortable<I, C, P>
		static constexpr void selection_sort(I begin, I end, C &comp, P &proj) {
			STL2_EXPECT(begin != end);
			for (I lm1 = prev(end); begin != lm1; ++begin) {
				I i = min_element(begin, end, __stl2::ref(comp),
					__stl2::ref(proj));
				if (i != begin) {
					iter_swap(begin, i);
				}
			}
		}
	};

	inline constexpr __nth_element_fn nth_element {};
} STL2_CLOSE_NAMESPACE

#endif
