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
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
  WeaklyIncrementable{I}
  class iota_view {
    I first_;
  public:
    class iterator {
      I value_;
    public:
      using value_type = I;
      using difference_type = DifferenceType<I>;
      using iterator_category =
        meta::if_c<
          models::Incrementable<I>,
          meta::if_c<
            models::Decrementable<I>,
            meta::if_c<
              models::RandomAccessIncrementable<I>,
              random_access_iterator_tag,
              bidirectional_iterator_tag>,
            forward_iterator_tag>,
          weak_input_iterator_tag>;

      iterator() = default;
      constexpr iterator(const iota_view& v) noexcept :
        value_{v.first_} {}

      constexpr I operator*() const noexcept {
        return value_;
      }

      constexpr iterator& operator++() & noexcept {
        ++value_;
        return *this;
      }
      constexpr iterator operator++(int) & noexcept {
        auto tmp = *this;
        ++*this;
        return tmp;
      }

      constexpr iterator& operator--() & noexcept
        requires ext::Decrementable<I>() {
        --value_;
        return *this;
      }
      constexpr iterator operator--(int) & noexcept
        requires ext::Decrementable<I>() {
        auto tmp = *this;
        --*this;
        return tmp;
      }

      constexpr I operator[](difference_type n) const noexcept
        requires ext::RandomAccessIncrementable<I>() {
        return value_ + n;
      }

      constexpr iterator& operator+=(difference_type n) & noexcept
        requires ext::RandomAccessIncrementable<I>() {
        value_ += n;
        return *this;
      }
      constexpr iterator& operator-=(difference_type n) & noexcept
        requires ext::RandomAccessIncrementable<I>() {
        value_ -= n;
        return *this;
      }

      constexpr iterator operator+(difference_type n) const noexcept
        requires ext::RandomAccessIncrementable<I>() {
        return {value_ + n};
      }
      constexpr iterator operator-(difference_type n) const noexcept
        requires ext::RandomAccessIncrementable<I>() {
        return {value_ - n};
      }
      friend constexpr iterator
      operator+(difference_type n, const iterator& i) noexcept
        requires ext::RandomAccessIncrementable<I>() {
        return i + n;
      }

      friend constexpr difference_type
      operator-(const iterator& lhs, const iterator& rhs) noexcept
        requires ext::RandomAccessIncrementable<I>() {
        return lhs.value_ - rhs.value_;
      }

      friend constexpr bool
      operator==(const iterator& lhs, const iterator& rhs) noexcept
        requires EqualityComparable<I>() {
        return lhs.value_ == rhs.value_;
      }
      friend constexpr bool
      operator!=(const iterator& lhs, const iterator& rhs) noexcept
        requires EqualityComparable<I>() {
        return lhs.value_ != rhs.value_;
      }

      friend constexpr bool
      operator<(const iterator& lhs, const iterator& rhs) noexcept
        requires TotallyOrdered<I>() {
        return lhs.value_ < rhs.value_;
      }
      friend constexpr bool
      operator>(const iterator& lhs, const iterator& rhs) noexcept
        requires TotallyOrdered<I>() {
        return lhs.value_ > rhs.value_;
      }
      friend constexpr bool
      operator<=(const iterator& lhs, const iterator& rhs) noexcept
        requires TotallyOrdered<I>() {
        return lhs.value_ <= rhs.value_;
      }
      friend constexpr bool
      operator>=(const iterator& lhs, const iterator& rhs) noexcept
        requires TotallyOrdered<I>() {
        return lhs.value_ >= rhs.value_;
      }
    };

    constexpr iota_view(I first = 0) noexcept : first_{first} {}

    constexpr iterator begin() const noexcept { return {*this}; }
    constexpr unreachable end() const noexcept { return {}; }
  };
} STL2_CLOSE_NAMESPACE

#endif
