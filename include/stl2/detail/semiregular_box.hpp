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
#ifndef STL2_DETAIL_SEMIREGULAR_BOX_HPP
#define STL2_DETAIL_SEMIREGULAR_BOX_HPP

#include <optional>
#include <type_traits>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		// If T models MoveConstructible, semiregular_box<T> models Movable &&
		// DefaultConstructible (so-called "MoveSemiregular"). If T models
		// CopyConstructible, semiregular_box<T> models Semiregular.
		template<ext::MoveConstructibleObject T>
		struct semiregular_box {
			semiregular_box() = default;
			constexpr semiregular_box()
			noexcept(std::is_nothrow_default_constructible_v<T>)
			requires DefaultConstructible<T>
			: o_{std::in_place} {}

			template<_NotSameAs<semiregular_box> U>
			requires ConvertibleTo<U, T>
			constexpr semiregular_box(U&& u)
			noexcept(std::is_nothrow_constructible_v<T, U>)
			: o_{std::in_place, static_cast<U&&>(u)} {}

			semiregular_box(const semiregular_box&) requires CopyConstructible<T> = default;
			semiregular_box(semiregular_box&&) = default;

			template<class... Args>
			requires Constructible<T, Args...>
			constexpr semiregular_box(std::in_place_t, Args&&... args)
			noexcept(std::is_nothrow_constructible_v<T, Args...>)
			: o_{std::in_place, static_cast<Args&&>(args)...} {}

			constexpr semiregular_box& operator=(const semiregular_box& that) &
			noexcept(std::is_nothrow_copy_constructible_v<T>)
			requires CopyConstructible<T>
			{
				o_.reset();
				if (that.o_)
					o_.emplace(*that.o_);
				return *this;
			}
			semiregular_box& operator=(const semiregular_box&) & requires Copyable<T> = default;

			constexpr semiregular_box& operator=(semiregular_box&& that) &
			noexcept(std::is_nothrow_move_constructible_v<T>)
			{
				o_.reset();
				if (that.o_)
					o_.emplace(static_cast<T&&>(*that.o_));
				return *this;
			}
			semiregular_box& operator=(semiregular_box&&) & requires Movable<T> = default;

			constexpr semiregular_box& operator=(T&& t) &
			noexcept(std::is_nothrow_move_constructible_v<T>)
			{
				o_.emplace(static_cast<T&&>(t));
				return *this;
			}
			constexpr semiregular_box& operator=(T&& t) &
			noexcept(std::is_nothrow_move_constructible_v<T> &&
				std::is_nothrow_move_assignable_v<T>)
			requires Movable<T>
			{
				o_ = static_cast<T&&>(t);
				return *this;
			}

			constexpr semiregular_box& operator=(const T& t) &
			noexcept(std::is_nothrow_copy_constructible_v<T>)
			requires CopyConstructible<T>
			{
				o_.emplace(t);
				return *this;
			}
			constexpr semiregular_box& operator=(const T& t) &
			noexcept(std::is_nothrow_copy_constructible_v<T> &&
				std::is_nothrow_copy_assignable_v<T>)
			requires Copyable<T>
			{
				o_ = t;
				return *this;
			}

			constexpr T& get() & noexcept {
				STL2_EXPECT(o_);
				return *o_;
			}
			constexpr const T& get() const& noexcept {
				STL2_EXPECT(o_);
				return *o_;
			}
			constexpr T&& get() && noexcept {
				STL2_EXPECT(o_);
				return static_cast<T&&>(*o_);
			}
			constexpr const T&& get() const&& noexcept {
				STL2_EXPECT(o_);
				return static_cast<const T&&>(*o_);
			}

		private:
			std::optional<T> o_;
		};

		template<Semiregular T>
		requires ext::Object<T>
		struct semiregular_box<T> : ebo_box<T, semiregular_box<T>> {
			using semiregular_box::ebo_box::ebo_box;

			template<class... Args>
			requires Constructible<T, Args...>
			constexpr semiregular_box(std::in_place_t, Args&&... args)
			noexcept(std::is_nothrow_constructible_v<T, Args...>)
			: semiregular_box::ebo_box{static_cast<Args&&>(args)...} {}
		};

		template<class T>
		semiregular_box(T) -> semiregular_box<T>;
	}
} STL2_CLOSE_NAMESPACE

#endif
