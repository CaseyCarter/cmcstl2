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

///////////////////////////////////////////////////////////////////////////
// Iterator operations [iterator.operations]
STL2_OPEN_NAMESPACE {
	// advance
	namespace __advance {
		template<class I>
		requires
			Iterator<I>
			// Pre: 0 <= n && [i,i+n)
		constexpr void impl(I& i, iter_difference_t<I> n)
		noexcept(noexcept(++std::declval<I&>()))
		{
			STL2_EXPECT(0 <= n);
			while (n != 0) {
				--n;
				++i;
			}
		}
	}

	template<Iterator I>
		// Pre: 0 <= n && [i,i+n)
	constexpr void advance(I& i, iter_difference_t<I> n)
	noexcept(noexcept(++std::declval<I&>()))
	{
		__advance::impl(i, n);
	}

	template<BidirectionalIterator I>
		// Pre: 0 <= n ? [i,i+n) : [i+n,i)
	constexpr void advance(I& i, iter_difference_t<I> n)
	noexcept(noexcept(++std::declval<I&>(), --std::declval<I&>()))
	{
		if (0 <= n) {
			__advance::impl(i, n);
		} else {
			do {
				++n;
				--i;
			} while (n != 0);
		}
	}

	template<RandomAccessIterator I>
		// Pre: 0 <= n ? [i,i+n) : [i+n,i)
	constexpr void advance(I& i, iter_difference_t<I> n)
	STL2_NOEXCEPT_RETURN(
		(void)(i += n)
	)

	template<class I, class S>
	requires
		Sentinel<S, I>
		// Pre: [i,bound)
	constexpr void advance(I& i, S bound)
	noexcept(noexcept(++i != bound))
	{
		while (i != bound) {
			++i;
		}
	}

	template<class I, class S>
	requires
		Sentinel<S, I> && Assignable<I&, S&&>
	constexpr void advance(I& i, S bound)
	STL2_NOEXCEPT_RETURN(
		(void)(i = std::move(bound))
	)

	template<class I, class S>
	requires
		Sentinel<S, I> && !Assignable<I&, S&&> &&
		SizedSentinel<S, I>
		// Pre: [i,bound)
	constexpr void advance(I& i, S bound)
	noexcept(noexcept(__stl2::advance(i, bound - i)))
	{
		iter_difference_t<I> d = bound - i;
		STL2_EXPECT(0 <= d);
		__stl2::advance(i, d);
	}

	namespace __advance {
		template<class I, class S>
		requires
			Sentinel<S, I>
			// Pre: 0 == n || (0 < n && [i,bound))
		constexpr iter_difference_t<I>
		impl(I& i, iter_difference_t<I> n, const S& bound)
		noexcept(noexcept(++i != bound))
		{
			STL2_EXPECT(0 <= n);
			while (n != 0 && i != bound) {
				++i;
				--n;
			}
			return n;
		}
	}

	template<class I, class S>
	requires
		Sentinel<S, I>
		// Pre: 0 == n || (0 < n && [i,bound))
	constexpr iter_difference_t<I>
	advance(I& i, iter_difference_t<I> n, S bound)
	STL2_NOEXCEPT_RETURN(
		__advance::impl(i, n, bound)
	)

	template<class I, class S>
	requires
		Sentinel<S, I> && SizedSentinel<S, I>
		// Pre: 0 <= n && [i,bound)
	constexpr iter_difference_t<I>
	advance(I& i, iter_difference_t<I> n, S bound)
	noexcept(noexcept(
		__stl2::advance(i, std::move(bound)),
		__stl2::advance(i, bound - i)))
	{
		STL2_EXPECT(0 <= n);
		auto d = iter_difference_t<I>{bound - i};
		STL2_EXPECT(0 <= d);
		if (d <= n) {
			__stl2::advance(i, std::move(bound));
			return n - d;
		}
		__stl2::advance(i, n);
		return 0;
	}

	template<class I>
	requires
		BidirectionalIterator<I>
		// Pre: 0 == n || (0 < n ? [i,bound) : [bound,i))
	constexpr iter_difference_t<I>
	advance(I& i, iter_difference_t<I> n, I bound)
	noexcept(noexcept(
		__advance::impl(i, n, bound),
		--i != bound))
	{
		if (0 <= n) {
			return __advance::impl(i, n, bound);
		}

		do {
			--i;
			++n;
		} while (n != 0 && i != bound);
		return n;
	}

	template<class I>
	requires
		BidirectionalIterator<I> && SizedSentinel<I, I>
		// Pre: 0 == n ? ([i,bound) || [bound,i)) : (0 < n ? [i,bound) : [bound,i))
	constexpr iter_difference_t<I>
	advance(I& i, iter_difference_t<I> n, I bound)
	noexcept(noexcept(
		i = std::move(bound),
		__stl2::advance(i, bound - i)))
	{
		auto d = iter_difference_t<I>{bound - i};
		STL2_EXPECT(0 <= n ? 0 <= d : 0 >= d);
		if (0 <= n ? d <= n : d >= n) {
			i = std::move(bound);
			return n - d;
		}
		__stl2::advance(i, n);
		return 0;
	}

	// next
	struct __next_fn : private __niebloid {
		template<Iterator I>
		constexpr I operator()(I x, iter_difference_t<I> n = 1) const
		STL2_NOEXCEPT_RETURN(
			__stl2::advance(x, n),
			x
		)

		template<Iterator I, Sentinel<I> S>
		constexpr I operator()(I x, S bound) const
		STL2_NOEXCEPT_RETURN(
			__stl2::advance(x, std::forward<S>(bound)),
			x
		)

		template<Iterator I, Sentinel<I> S>
		constexpr I operator()(I x, iter_difference_t<I> n, S bound) const
		STL2_NOEXCEPT_RETURN(
			__stl2::advance(x, n, std::forward<S>(bound)),
			x
		)
	};

	inline constexpr __next_fn next {};

	// prev
	struct __prev_fn : private __niebloid {
		template<BidirectionalIterator I>
		constexpr I operator()(I x, iter_difference_t<I> n = 1) const
		STL2_NOEXCEPT_RETURN(
			__stl2::advance(x, -n),
			x
		)

		template<BidirectionalIterator I>
		constexpr I operator()(I x, iter_difference_t<I> n, I bound) const
		STL2_NOEXCEPT_RETURN(
			__stl2::advance(x, -n, std::move(bound)),
			x
		)
	};

	inline constexpr __prev_fn prev {};
} STL2_CLOSE_NAMESPACE

#endif
