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

#include <stl2/optional.hpp>
#include <stl2/detail/cheap_storage.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		template <Destructible T>
		requires _Is<T, is_object>
		class semiregular_box {
		public:
			semiregular_box() = default;
			constexpr semiregular_box()
			noexcept(is_nothrow_default_constructible<T>::value)
			requires DefaultConstructible<T>
			: o_{in_place} {}

			constexpr semiregular_box(T&& t)
			noexcept(is_nothrow_move_constructible<T>::value)
			requires MoveConstructible<T>
			: o_{in_place, __stl2::move(t)} {}

			constexpr semiregular_box(const T& t)
			noexcept(is_nothrow_copy_constructible<T>::value)
			requires CopyConstructible<T>
			: o_{in_place, t} {}

			template <class...Args>
			requires Constructible<T, Args...>
			constexpr semiregular_box(in_place_t, Args&&...args)
			noexcept(is_nothrow_constructible<T, Args...>::value)
			: o_{in_place, __stl2::forward<Args>(args)...} {}

			constexpr semiregular_box& operator=(T&& t) &
			noexcept(is_nothrow_move_constructible<T>::value)
			requires MoveConstructible<T>
			{
				o_.emplace(__stl2::move(t));
				return *this;
			}
			constexpr semiregular_box& operator=(T&& t) &
			noexcept(is_nothrow_move_assignable<T>::value)
			requires Movable<T>
			{
				o_ = t;
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
			noexcept(is_nothrow_copy_assignable<T>::value)
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
				return __stl2::move(*o_);
			}
			constexpr const T&& get() const&& noexcept {
				STL2_EXPECT(o_);
				return __stl2::move(*o_);
			}

		private:
			optional<T> o_;
		};

		template <class T>
		requires Destructible<T> && Semiregular<T>
		class semiregular_box<T> : public ebo_box<T, semiregular_box<T>> {
			using base_t = ebo_box<T, semiregular_box<T>>;
		public:
			semiregular_box() = default;

			template <class...Args>
			requires Constructible<T, Args...>
			constexpr semiregular_box(in_place_t, Args&&...args)
			noexcept(is_nothrow_constructible<T, Args...>::value)
			: base_t{__stl2::forward<Args>(args)...} {}

			using base_t::base_t;
		};
	}
} STL2_CLOSE_NAMESPACE

#endif
