#ifndef STL2_DETAIL_EBO_STORAGE_HPP
#define STL2_DETAIL_EBO_STORAGE_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
  namespace detail {
    template <class T, class Tag = void>
    class ebo_storage {
      T item_;
    public:
      ebo_storage() requires DefaultConstructible<T>() = default;
      constexpr ebo_storage(const T& t)
        noexcept(is_nothrow_copy_constructible<T>::value)
        requires CopyConstructible<T>() : item_(t) {}
      constexpr ebo_storage(T&& t)
        noexcept(is_nothrow_move_constructible<T>::value)
        requires MoveConstructible<T>() : item_(__stl2::move(t)) {}

      template <typename U>
        requires !Same<ebo_storage, decay_t<U>>() &&
          Constructible<T, U>()
      constexpr ebo_storage(U&& u)
        noexcept(is_nothrow_constructible<T, U>::value) :
        item_(__stl2::forward<U>(u)) {}

      constexpr T& get() & noexcept { return item_; }
      constexpr const T& get() const& noexcept { return item_; }
      constexpr T&& get() && noexcept { return __stl2::move(item_); }
    };

    template <class T, class Tag>
      requires _Is<T, is_empty> && _IsNot<T, is_final>
    class ebo_storage<T, Tag> : private T {
    public:
      using T::T;

      ebo_storage() requires DefaultConstructible<T>() = default;
      constexpr ebo_storage(const T& t)
        noexcept(is_nothrow_copy_constructible<T>::value)
        requires CopyConstructible<T>() : T(t) {}
      constexpr ebo_storage(T&& t)
        noexcept(is_nothrow_move_constructible<T>::value)
        requires MoveConstructible<T>() : T(__stl2::move(t)) {}

      constexpr T& get() & noexcept { return *this; }
      constexpr const T& get() const& noexcept { return *this; }
      constexpr T&& get() && noexcept { return __stl2::move(*this); }
    };
  }

  namespace ext {
    template <class T, class U>
    class compressed_pair :
      detail::ebo_storage<T, meta::size_t<0>>,
      detail::ebo_storage<U, meta::size_t<1>> {
      using first_t = detail::ebo_storage<T, meta::size_t<0>>;
      using second_t = detail::ebo_storage<U, meta::size_t<1>>;
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
