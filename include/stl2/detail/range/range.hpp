// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_RANGE_RANGE_HPP
#define STL2_DETAIL_RANGE_RANGE_HPP

#include <stl2/type_traits.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/iterator/common_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/operations.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		// TODO: replace with subrange from P0789
		template <ext::DestructibleObject I, ext::DestructibleObject S = I>
		struct range;

		template <Iterator I, Sentinel<I> S>
		struct range<I, S>
		: tagged_pair<tag::begin(I), tag::end(S)> {
			range() = default;
			using tagged_pair<tag::begin(I), tag::end(S)>::tagged_pair;
		};

		template <Iterator I, Sentinel<I> S>
		struct range<dangling<I>, dangling<S>>
		: tagged_pair<tag::begin(dangling<I>), tag::end(dangling<S>)> {
			range() = default;
			using tagged_pair<tag::begin(dangling<I>), tag::end(dangling<S>)>::tagged_pair;
		};

		template <class I, class S>
		constexpr I begin(ext::range<I, S>&& r)
		STL2_NOEXCEPT_RETURN(
			r.begin()
		)

		template <class I, class S>
		constexpr S end(ext::range<I, S>&& r)
		STL2_NOEXCEPT_RETURN(
			r.end()
		)

		template <Iterator I, Sentinel<I> S>
		constexpr auto make_range(I i, S s)
		STL2_NOEXCEPT_RETURN(
			range<I, S>{std::move(i), std::move(s)}
		)

		namespace __make_bounded_range {
			template <Iterator I, Sentinel<I> S>
			requires ConvertibleTo<S, I>
			constexpr auto impl(ext::priority_tag<3>, I i, S s)
			STL2_NOEXCEPT_RETURN(
				range<I>{std::move(i), I{std::move(s)}}
			)

			template <RandomAccessIterator I, SizedSentinel<I> S>
			constexpr auto impl(ext::priority_tag<2>, I i, S s) {
				I end = __stl2::next(i, std::move(s));
				return range<I>{std::move(i), std::move(end)};
			}

			template <Iterator I, Sentinel<I> S>
			requires Common<I, S>
			constexpr auto impl(ext::priority_tag<1>, I i, S s)
			STL2_NOEXCEPT_RETURN(
				range<common_type_t<I, S>>{std::move(i), std::move(s)}
			)

			template <Iterator I, Sentinel<I> S>
			constexpr auto impl(ext::priority_tag<0>, I i, S s)
			STL2_NOEXCEPT_RETURN(
				range<common_iterator<I, S>>{std::move(i), std::move(s)}
			)
		}

		template <Iterator I, Sentinel<I> S>
		constexpr auto make_bounded_range(I i, S s)
		STL2_NOEXCEPT_RETURN(
			__make_bounded_range::impl(
				ext::max_priority_tag, std::move(i), std::move(s))
		)

		template <Iterator I>
		requires Sentinel<I, I>
		constexpr auto make_bounded_range(I i, I s)
		STL2_NOEXCEPT_RETURN(
			range<I>{std::move(i), std::move(s)}
		)
	}
} STL2_CLOSE_NAMESPACE

#endif
