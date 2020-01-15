// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ITERATOR_OPERATIONS_HPP
#define STL2_DETAIL_ITERATOR_OPERATIONS_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// Iterator operations [range.iter.ops]
STL2_OPEN_NAMESPACE {
	template<input_or_output_iterator> class counted_iterator;

	struct __advance_fn : private __niebloid {
		template<input_or_output_iterator I>
		constexpr void operator()(I& i, iter_difference_t<I> n) const
		// [[expects: n >= 0 || bidirectional_iterator<I>]]
		{
			if constexpr (random_access_iterator<I>) {
				i += n;
			} else {
				if constexpr (bidirectional_iterator<I>) {
					for (; n < 0; ++n) {
						--i;
					}
				}
				STL2_EXPECT(n >= 0);
				for (; n > 0; --n) {
					++i;
				}
			}
		}

		template<input_or_output_iterator I, sentinel_for<I> S>
		constexpr void operator()(I& i, S bound) const
		// [[expects axiom: reachable(i, bound)]]
		{
			if constexpr (assignable_from<I&, S>) {
				i = std::move(bound);
			} else if constexpr (sized_sentinel_for<S, I>) {
				iter_difference_t<I> d = bound - i;
				STL2_EXPECT(d >= 0);
				(*this)(i, d);
			} else while (i != bound) {
				++i;
			}
		}

		template<input_or_output_iterator I, sentinel_for<I> S>
		constexpr iter_difference_t<I>
		operator()(I& i, iter_difference_t<I> n, S bound) const
		// [[expects axiom: 0 == n ||
		//     (n > 0 && reachable(i, bound)) ||
		//     (n < 0 && same_as<I, S> && bidirectional_iterator<I> && reachable(bound, i))]]
		{
			if constexpr (sized_sentinel_for<S, I>) {
				const auto d = bound - i;
				if constexpr (bidirectional_iterator<I> && same_as<I, S>) {
					STL2_EXPECT(n >= 0 ? d >= 0 : d <= 0);
					if (n >= 0 ? n >= d : n <= d) {
						i = std::move(bound);
						return n - d;
					}
				} else {
					STL2_EXPECT(n >= 0 && d >= 0);
					if (n >= d) {
						(*this)(i, std::move(bound));
						return n - d;
					}
				}
				(*this)(i, n);
				return 0;
			} else {
				if constexpr (bidirectional_iterator<I> && same_as<I, S>) {
					if (n < 0) {
						while (n != 0 && i != bound) {
							--i;
							++n;
						}
						return n;
					}
				}
				STL2_EXPECT(n >= 0);
				while (n != 0 && i != bound) {
					++i;
					--n;
				}
				return n;
			}
		}

		template<input_or_output_iterator I>
		constexpr void
		operator()(counted_iterator<I>& i, iter_difference_t<I> n) const;
	};

	inline constexpr __advance_fn advance;

	// next
	struct __next_fn : private __niebloid {
		template<input_or_output_iterator I>
		constexpr I operator()(I i) const {
			return ++i;
		}

		template<input_or_output_iterator I>
		constexpr I operator()(I i, iter_difference_t<I> n) const {
			advance(i, n);
			return i;
		}

		template<input_or_output_iterator I, sentinel_for<I> S>
		constexpr I operator()(I i, S bound) const {
			advance(i, std::move(bound));
			return i;
		}

		template<input_or_output_iterator I, sentinel_for<I> S>
		constexpr I operator()(I i, iter_difference_t<I> n, S bound) const {
			advance(i, n, std::move(bound));
			return i;
		}
	};

	inline constexpr __next_fn next;

	// prev
	struct __prev_fn : private __niebloid {
		template<bidirectional_iterator I>
		constexpr I operator()(I i) const {
			return --i;
		}

		template<bidirectional_iterator I>
		constexpr I operator()(I i, iter_difference_t<I> n) const {
			advance(i, -n);
			return i;
		}

		template<bidirectional_iterator I>
		constexpr I operator()(I i, iter_difference_t<I> n, I bound) const {
			advance(i, -n, std::move(bound));
			return i;
		}
	};

	inline constexpr __prev_fn prev;
} STL2_CLOSE_NAMESPACE

#endif
