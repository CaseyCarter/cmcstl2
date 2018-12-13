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
#ifndef STL2_DETAIL_RANGE_PRIMITIVES_HPP
#define STL2_DETAIL_RANGE_PRIMITIVES_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/iterator/dangling.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// Range primitives [range.primitives]
//
STL2_OPEN_NAMESPACE {
	// enumerate
	namespace ext {
		template<Iterator I, Sentinel<I> S>
		constexpr tagged_pair<tag::count(iter_difference_t<I>), tag::end(I)>
		enumerate(I first, S last)
		noexcept(noexcept(++first != last) &&
			std::is_nothrow_move_constructible<I>::value)
		{
			iter_difference_t<I> n = 0;
			while (first != last) {
				++n;
				++first;
			}
			return {n, std::move(first)};
		}

		template<Iterator I, SizedSentinel<I> S>
		constexpr tagged_pair<tag::count(iter_difference_t<I>), tag::end(I)>
		enumerate(I first, S last)
		noexcept(noexcept(next(std::move(first), std::move(last))) &&
			std::is_nothrow_move_constructible<I>::value)
		{
			auto d = last - first;
			STL2_EXPECT((Same<I, S> || d >= 0));
			return {d, next(std::move(first), std::move(last))};
		}

		template<Iterator I, Sentinel<I> S>
		requires (!SizedSentinel<S, I>) && SizedSentinel<I, I>
		constexpr tagged_pair<tag::count(iter_difference_t<I>), tag::end(I)>
		enumerate(I first, S last)
		noexcept(noexcept(next(first, std::move(last))) &&
			std::is_nothrow_move_constructible<I>::value)
		{
			auto end = next(first, std::move(last));
			auto n = end - first;
			return {n, std::move(end)};
		}

		template<Range R>
		constexpr auto enumerate(R&& r)
		STL2_NOEXCEPT_RETURN(
			__stl2::ext::enumerate(begin(r), end(r))
		)

		template<SizedRange R>
		constexpr auto enumerate(R&& r)
		STL2_NOEXCEPT_RETURN(
			tagged_pair<tag::count(iter_difference_t<iterator_t<R>>),
				tag::end(safe_iterator_t<R>)>{
				size(r),
				next(begin(r), end(r))
			}
		)
	}

	struct __distance_fn : private __niebloid {
		template<Iterator I, Sentinel<I> S>
		constexpr iter_difference_t<I> operator()(I first, S last) const
		// [[expects axiom: reachable(first, last)]]
		STL2_NOEXCEPT_RETURN(
			ext::enumerate(std::move(first), std::move(last)).first
		)

		template<Iterator I, SizedSentinel<I> S>
		constexpr iter_difference_t<I> operator()(I first, S last) const
		noexcept(noexcept(last - first))
		// [[expects axiom: reachable(first, last)]]
		{
			auto d = last - first;
			STL2_EXPECT(d >= 0);
			return d;
		}

		template<Iterator I>
		requires SizedSentinel<I, I>
		constexpr iter_difference_t<I> operator()(I first, I last) const
		// [[expects axiom: reachable(first, last)]]
		STL2_NOEXCEPT_RETURN(
			last - first
		)

		template<Range R>
		constexpr iter_difference_t<iterator_t<R>> operator()(R&& r) const
		noexcept(noexcept(std::declval<__distance_fn const&>()(begin(r), end(r))))
		{
			const iter_difference_t<iterator_t<R>> n =
				(*this)(begin(r), end(r));
			STL2_EXPECT(n >= 0);
			return n;
		}

		template<SizedRange R>
		constexpr iter_difference_t<iterator_t<R>> operator()(R&& r) const
		noexcept(noexcept(size(r)))
		{
			const auto n =
				static_cast<iter_difference_t<iterator_t<R>>>(size(r));
			STL2_EXPECT(n >= 0);
			return n;
		}
	};

	inline constexpr __distance_fn distance {};
} STL2_CLOSE_NAMESPACE

#endif
