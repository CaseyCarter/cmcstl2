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
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// Iterator operations [iterator.operations]
// Not to spec: All constexpr per P0579.
STL2_OPEN_NAMESPACE {
	// advance
	namespace __advance {
		template <class I>
		requires
			Iterator<I>()
			// Pre: 0 <= n && [i,i+n)
		constexpr void impl(I& i, difference_type_t<I> n)
		noexcept(noexcept(++std::declval<I&>()))
		{
			STL2_EXPECT(0 <= n);
			while (n != 0) {
				--n;
				++i;
			}
		}
	}

	Iterator{I}
		// Pre: 0 <= n && [i,i+n)
	constexpr void advance(I& i, difference_type_t<I> n)
	noexcept(noexcept(++std::declval<I&>()))
	{
		__advance::impl(i, n);
	}

	BidirectionalIterator{I}
		// Pre: 0 <= n ? [i,i+n) : [i+n,i)
	constexpr void advance(I& i, difference_type_t<I> n)
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

	RandomAccessIterator{I}
		// Pre: 0 <= n ? [i,i+n) : [i+n,i)
	constexpr void advance(I& i, difference_type_t<I> n)
	STL2_NOEXCEPT_RETURN(
		(void)(i += n)
	)

	template <class I, class S>
	requires
		Sentinel<S, I>()
		// Pre: [i,bound)
	constexpr void advance(I& i, S bound)
	noexcept(noexcept(++i != bound))
	{
		while (i != bound) {
			++i;
		}
	}

	template <class I, class S>
	requires
		Sentinel<S, I>() && Assignable<I&, S&&>()
	constexpr void advance(I& i, S bound)
	STL2_NOEXCEPT_RETURN(
		(void)(i = std::move(bound))
	)

	template <class I, class S>
	requires
		Sentinel<S, I>() && !Assignable<I&, S&&>() &&
		SizedSentinel<S, I>()
		// Pre: [i,bound)
	constexpr void advance(I& i, S bound)
	noexcept(noexcept(__stl2::advance(i, bound - i)))
	{
		difference_type_t<I> d = bound - i;
		STL2_EXPECT(0 <= d);
		__stl2::advance(i, d);
	}

	namespace __advance {
		template <class I, class S>
		requires
			Sentinel<S, I>()
			// Pre: 0 == n || (0 < n && [i,bound))
		constexpr difference_type_t<I>
		impl(I& i, difference_type_t<I> n, const S& bound)
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

	template <class I, class S>
	requires
		Sentinel<S, I>()
		// Pre: 0 == n || (0 < n && [i,bound))
	constexpr difference_type_t<I>
	advance(I& i, difference_type_t<I> n, S bound)
	STL2_NOEXCEPT_RETURN(
		__advance::impl(i, n, bound)
	)

	template <class I, class S>
	requires
		Sentinel<S, I>() && SizedSentinel<S, I>()
		// Pre: 0 <= n && [i,bound)
	constexpr difference_type_t<I>
	advance(I& i, difference_type_t<I> n, S bound)
	noexcept(noexcept(
		__stl2::advance(i, std::move(bound)),
		__stl2::advance(i, bound - i)))
	{
		STL2_EXPECT(0 <= n);
		auto d = difference_type_t<I>{bound - i};
		STL2_EXPECT(0 <= d);
		if (d <= n) {
			__stl2::advance(i, std::move(bound));
			return n - d;
		}
		__stl2::advance(i, n);
		return 0;
	}

	template <class I>
	requires
		BidirectionalIterator<I>()
		// Pre: 0 == n || (0 < n ? [i,bound) : [bound,i))
	constexpr difference_type_t<I>
	advance(I& i, difference_type_t<I> n, I bound)
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

	template <class I>
	requires
		BidirectionalIterator<I>() && SizedSentinel<I, I>()
		// Pre: 0 == n ? ([i,bound) || [bound,i)) : (0 < n ? [i,bound) : [bound,i))
	constexpr difference_type_t<I>
	advance(I& i, difference_type_t<I> n, I bound)
	noexcept(noexcept(
		i = std::move(bound),
		__stl2::advance(i, bound - i)))
	{
		auto d = difference_type_t<I>{bound - i};
		STL2_EXPECT(0 <= n ? 0 <= d : 0 >= d);
		if (0 <= n ? d <= n : d >= n) {
			i = std::move(bound);
			return n - d;
		}
		__stl2::advance(i, n);
		return 0;
	}

	// next
	Iterator{I}
	constexpr I next(I x, difference_type_t<I> n = 1)
	STL2_NOEXCEPT_RETURN(
		__stl2::advance(x, n),
		x
	)

	template <class S, class I>
	requires
		Sentinel<__f<S>, I>()
	constexpr I next(I x, S&& bound)
	STL2_NOEXCEPT_RETURN(
		__stl2::advance(x, std::forward<S>(bound)),
		x
	)

	template <class S, class I>
	requires
		Sentinel<__f<S>, I>()
	constexpr I next(I x, difference_type_t<I> n, S&& bound)
	STL2_NOEXCEPT_RETURN(
		__stl2::advance(x, n, std::forward<S>(bound)),
		x
	)

	// prev
	BidirectionalIterator{I}
	constexpr I prev(I x, difference_type_t<I> n = 1)
	STL2_NOEXCEPT_RETURN(
		__stl2::advance(x, -n),
		x
	)

	BidirectionalIterator{I}
	constexpr I prev(I x, difference_type_t<I> n, I bound)
	STL2_NOEXCEPT_RETURN(
		__stl2::advance(x, -n, std::move(bound)),
		x
	)

	namespace ext {
		Sentinel{S, I}
		constexpr tagged_pair<tag::count(difference_type_t<I>), tag::end(I)>
		enumerate(I first, S last)
		noexcept(noexcept(++first != last) &&
			std::is_nothrow_move_constructible<I>::value)
		{
			difference_type_t<I> n = 0;
			while (first != last) {
				++n;
				++first;
			}
			return {n, std::move(first)};
		}

		SizedSentinel{S, I}
		constexpr tagged_pair<tag::count(difference_type_t<I>), tag::end(I)>
		enumerate(I first, S last)
		noexcept(noexcept(__stl2::next(std::move(first), std::move(last))) &&
			std::is_nothrow_move_constructible<I>::value)
		{
			auto d = last - first;
			STL2_EXPECT((models::Same<I, S> || d >= 0));
			return {d, __stl2::next(std::move(first), std::move(last))};
		}

		template <class S, class I>
		requires
			Sentinel<S, I>() && !SizedSentinel<S, I>() && SizedSentinel<I, I>()
		constexpr tagged_pair<tag::count(difference_type_t<I>), tag::end(I)>
		enumerate(I first, S last)
		noexcept(noexcept(__stl2::next(first, std::move(last))) &&
			std::is_nothrow_move_constructible<I>::value)
		{
			auto end = __stl2::next(first, std::move(last));
			auto n = end - first;
			return {n, std::move(end)};
		}
	}

	// distance
	Sentinel{S, I}
		// Pre: [first, last)
	constexpr difference_type_t<I> distance(I first, S last)
	STL2_NOEXCEPT_RETURN(
		ext::enumerate(std::move(first), std::move(last)).first
	)

	SizedSentinel{S, I}
		// Pre: [first, last)
	constexpr difference_type_t<I> distance(I first, S last)
	noexcept(noexcept(last - first))
	{
		auto d = last - first;
		STL2_EXPECT(d >= 0);
		return d;
	}

	template <class I>
	requires
		SizedSentinel<I, I>()
		// Pre: [first, last) || [last, first)
	constexpr difference_type_t<I> distance(I first, I last)
	STL2_NOEXCEPT_RETURN(
		last - first
	)
} STL2_CLOSE_NAMESPACE

#endif
