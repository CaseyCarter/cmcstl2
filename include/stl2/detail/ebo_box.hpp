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
#ifndef STL2_DETAIL_EBO_BOX_HPP
#define STL2_DETAIL_EBO_BOX_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		template <ext::DestructibleObject T, class Tag = void>
		struct ebo_box {
			ebo_box() = default;
			constexpr ebo_box(const T& t)
			noexcept(std::is_nothrow_copy_constructible<T>::value)
			requires CopyConstructible<T>
			: item_(t)
			{}
			constexpr ebo_box(T&& t)
			noexcept(std::is_nothrow_move_constructible<T>::value)
			requires MoveConstructible<T>
			: item_(std::move(t))
			{}

			template <class First>
			requires !_OneOf<std::decay_t<First>, ebo_box, T> &&
				Constructible<T, First> && ConvertibleTo<First, T>
			constexpr ebo_box(First&& f)
			noexcept(std::is_nothrow_constructible<T, First>::value)
			: item_(std::forward<First>(f))
			{}
			template <class First, class... Rest>
			requires (sizeof...(Rest) > 0 || !_OneOf<std::decay_t<First>, ebo_box, T>) &&
				Constructible<T, First, Rest...>
			constexpr explicit ebo_box(First&& f, Rest&&...r)
			noexcept(std::is_nothrow_constructible<T, First, Rest...>::value)
			: item_(std::forward<First>(f), std::forward<Rest>(r)...)
			{}

			constexpr T& get() & noexcept { return item_; }
			constexpr const T& get() const& noexcept { return item_; }
			constexpr T&& get() && noexcept { return std::move(item_); }
			constexpr const T&& get() const&& noexcept { return std::move(item_); }

		private:
			T item_;
		};

		template <ext::DestructibleObject T, class Tag>
		requires std::is_empty<T>::value && !std::is_final<T>::value
		struct ebo_box<T, Tag> : private T {
			ebo_box() = default;
			constexpr ebo_box(const T& t)
			noexcept(std::is_nothrow_copy_constructible<T>::value)
			requires CopyConstructible<T>
			: T(t)
			{}
			constexpr ebo_box(T&& t)
			noexcept(std::is_nothrow_move_constructible<T>::value)
			requires MoveConstructible<T>
			: T(std::move(t))
			{}
			using T::T;

			constexpr T& get() & noexcept { return *this; }
			constexpr const T& get() const& noexcept { return *this; }
			constexpr T&& get() && noexcept { return std::move(*this); }
			constexpr const T&& get() const&& noexcept { return std::move(*this); }
		};
	}
} STL2_CLOSE_NAMESPACE

#endif
