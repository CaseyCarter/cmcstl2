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
#ifndef STL2_DETAIL_ITERATOR_OPERATIONS_HPP
#define STL2_DETAIL_ITERATOR_OPERATIONS_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/concepts/object.hpp>

#define STL2_AXIOM_EXPECT(...)

///////////////////////////////////////////////////////////////////////////
// Iterator operations [iterator.operations]
STL2_OPEN_NAMESPACE {
	template<Iterator> class counted_iterator;
	namespace __detail_advance {
		template<Iterator I>
		constexpr auto has_nothrow_successor = noexcept(++std::declval<I&>());

		template<Iterator I>
		constexpr auto has_nothrow_predecessor = true;

		template<BidirectionalIterator I>
		constexpr auto has_nothrow_predecessor<I> = noexcept(--std::declval<I&>());

		template<Iterator I>
		constexpr auto has_nothrow_step = true;

		template<RandomAccessIterator I>
		constexpr auto has_nothrow_step<I> =
			noexcept(std::declval<I&>() += std::declval<iter_difference_t<I>>());

		template<Iterator I>
		constexpr auto is_nothrow_advanceable =
			has_nothrow_successor<I> && has_nothrow_predecessor<I> && has_nothrow_step<I>;
	} // namespace __detail_advance
	struct __advance_fn : private __niebloid {
	private:
		template<Iterator I>
		static constexpr void increment_by(I& i, iter_difference_t<I> n)
		noexcept(noexcept(++i))
		{
			// Pre: [i,i+n)
			STL2_EXPECT(0 <= n);
			while (n != 0) {
				--n;
				++i;
			}
		}

		template<Iterator I>
		static constexpr void decrement_by(I& i, iter_difference_t<I> n)
		noexcept(noexcept(--i))
		{
			do {
				++n;
				--i;
			} while (n != 0);
		}

		template<Iterator I, Sentinel<I> S>
		static constexpr iter_difference_t<I> increment_by(I& i, iter_difference_t<I> n, S bound)
		noexcept(noexcept(++i != bound))
		{
			STL2_EXPECT(0 <= n);
			while (n != 0 && i != bound) {
				++i;
				--n;
			}
			return n;
		}

		template<Iterator I, Sentinel<I> S>
		static constexpr iter_difference_t<I> decrement_by(I& i, iter_difference_t<I> n, S bound)
		noexcept(noexcept(--i != bound))
		{
			do {
				--i;
				++n;
			} while (n != 0 && i != bound);
			return n;
		}
	public:
		template<Iterator I>
		constexpr void operator()(I& i, iter_difference_t<I> n) const
		noexcept(__detail_advance::is_nothrow_advanceable<I>)
		{
			if constexpr (RandomAccessIterator<I>) {
				i += n;
			}
			else if constexpr (BidirectionalIterator<I>) {
				// Pre: 0 <= n ? [i,i+n) : [i+n,i)
				return (0 <= n) ? increment_by(i, n) : decrement_by(i, n);
			}
			else {
				increment_by(i, n);
			}
		}

		template<Iterator I, Sentinel<I> S>
		constexpr void operator()(I& i, S bound) const
		// [[expects axiom: reachable(i, bound)]]
		{
			if constexpr (Assignable<I&, S&&>) {
				i = std::move(bound);
			}
			else if constexpr (SizedSentinel<S, I>) {
				iter_difference_t<I> d = bound - i;
				STL2_EXPECT(0 <= d);
				(*this)(i, d);
			}
			else {
				while (i != bound) {
					++i;
				}
			}
		}

		template<Iterator I, Sentinel<I> S>
		constexpr iter_difference_t<I> operator()(I& i, iter_difference_t<I> n, S bound) const
		// [[expects axiom: 0 == n || (0 < n && reachable(i, bound))]]
		{
			if constexpr (BidirectionalIterator<I> && Same<I, S>) {
				if constexpr (SizedSentinel<I, I>) {
					auto d = iter_difference_t<I>{bound - i};
					STL2_EXPECT(0 <= n ? 0 <= d : 0 >= d);
					if (0 <= n ? d <= n : d >= n) {
						i = std::move(bound);
						return n - d;
					}
					(*this)(i, n);
					return 0;
				}
				else {
					return (0 <= n) ? increment_by(i, n, std::move(bound))
										: decrement_by(i, n, std::move(bound));
				}
			}
			else if constexpr (SizedSentinel<S, I>) {
				STL2_EXPECT(0 <= n);
				auto d = iter_difference_t<I>{bound - i};
				STL2_EXPECT(0 <= d);
				if (d <= n) {
					(*this)(i, std::move(bound));
					return n - d;
				}
				(*this)(i, n);
				return 0;
			}
			else {
				STL2_EXPECT(0 <= n);
				return increment_by(i, n, std::move(bound));
			}
		}

		template<Iterator I>
		constexpr void operator()(counted_iterator<I>& i, iter_difference_t<I> n) const;
	};

	inline constexpr __advance_fn advance {};

	// next
	struct __next_fn : private __niebloid {
		template<Iterator I>
		constexpr I operator()(I x) const
		STL2_NOEXCEPT_RETURN(
			++x
		)

		template<Iterator I>
		constexpr I operator()(I x, iter_difference_t<I> n) const
		STL2_NOEXCEPT_RETURN(
			advance(x, n),
			x
		)

		template<Iterator I, Sentinel<I> S>
		constexpr I operator()(I x, S bound) const
		STL2_NOEXCEPT_RETURN(
			advance(x, std::forward<S>(bound)),
			x
		)

		template<Iterator I, Sentinel<I> S>
		constexpr I operator()(I x, iter_difference_t<I> n, S bound) const
		STL2_NOEXCEPT_RETURN(
			advance(x, n, std::forward<S>(bound)),
			x
		)
	};

	inline constexpr __next_fn next {};

	// prev
	struct __prev_fn : private __niebloid {
		template<BidirectionalIterator I>
		constexpr I operator()(I x) const
		STL2_NOEXCEPT_RETURN(
			--x
		)

		template<BidirectionalIterator I>
		constexpr I operator()(I x, iter_difference_t<I> n) const
		STL2_NOEXCEPT_RETURN(
			advance(x, -n),
			x
		)

		template<BidirectionalIterator I>
		constexpr I operator()(I x, iter_difference_t<I> n, I bound) const
		STL2_NOEXCEPT_RETURN(
			advance(x, -n, std::move(bound)),
			x
		)
	};

	inline constexpr __prev_fn prev {};
} STL2_CLOSE_NAMESPACE

#endif
