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
#ifndef STL2_DETAIL_VIEW_REPEAT_HPP
#define STL2_DETAIL_VIEW_REPEAT_HPP

#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/cheap_storage.hpp>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
  template <Semiregular T>
  class repeat_view : view_base, detail::ebo_box<T> {
    using storage_t = detail::ebo_box<T>;
  public:
    class iterator : detail::cheap_reference_box_t<const T> {
      using storage_t = detail::cheap_reference_box_t<const T>;
    public:
      using value_type = T;
      using iterator_category = random_access_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using reference = meta::if_c<
        detail::cheaply_copyable<T>, value_type, const value_type&>;

      iterator() = default;
      constexpr iterator(const repeat_view& r)
        noexcept(is_nothrow_constructible<storage_t, const T&>::value) :
        storage_t{r.value()} {}

      constexpr reference operator*() const
        noexcept(noexcept(declval<const storage_t&>().get())) {
        return storage_t::get();
      }
      constexpr reference operator[](difference_type) const
        noexcept(noexcept(declval<const storage_t&>().get())) {
        return storage_t::get();
      }

      constexpr iterator& operator++() noexcept {
        return *this;
      }
      constexpr iterator operator++(int)
        noexcept(is_nothrow_copy_constructible<iterator>::value) {
        return *this;
      }

      constexpr iterator& operator--() noexcept {
        return *this;
      }
      constexpr iterator operator--(int)
        noexcept(is_nothrow_copy_constructible<iterator>::value) {
        return *this;
      }

      constexpr iterator& operator+=(difference_type) noexcept {
        return *this;
      }
      constexpr iterator& operator-=(difference_type) noexcept {
        return *this;
      }

      friend constexpr bool operator==(const iterator&, const iterator&) noexcept {
        return true;
      }
      friend constexpr bool operator!=(const iterator&, const iterator&) noexcept {
        return false;
      }

      friend constexpr iterator operator+(const iterator& i, difference_type)
        noexcept(is_nothrow_copy_constructible<iterator>::value) {
        return i;
      }
      friend constexpr iterator operator+(difference_type, const iterator& i)
        noexcept(is_nothrow_copy_constructible<iterator>::value) {
        return i;
      }

      friend constexpr iterator operator-(const iterator& i, difference_type)
        noexcept(is_nothrow_copy_constructible<iterator>::value) {
        return i;
      }
    };

    repeat_view() = default;
    repeat_view(T value)
      noexcept(is_nothrow_constructible<storage_t, T>::value) :
      storage_t{__stl2::move(value)} {}

    constexpr iterator begin() const
      noexcept(is_nothrow_constructible<iterator, const repeat_view&>::value) {
      return {*this};
    }
    constexpr unreachable end() const noexcept {
      return {};
    }

    constexpr const T& value() const noexcept {
      return storage_t::get();
    }
  };
} STL2_CLOSE_NAMESPACE

#endif
