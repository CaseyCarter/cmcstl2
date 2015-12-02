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
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
  WeaklyIncrementable{I}
  class iota_view {
    I first_{};

    class cursor {
    public:
      cursor() = default;
      constexpr cursor(const iota_view& v)
        noexcept(is_nothrow_copy_constructible<I>::value) :
        value_{v.first_} {}

      constexpr I current() const
        noexcept(is_nothrow_copy_constructible<I>::value) {
        return value_;
      }

      constexpr bool equal(const cursor& that) const
        noexcept(noexcept(that.value_ == declval<const I&>()))
        requires EqualityComparable<I>() {
        return value_ == that.value_;
      }

      constexpr void next()
        noexcept(noexcept(++declval<I&>())) {
        ++value_;
      }

      constexpr void prev()
        noexcept(noexcept(--declval<I&>()))
        requires ext::Decrementable<I>() {
        --value_;
      }

      constexpr void advance(difference_type_t<I> n)
        noexcept(noexcept(declval<I&>() += n))
        requires ext::RandomAccessIncrementable<I>() {
        value_ += n;
      }

      constexpr difference_type_t<I> distance_to(const cursor& that)
        noexcept(noexcept(that.value_ - declval<const I&>()))
        requires ext::RandomAccessIncrementable<I>() {
        return that.value_ - value_;
      }

    private:
      I value_;
    };

  public:
    iota_view() = default;
    constexpr iota_view(I first)
      noexcept(is_nothrow_move_constructible<I>::value) :
      first_(__stl2::move(first)) {}

    using iterator = basic_iterator<cursor>;
    constexpr iterator begin() const
      noexcept(noexcept(iterator{declval<const iota_view&>()})) {
      return {cursor{*this}};
    }
    constexpr unreachable end() const noexcept { return {}; }
  };
} STL2_CLOSE_NAMESPACE

#endif
