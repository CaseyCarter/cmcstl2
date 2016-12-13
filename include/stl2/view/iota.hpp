// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015-2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_IOTA_HPP
#define STL2_VIEW_IOTA_HPP

#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		WeaklyIncrementable{I}
		class iota_view : view_base {
			I first_{};

			class cursor {
				I value_{};
			public:
				using difference_type = difference_type_t<I>;

				cursor() = default;
				constexpr cursor(const iota_view& v)
				noexcept(is_nothrow_copy_constructible<I>::value)
				: value_{v.first_} {}

				constexpr I read() const
				noexcept(is_nothrow_copy_constructible<I>::value)
				{ return value_; }

				constexpr void next()
				STL2_NOEXCEPT_RETURN(
					(void)++value_
				)

				constexpr bool equal(const cursor& that) const
				noexcept(noexcept(value_ == that.value_))
				requires EqualityComparable<I>()
				{ return value_ == that.value_; }

				constexpr void prev()
				noexcept(noexcept(--value_))
				requires ext::Decrementable<I>()
				{ --value_; }

				constexpr void advance(difference_type n)
				noexcept(noexcept(value_ += n))
				requires ext::RandomAccessIncrementable<I>()
				{ value_ += n; }

				constexpr difference_type distance_to(const cursor& that)
				noexcept(noexcept(that.value_ - value_))
				requires
					ext::RandomAccessIncrementable<I>() || SizedSentinel<I, I>()
				{ return that.value_ - value_; }
			};

		public:
			iota_view() = default;
			constexpr iota_view(I first)
			noexcept(is_nothrow_move_constructible<I>::value)
			: first_(__stl2::move(first)) {}

			using iterator = basic_iterator<cursor>;
			constexpr iterator begin() const
			noexcept(noexcept(iterator{declval<const iota_view&>()}))
			{ return {cursor{*this}}; }

			constexpr unreachable end() const noexcept { return {}; }
		};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
