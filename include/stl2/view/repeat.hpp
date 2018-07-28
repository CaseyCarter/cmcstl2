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

#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/cheap_storage.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/memory/addressof.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <CopyConstructibleObject T>
		class repeat_view : private detail::semiregular_box<T> {
			using storage_t = detail::semiregular_box<T>;
			using storage_t::get;

			class cursor : detail::cheap_reference_box_t<const T> {
				using storage_t = detail::cheap_reference_box_t<const T>;
				using storage_t::get;
			public:
				using difference_type = std::ptrdiff_t;
				using reference =
					meta::if_c<detail::cheaply_copyable<T>, T, const T&>;

				cursor() = default;
				constexpr explicit cursor(const repeat_view& r)
				noexcept(std::is_nothrow_constructible<storage_t, const T&>::value)
				: storage_t{r.value()} {}

				constexpr reference read() const
				noexcept(std::is_nothrow_constructible<reference, const T&>::value)
				{ return get(); }
				constexpr const T* arrow() const noexcept
				{ return detail::addressof(get()); }
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
			noexcept(std::is_nothrow_constructible<storage_t, T>::value)
			: storage_t{std::move(value)} {}

			constexpr iterator begin() const
			noexcept(std::is_nothrow_constructible<iterator, const repeat_view&>::value)
			{ return iterator{cursor{*this}}; }
			constexpr unreachable end() const noexcept { return {}; }
			constexpr const T& value() const noexcept { return get(); }
		};
	} // namespace ext

	namespace view::ext {
		class __repeat_fn {
			template <class T>
			constexpr auto operator()(T const& t) const
			STL2_NOEXCEPT_REQUIRES_RETURN(
				__stl2::ext::repeat_view{t}
			)

			template <class T>
			constexpr auto operator()(remove_reference_t<T>&& t) const
			STL2_NOEXCEPT_REQUIRES_RETURN(
				__stl2::ext::repeat_view{std::move(t)}
			)
		};

		inline constexpr __repeat_fn repeat {};
	} // namespace view::ext

	template <class T>
	struct enable_view<ext::repeat_view<T>> : std::true_type {};
} STL2_CLOSE_NAMESPACE

#endif
