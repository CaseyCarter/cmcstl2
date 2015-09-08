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
#ifndef STL2_DETAIL_ITERATOR_COUNTED_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_COUNTED_ITERATOR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>
#include <stl2/detail/iterator/operations.hpp>

STL2_OPEN_NAMESPACE {
  WeakIterator{I}
  class counted_iterator {
  public:
    using iterator_type = I;
    using difference_type = DifferenceType<I>;
    using reference = ReferenceType<I>;

    constexpr counted_iterator()
      noexcept(is_nothrow_default_constructible<I>::value) :
      current{}, cnt{0} {}
    constexpr counted_iterator(I x, DifferenceType<I> n)
      noexcept(is_nothrow_move_constructible<I>::value) :
      current{__stl2::move(x)}, cnt{n} {
        STL2_ASSERT(0 <= n);
      }
    template <ConvertibleTo<I> U>
    constexpr counted_iterator(const counted_iterator<U>& u)
      noexcept(is_nothrow_constructible<I, const U&>::value);

    template <ConvertibleTo<I> U>
    constexpr counted_iterator& operator=(const counted_iterator<U>& u) &
      noexcept(is_nothrow_assignable<I&, const U&>::value);

    constexpr I base() const
      noexcept(is_nothrow_copy_constructible<I>::value) {
      return current;
    }
    constexpr DifferenceType<I> count() const noexcept {
      return cnt;
    }

    constexpr decltype(auto) operator*() const
      noexcept(noexcept(*declval<const I&>())) {
      return *current;
    }

    constexpr counted_iterator& operator++() &
      noexcept(noexcept(++declval<I&>())) {
      ++current;
      STL2_ASSERT(0 < cnt);
      --cnt;
      return *this;
    }
    constexpr counted_iterator operator++(int) &
      noexcept(is_nothrow_copy_constructible<counted_iterator>::value &&
               noexcept(++declval<I&>()))
    {
      auto tmp = *this;
      ++*this;
      return tmp;
    }
    constexpr counted_iterator& operator--() &
      noexcept(noexcept(--declval<I&>()))
      requires BidirectionalIterator<I>()
    {
      --current;
      ++cnt;
      return *this;
    }
    constexpr counted_iterator operator--(int) &
      noexcept(is_nothrow_copy_constructible<counted_iterator>::value &&
               noexcept(--declval<I&>()))
      requires BidirectionalIterator<I>()
    {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    constexpr counted_iterator operator+(DifferenceType<I> n) const
      requires RandomAccessIterator<I>()
    {
      STL2_ASSERT(n <= cnt);
      return {current + n, cnt - n};
    }
    constexpr counted_iterator& operator+=(DifferenceType<I> n) &
      noexcept(noexcept(declval<I&>() += n))
      requires RandomAccessIterator<I>()
    {
      STL2_ASSERT(n <= cnt);
      current += n;
      cnt -= n;
      return *this;
    }
    constexpr counted_iterator operator-(DifferenceType<I> n) const
      requires RandomAccessIterator<I>()
    {
      return *this + -n;
    }
    constexpr counted_iterator& operator-=(DifferenceType<I> n) &
      noexcept(noexcept(declval<I&>() += n))
      requires RandomAccessIterator<I>()
    {
      return (*this += -n);
    }

    constexpr decltype(auto) operator[](DifferenceType<I> n) const
      noexcept(noexcept(declval<const I&>()[n]))
      requires RandomAccessIterator<I>()
    {
      STL2_ASSERT(n < cnt);
      return current[n];
    }

  protected:
    I current;
    DifferenceType<I> cnt;

  private:
    friend struct __counted_iterator_access;
  };

  struct __counted_iterator_access {
    static constexpr decltype(auto) base(auto&& ci) noexcept {
      return (__stl2::forward<decltype(ci)>(ci).current);
    }
    static constexpr decltype(auto) count(auto&& ci) noexcept {
      return (__stl2::forward<decltype(ci)>(ci).cnt);
    }
  };

  WeakIterator{I}
  template <ConvertibleTo<I> U>
  constexpr counted_iterator<I>::counted_iterator(const counted_iterator<U>& u)
    noexcept(is_nothrow_constructible<I, const U&>::value) :
    current{__counted_iterator_access::base(u)},
    cnt{__counted_iterator_access::count(u)} {}

  WeakIterator{I}
  template <ConvertibleTo<I> U>
  constexpr counted_iterator<I>&
    counted_iterator<I>::operator=(const counted_iterator<U>& u) &
      noexcept(is_nothrow_assignable<I&, const U&>::value)
    {
      current = __counted_iterator_access::base(u);
      cnt = __counted_iterator_access::count(u);
      return *this;
    }

  WeakInputIterator{I}
  struct value_type<counted_iterator<I>> :
    value_type<I> {};

  WeakInputIterator{I}
  struct iterator_category<counted_iterator<I>> {
    using type = input_iterator_tag;
  };

  ForwardIterator{I}
  struct iterator_category<counted_iterator<I>> :
    iterator_category<I> { };

  Common{I1, I2}
  constexpr bool operator==(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return x.count() == y.count();
    }
  constexpr bool operator==(
    const counted_iterator<auto>& x, default_sentinel) noexcept {
      return x.count() == 0;
    }
  constexpr bool operator==(
    default_sentinel x, const counted_iterator<auto>& y) noexcept {
      return y == x;
    }

  Common{I1, I2}
  constexpr bool operator!=(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return !(x == y);
    }
  constexpr bool operator!=(
    const counted_iterator<auto>& x, default_sentinel y) noexcept {
      return !(x == y);
    }
  constexpr bool operator!=(
    default_sentinel x, const counted_iterator<auto>& y) noexcept {
      return !(x == y);
    }

  Common{I1, I2}
  constexpr bool operator<(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return x.count() > y.count();
    }
  constexpr bool operator<(
    const counted_iterator<auto>& x, default_sentinel) noexcept {
      return x.count() != 0;
    }
  constexpr bool operator<(
    default_sentinel, const counted_iterator<auto>&) noexcept {
      return false;
    }

  Common{I1, I2}
  constexpr bool operator<=(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return !(y < x);
    }
  constexpr bool operator<=(
    const counted_iterator<auto>& x, default_sentinel y) noexcept {
      return !(y < x);
    }
  constexpr bool operator<=(
    default_sentinel x, const counted_iterator<auto>& y) noexcept {
      return !(y < x);
    }

  Common{I1, I2}
  constexpr bool operator>(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return y < x;
    }
  constexpr bool operator>(
    const counted_iterator<auto>& x, default_sentinel y) noexcept {
      return y < x;
    }
  constexpr bool operator>(
    default_sentinel x, const counted_iterator<auto>& y) noexcept {
      return y < x;
    }

  Common{I1, I2}
  constexpr bool operator>=(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return !(x < y);
    }
  constexpr bool operator>=(
    const counted_iterator<auto>& x, default_sentinel y) noexcept {
      return !(x < y);
    }
  constexpr bool operator>=(
    default_sentinel x, const counted_iterator<auto>& y) noexcept {
      return !(x < y);
    }

  Common{I1, I2}
  constexpr DifferenceType<I2> operator-(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return y.count() - x.count();
    }
  constexpr auto operator-(
    const counted_iterator<auto>& x, default_sentinel) noexcept {
      return -x.count();
    }
  constexpr auto operator-(
    default_sentinel, const counted_iterator<auto>& y) noexcept {
      return y.count();
    }

  RandomAccessIterator{I}
  constexpr auto operator+(
    DifferenceType<I> n, const counted_iterator<I>& x)
      STL2_NOEXCEPT_RETURN(x + n)

  WeakIterator{I}
  constexpr counted_iterator<I>
    make_counted_iterator(I i, DifferenceType<I> n)
      noexcept(is_nothrow_constructible<
               counted_iterator<I>, I, DifferenceType<I>>::value) {
    return {i, n};
  }

  WeakIterator{I}
  constexpr void advance(counted_iterator<I>& i, DifferenceType<I> n)
    noexcept(noexcept(__stl2::advance(declval<I&>(), n))) {
      STL2_ASSERT(n <= i.count());
      __stl2::advance(__counted_iterator_access::base(i), n);
      __counted_iterator_access::count(i) -= n;
    }

  RandomAccessIterator{I}
  constexpr void advance(counted_iterator<I>& i, DifferenceType<I> n)
  STL2_NOEXCEPT_RETURN(void(i += n))
} STL2_CLOSE_NAMESPACE

#endif
