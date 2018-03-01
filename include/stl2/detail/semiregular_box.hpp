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

#include <type_traits>
#include <stl2/optional.hpp>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		// If T models MoveConstructible, semiregular_box<T> models Movable &&
		// DefaultConstructible (so-called "MoveSemiregular"). If T models
		// CopyConstructible, semiregular_box<T> models Semiregular.
		template <ext::MoveConstructibleObject T>
		struct semiregular_box {
			semiregular_box() = default;
			constexpr semiregular_box()
			noexcept(is_nothrow_default_constructible<T>::value)
			requires DefaultConstructible<T>
			: o_{in_place} {}

			constexpr semiregular_box(T&& t)
			noexcept(is_nothrow_move_constructible<T>::value)
			: o_{in_place, std::move(t)} {}

			constexpr semiregular_box(const T& t)
			noexcept(is_nothrow_copy_constructible<T>::value)
			requires CopyConstructible<T>
			: o_{in_place, t} {}

			semiregular_box(const semiregular_box&) requires CopyConstructible<T> = default;
			semiregular_box(semiregular_box&&) = default;

			template <class... Args>
			requires Constructible<T, Args...>
			constexpr semiregular_box(in_place_t, Args&&... args)
			noexcept(is_nothrow_constructible<T, Args...>::value)
			: o_{in_place, std::forward<Args>(args)...} {}

			constexpr semiregular_box& operator=(const semiregular_box& that) &
			noexcept(is_nothrow_copy_constructible<T>::value)
			requires CopyConstructible<T>
			{
				o_.reset();
				if (that.o_)
					o_.emplace(*that.o_);
				return *this;
			}
			semiregular_box& operator=(const semiregular_box&) & requires Copyable<T> = default;

			constexpr semiregular_box& operator=(semiregular_box&& that) &
			noexcept(is_nothrow_move_constructible<T>::value)
			{
				o_.reset();
				if (that.o_)
					o_.emplace(std::move(*that.o_));
				return *this;
			}
			semiregular_box& operator=(semiregular_box&&) & requires Movable<T> = default;

			constexpr semiregular_box& operator=(T&& t) &
			noexcept(is_nothrow_move_constructible<T>::value)
			{
				o_.emplace(std::move(t));
				return *this;
			}
			constexpr semiregular_box& operator=(T&& t) &
			noexcept(is_nothrow_move_constructible<T>::value &&
				is_nothrow_move_assignable<T>::value)
			requires Movable<T>
			{
				o_ = std::move(t);
				return *this;
			}

			constexpr semiregular_box& operator=(const T& t) &
			noexcept(is_nothrow_copy_constructible<T>::value)
			requires CopyConstructible<T>
			{
				o_.emplace(t);
				return *this;
			}
			constexpr semiregular_box& operator=(const T& t) &
			noexcept(is_nothrow_copy_constructible<T>::value &&
				is_nothrow_copy_assignable<T>::value)
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
				return std::move(*o_);
			}
			constexpr const T&& get() const&& noexcept {
				STL2_EXPECT(o_);
				return std::move(*o_);
			}

		private:
			optional<T> o_;
		};

		template <Semiregular T>
		struct semiregular_box<T> : public ebo_box<T, semiregular_box<T>> {
			semiregular_box() = default;

			template <class... Args>
			requires Constructible<T, Args...>
			constexpr semiregular_box(in_place_t, Args&&... args)
			noexcept(is_nothrow_constructible<T, Args...>::value)
			: semiregular_box::ebo_box{std::forward<Args>(args)...} {}

			using semiregular_box::ebo_box::ebo_box;
		};
	}
} STL2_CLOSE_NAMESPACE

#endif
