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
#ifndef STL2_DETAIL_CHEAP_STORAGE_HPP
#define STL2_DETAIL_CHEAP_STORAGE_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
  namespace detail {
    template <class T, class Tag = void>
    class ebo_box {
      T item_;
    public:
      ebo_box() requires DefaultConstructible<T>() = default;
      constexpr ebo_box(const T& t)
        noexcept(is_nothrow_copy_constructible<T>::value)
        requires CopyConstructible<T>() : item_(t) {}
      constexpr ebo_box(T&& t)
        noexcept(is_nothrow_move_constructible<T>::value)
        requires MoveConstructible<T>() : item_(__stl2::move(t)) {}

      template <typename U>
        requires !Same<ebo_box, decay_t<U>>() &&
          Constructible<T, U>()
      constexpr ebo_box(U&& u)
        noexcept(is_nothrow_constructible<T, U>::value) :
        item_(__stl2::forward<U>(u)) {}

      constexpr T& get() & noexcept { return item_; }
      constexpr const T& get() const& noexcept { return item_; }
      constexpr T&& get() && noexcept { return __stl2::move(item_); }
    };

    template <class T, class Tag>
      requires _Is<T, is_empty> && _IsNot<T, is_final>
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
    };

    constexpr std::size_t cheap_copy_size = 32;

    template <class T>
    constexpr bool cheaply_copyable = false;
    template <CopyConstructible T>
      requires
        ((_Is<T, is_empty> && !_Is<T, is_final>) ||
         (sizeof(T) <= cheap_copy_size &&
          ext::TriviallyCopyConstructible<T>()))
    constexpr bool cheaply_copyable<T> = true;

    template <ext::Addressable T>
    class ref_box {
    public:
      ref_box() = default;
      constexpr ref_box(T& t) noexcept :
        ptr_{&t} {}
      ref_box(T&&) = delete;

      constexpr T& get() const {
        return *ptr_;
      }

    private:
      raw_ptr<T> ptr_;
    };

    // Note: promotes to CopyConstructible
    template <ext::Addressable T>
    using cheap_reference_box_t = meta::if_c<
      cheaply_copyable<remove_cv_t<T>>,
      ebo_box<remove_cv_t<T>>,
      ref_box<T>>;
  }

  namespace ext {
    template <class T, class U>
    class compressed_pair :
      detail::ebo_box<T, meta::size_t<0>>,
      detail::ebo_box<U, meta::size_t<1>> {
      using first_t = detail::ebo_box<T, meta::size_t<0>>;
      using second_t = detail::ebo_box<U, meta::size_t<1>>;
    public:
      compressed_pair()
        requires DefaultConstructible<T>() &&
          DefaultConstructible<U>() = default;

      template <typename TT = T, typename UU = U>
        requires Constructible<T, TT>() && Constructible<U, UU>()
      constexpr compressed_pair(TT&& t, UU&& u) :
        first_t(__stl2::forward<TT>(t)),
        second_t(__stl2::forward<UU>(u)) {}

      constexpr T& first() & noexcept { return first_t::get(); }
      constexpr const T& first() const& noexcept { return first_t::get(); }
      constexpr T&& first() && noexcept { return first_t::get(); }

      constexpr U& second() & noexcept { return second_t::get(); }
      constexpr const U& second() const& noexcept { return second_t::get(); }
      constexpr U&& second() && noexcept { return second_t::get(); }
    };
  }
} STL2_CLOSE_NAMESPACE

#endif
