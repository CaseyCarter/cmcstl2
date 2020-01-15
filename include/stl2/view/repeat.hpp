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
#ifndef STL2_VIEW_REPEAT_HPP
#define STL2_VIEW_REPEAT_HPP

#include <memory>
#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<copy_constructible_object T>
		struct STL2_EMPTY_BASES repeat_view
		: private detail::semiregular_box<T>
		, view_interface<repeat_view<T>>
		{
		private:
			using storage_t = detail::semiregular_box<T>;
			using storage_t::get;

			template<bool IsConst>
			struct cursor {
			private:
				friend cursor<true>;
				using E = __maybe_const<IsConst, T>;
				detail::raw_ptr<E> value_;

			public:
				cursor() = default;
				constexpr explicit cursor(__maybe_const<IsConst, repeat_view>& r) noexcept
				: value_{std::addressof(r.get())} {}
				constexpr cursor(const cursor<!IsConst>& that) noexcept requires IsConst
				: value_{that.value_} {}

				constexpr E& read() const noexcept
				{ return *value_; }
				constexpr E* arrow() const noexcept
				{ return value_; }
				constexpr bool equal(const cursor&) const noexcept { return true; }
				constexpr void next() noexcept {}
				constexpr void prev() noexcept {}
				constexpr void advance(std::ptrdiff_t) noexcept {}
				constexpr std::ptrdiff_t distance_to(const cursor&) const noexcept
				{ return 0; }
			};

		public:
			repeat_view() = default;
			template<_NotSameAs<repeat_view> U>
			requires convertible_to<U, T>
			explicit constexpr repeat_view(U&& u)
			noexcept(std::is_nothrow_constructible_v<T, U>)
			: storage_t{static_cast<U&&>(u)} {}

			constexpr T& value() noexcept { return get(); }
			constexpr const T& value() const noexcept { return get(); }

			constexpr basic_iterator<cursor<false>> begin() noexcept
			{ return basic_iterator{cursor<false>{*this}}; }
			constexpr basic_iterator<cursor<true>> begin() const noexcept
			{ return basic_iterator{cursor<true>{*this}}; }

			constexpr unreachable_sentinel_t end() const noexcept { return {}; }
		};

		template<class T>
		repeat_view(T) -> repeat_view<T>;
	} // namespace ext

	namespace views::ext {
		struct __repeat_fn {
			template<class T>
			constexpr auto operator()(T&& t) const
			STL2_REQUIRES_RETURN(
				__stl2::ext::repeat_view{static_cast<T&&>(t)}
			)
		};

		inline constexpr __repeat_fn repeat;
	} // namespace views::ext
} STL2_CLOSE_NAMESPACE

#endif
