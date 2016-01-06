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
    template <class T>
    constexpr bool __non_final_class =
      is_class<T>::value && !is_final<T>::value;

    template <Destructible T, class Tag = void>
    class ebo_box {
    public:
      ebo_box() requires DefaultConstructible<T>() = default;
      constexpr ebo_box(const T& t)
        noexcept(is_nothrow_copy_constructible<T>::value)
        requires CopyConstructible<T>() : item_(t) {}
      constexpr ebo_box(T&& t)
        noexcept(is_nothrow_move_constructible<T>::value)
        requires MoveConstructible<T>() : item_(__stl2::move(t)) {}

      template <typename First, typename...Rest>
        requires !Same<ebo_box, decay_t<First>>() &&
          Constructible<T, First, Rest...>()
      constexpr ebo_box(First&& f, Rest&&...r)
        noexcept(is_nothrow_constructible<T, First, Rest...>::value) :
        item_(__stl2::forward<First>(f), __stl2::forward<Rest>(r)...) {}

      constexpr T& get() & noexcept { return item_; }
      constexpr const T& get() const& noexcept { return item_; }
      constexpr T&& get() && noexcept { return __stl2::move(item_); }
      constexpr const T&& get() const&& noexcept { return __stl2::move(item_); }

    private:
      T item_;
    };

    template <Destructible T, class Tag>
      requires __non_final_class<T>
    class ebo_box<T, Tag> : private T {
    public:
      using T::T;

      ebo_box() requires DefaultConstructible<T>() = default;
      constexpr ebo_box(const T& t)
        noexcept(is_nothrow_copy_constructible<T>::value)
        requires CopyConstructible<T>() : T(t) {}
      constexpr ebo_box(T&& t)
        noexcept(is_nothrow_move_constructible<T>::value)
        requires MoveConstructible<T>() : T(__stl2::move(t)) {}

      constexpr T& get() & noexcept { return *this; }
      constexpr const T& get() const& noexcept { return *this; }
      constexpr T&& get() && noexcept { return __stl2::move(*this); }
      constexpr const T&& get() const&& noexcept { return __stl2::move(*this); }
    };
  }
} STL2_CLOSE_NAMESPACE

#endif
