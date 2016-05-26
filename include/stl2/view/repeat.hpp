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
#ifndef STL2_DETAIL_VIEW_REPEAT_HPP
#define STL2_DETAIL_VIEW_REPEAT_HPP

#include <stl2/iterator.hpp>
#include <stl2/memory.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/cheap_storage.hpp>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>

STL2_OPEN_NAMESPACE {
	template <Semiregular T>
	class repeat_view : view_base, detail::ebo_box<T> {
		using storage_t = detail::ebo_box<T>;

		class cursor : detail::cheap_reference_box_t<const T> {
			using storage_t = detail::cheap_reference_box_t<const T>;
		public:
			using difference_type = std::ptrdiff_t;
			using reference =
				meta::if_c<detail::cheaply_copyable<T>, T, const T&>;

			cursor() = default;
			constexpr cursor(const repeat_view& r)
			noexcept(is_nothrow_constructible<storage_t, const T&>::value)
			: storage_t{r.value()}
			{}

			constexpr reference read() const noexcept
			{ return storage_t::get(); }
			constexpr bool done() const noexcept { return false; }
			constexpr bool equal(const cursor&) const noexcept { return true; }
			constexpr void next() const noexcept {}
			constexpr void prev() const noexcept {}
			constexpr void advance(difference_type) const noexcept {}
			constexpr difference_type distance_to(const cursor&) const noexcept
			{ return 0; }
		};

	public:
		using iterator = basic_iterator<cursor>;

		repeat_view() = default;
		constexpr repeat_view(T value)
		noexcept(is_nothrow_constructible<storage_t, T>::value)
		: storage_t{__stl2::move(value)}
		{}

		constexpr iterator begin() const
		noexcept(is_nothrow_constructible<iterator, const repeat_view&>::value)
		{ return {cursor{*this}}; }
		constexpr unreachable end() const noexcept { return {}; }
		constexpr const T& value() const noexcept { return storage_t::get(); }
	};
} STL2_CLOSE_NAMESPACE

#endif
