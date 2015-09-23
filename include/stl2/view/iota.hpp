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
#ifndef STL2_DETAIL_VIEW_IOTA_HPP
#define STL2_DETAIL_VIEW_IOTA_HPP

#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/fundamental.hpp>

STL2_OPEN_NAMESPACE {
  Integral{I}
  class iota_view {
    I first_;
  public:
    class iterator {
      I value_;
    public:
      using value_type = I;
      using difference_type = make_signed_t<I>;
      using iterator_category = random_access_iterator_tag;

      iterator() = default;
      constexpr iterator(const iota_view& v) noexcept :
        value_{v.first_} {}

      constexpr I operator*() const noexcept {
        return value_;
      }

      constexpr iterator& operator++() & noexcept {
        ++value_; return *this;
      }
      constexpr iterator& operator--() & noexcept {
        --value_; return *this;
      }

      constexpr iterator operator++(int) & noexcept {
        auto tmp = *this;
        ++*this;
        return tmp;
      }
      constexpr iterator operator--(int) & noexcept {
        auto tmp = *this;
        --*this;
        return tmp;
      }

      constexpr I operator[](difference_type n) const noexcept {
        return value_ + n;
      }

      constexpr iterator& operator+=(difference_type n) & noexcept {
        value_ += n;
        return *this;
      }
      constexpr iterator& operator-=(difference_type n) & noexcept {
        value_ -= n;
        return *this;
      }

      constexpr iterator operator+(difference_type n) const noexcept {
        return {value_ + n};
      }
      constexpr iterator operator-(difference_type n) const noexcept {
        return {value_ - n};
      }
      friend constexpr iterator
      operator+(difference_type n, const iterator& i) noexcept {
        return i + n;
      }

      friend constexpr difference_type
      operator-(const iterator& lhs, const iterator& rhs) noexcept {
        return lhs.value_ - rhs.value_;
      }

      friend constexpr bool
      operator==(const iterator& lhs, const iterator& rhs) noexcept {
        return lhs.value_ == rhs.value_;
      }
      friend constexpr bool
      operator!=(const iterator& lhs, const iterator& rhs) noexcept {
        return lhs.value_ != rhs.value_;
      }
      friend constexpr bool
      operator<(const iterator& lhs, const iterator& rhs) noexcept {
        return lhs.value_ < rhs.value_;
      }
      friend constexpr bool
      operator>(const iterator& lhs, const iterator& rhs) noexcept {
        return lhs.value_ > rhs.value_;
      }
      friend constexpr bool
      operator<=(const iterator& lhs, const iterator& rhs) noexcept {
        return lhs.value_ <= rhs.value_;
      }
      friend constexpr bool
      operator>=(const iterator& lhs, const iterator& rhs) noexcept {
        return lhs.value_ >= rhs.value_;
      }
    };

    constexpr iota_view(I first = 0) noexcept : first_{first} {}

    constexpr iterator begin() const noexcept { return {*this}; }
    constexpr unreachable end() const noexcept { return {}; }
  };
} STL2_CLOSE_NAMESPACE

#endif
