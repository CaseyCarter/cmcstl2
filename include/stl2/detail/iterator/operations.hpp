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
//
STL2_OPEN_NAMESPACE {
	// advance
	namespace __advance {
		template <class I>
		requires
			models::Iterator<I>
			// Pre: 0 <= n && [i,i+n)
		constexpr void impl(I& i, difference_type_t<I> n) {
			STL2_EXPECT(0 <= n);
			while (n != 0) {
				--n;
				++i;
			}
		}
	}

	Iterator{I}
		// Pre: 0 <= n && [i,i+n)
	constexpr void advance(I& i, difference_type_t<I> n) {
		__advance::impl(i, n);
	}

	BidirectionalIterator{I}
		// Pre: 0 <= n ? [i,i+n) : [i+n,i)
	constexpr void advance(I& i, difference_type_t<I> n) {
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
	constexpr void advance(I& i, difference_type_t<I> n) {
		i += n;
	}

	template <class I, class S>
	requires
		models::Sentinel<S, I>
		// Pre: [i,bound)
	constexpr void advance(I& i, S bound) {
		while (i != bound) {
			++i;
		}
	}

	template <class I, class S>
	requires
		models::Sentinel<S, I> && models::Assignable<I&, S&&>
	constexpr void advance(I& i, S bound) {
		i = __stl2::move(bound);
	}

	template <class I, class S>
	requires
		models::Sentinel<S, I> && !models::Assignable<I&, S&&> &&
		models::SizedSentinel<S, I>
		// Pre: [i,bound)
	constexpr void advance(I& i, S bound) {
		difference_type_t<I> d = bound - i;
		STL2_EXPECT(0 <= d);
		__stl2::advance(i, d);
	}

	namespace __advance {
		template <class I, class S>
		requires
			models::Sentinel<S, I>
			// Pre: 0 == n || (0 < n && [i,bound))
		constexpr difference_type_t<I>
		impl(I& i, difference_type_t<I> n, const S& bound) {
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
		models::Sentinel<S, I>
		// Pre: 0 == n || (0 < n && [i,bound))
	constexpr difference_type_t<I>
	advance(I& i, difference_type_t<I> n, S bound) {
		return __advance::impl(i, n, bound);
	}

	template <class I, class S>
	requires
		models::Sentinel<S, I> && models::SizedSentinel<S, I>
		// Pre: 0 <= n && [i,bound)
	constexpr difference_type_t<I>
	advance(I& i, difference_type_t<I> n, S bound) {
		STL2_EXPECT(0 <= n);
		auto d = difference_type_t<I>{bound - i};
		STL2_EXPECT(0 <= d);
		if (d <= n) {
			__stl2::advance(i, __stl2::move(bound));
			return n - d;
		}
		__stl2::advance(i, n);
		return 0;
	}

	template <class I>
	requires
		models::BidirectionalIterator<I>
		// Pre: 0 == n || (0 < n ? [i,bound) : [bound,i))
	constexpr difference_type_t<I>
	advance(I& i, difference_type_t<I> n, I bound) {
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
		models::BidirectionalIterator<I> && models::SizedSentinel<I, I>
		// Pre: 0 == n ? ([i,bound) || [bound,i)) : (0 < n ? [i,bound) : [bound,i))
	constexpr difference_type_t<I>
	advance(I& i, difference_type_t<I> n, I bound) {
		auto d = difference_type_t<I>{bound - i};
		STL2_EXPECT(0 <= n ? 0 <= d : 0 >= d);
		if (0 <= n ? d <= n : d >= n) {
			i = __stl2::move(bound);
			return n - d;
		}
		__stl2::advance(i, n);
		return 0;
	}

	// next
	Iterator{I}
	constexpr I next(I x, difference_type_t<I> n = 1) {
		__stl2::advance(x, n);
		return x;
	}

	template <class S, class I>
	requires
		models::Sentinel<__f<S>, I>
	constexpr I next(I x, S&& bound) {
		__stl2::advance(x, __stl2::forward<S>(bound));
		return x;
	}

	template <class S, class I>
	requires
		models::Sentinel<__f<S>, I>
	constexpr I next(I x, difference_type_t<I> n, S&& bound) {
		__stl2::advance(x, n, __stl2::forward<S>(bound));
		return x;
	}

	// prev
	BidirectionalIterator{I}
	constexpr I prev(I x, difference_type_t<I> n = 1) {
		__stl2::advance(x, -n);
		return x;
	}

	BidirectionalIterator{I}
	constexpr I prev(I x, difference_type_t<I> n, I bound) {
		__stl2::advance(x, -n, std::move(bound));
		return x;
	}

	namespace ext {
		Sentinel{S, I}
		constexpr tagged_pair<tag::count(difference_type_t<I>), tag::end(I)>
		enumerate(I first, S last) {
			difference_type_t<I> n = 0;
			while (first != last) {
				++n;
				++first;
			}
			return {n, __stl2::move(first)};
		}

		SizedSentinel{S, I}
		constexpr tagged_pair<tag::count(difference_type_t<I>), tag::end(I)>
		enumerate(I first, S last) {
			auto d = last - first;
			STL2_EXPECT((models::Same<I, S> || d >= 0));
			return {d, __stl2::next(__stl2::move(first), __stl2::move(last))};
		}

		template <class S, class I>
		requires
			Sentinel<S, I>() && !SizedSentinel<S, I>() && SizedSentinel<I, I>()
		constexpr tagged_pair<tag::count(difference_type_t<I>), tag::end(I)>
		enumerate(I first, S last) {
			auto end = __stl2::next(first, __stl2::move(last));
			auto n = end - first;
			return {n, __stl2::move(end)};
		}
	}

	// distance
	Sentinel{S, I}
		// Pre: [first, last)
	constexpr difference_type_t<I> distance(I first, S last) {
		return ext::enumerate(__stl2::move(first), __stl2::move(last)).first;
	}

	SizedSentinel{S, I}
		// Pre: [first, last)
	constexpr difference_type_t<I> distance(I first, S last) {
		auto d = last - first;
		STL2_EXPECT(d >= 0);
		return d;
	}

	template <class I>
	requires
		SizedSentinel<I, I>()
		// Pre: [first, last) || [last, first)
	constexpr difference_type_t<I> distance(I first, I last) {
		return last - first;
	}
} STL2_CLOSE_NAMESPACE

#endif
