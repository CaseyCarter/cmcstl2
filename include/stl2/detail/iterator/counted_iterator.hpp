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
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>
#include <stl2/detail/iterator/operations.hpp>

STL2_OPEN_NAMESPACE {
  // Not to spec: No protected data members.
  WeakIterator{I}
  class counted_iterator : detail::ebo_box<I> {
    friend struct __counted_iterator_access;
    using box_t = detail::ebo_box<I>;
    difference_type_t<I> count_;

    constexpr I& current() & noexcept { return box_t::get(); }
    constexpr const I& current() const& noexcept { return box_t::get(); }
    constexpr I&& current() && noexcept { return box_t::get(); }

  public:
    using iterator_type = I;
    using difference_type = difference_type_t<I>;
    using reference = reference_t<I>;

    STL2_CONSTEXPR_EXT counted_iterator()
      noexcept(is_nothrow_default_constructible<I>::value) :
      box_t{}, count_{0} {}

    // Extension
    STL2_CONSTEXPR_EXT counted_iterator(default_sentinel)
      noexcept(is_nothrow_default_constructible<I>::value) :
      counted_iterator() {}
    STL2_CONSTEXPR_EXT counted_iterator(default_sentinel)
      noexcept(is_nothrow_default_constructible<I>::value)
      requires BidirectionalIterator<I>()  = delete;

    STL2_CONSTEXPR_EXT counted_iterator(I x, difference_type_t<I> n)
      noexcept(is_nothrow_move_constructible<I>::value) :
      box_t{__stl2::move(x)}, count_{n} {
        STL2_ASSUME(0 <= n);
      }
    template <ConvertibleTo<I> U>
    STL2_CONSTEXPR_EXT counted_iterator(const counted_iterator<U>& u)
      noexcept(is_nothrow_constructible<I, const U&>::value);

    template <ConvertibleTo<I> U>
    STL2_CONSTEXPR_EXT counted_iterator& operator=(const counted_iterator<U>& u) &
      noexcept(is_nothrow_assignable<I&, const U&>::value);

    STL2_CONSTEXPR_EXT I base() const
      noexcept(is_nothrow_copy_constructible<I>::value) {
      return current();
    }
    STL2_CONSTEXPR_EXT difference_type_t<I> count() const noexcept {
      return count_;
    }

    STL2_CONSTEXPR_EXT decltype(auto) operator*() const
      noexcept(noexcept(*declval<const I&>()))
      // Extension: allow mutable-only iterators
      requires detail::Dereferenceable<const I> {
      return *current();
    }
    // Extension to support output iterators, since Writable
    // allows * to be modifying.
    STL2_CONSTEXPR_EXT decltype(auto) operator*()
      noexcept(noexcept(*declval<I&>())) {
      return *current();
    }

    STL2_CONSTEXPR_EXT counted_iterator& operator++() &
      noexcept(noexcept(++declval<I&>())) {
      ++current();
      STL2_ASSUME_CONSTEXPR(0 < count_);
      --count_;
      return *this;
    }
    STL2_CONSTEXPR_EXT counted_iterator operator++(int) &
      noexcept(is_nothrow_copy_constructible<counted_iterator>::value &&
               noexcept(++declval<I&>()))
    {
      auto tmp = *this;
      ++*this;
      return tmp;
    }
    STL2_CONSTEXPR_EXT counted_iterator& operator--() &
      noexcept(noexcept(--declval<I&>()))
      requires BidirectionalIterator<I>()
    {
      --current();
      ++count_;
      return *this;
    }
    STL2_CONSTEXPR_EXT counted_iterator operator--(int) &
      noexcept(is_nothrow_copy_constructible<counted_iterator>::value &&
               noexcept(--declval<I&>()))
      requires BidirectionalIterator<I>()
    {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    STL2_CONSTEXPR_EXT counted_iterator operator+(difference_type_t<I> n) const
      requires RandomAccessIterator<I>()
    {
      STL2_ASSUME(n <= count_);
      return {current() + n, count_ - n};
    }
    STL2_CONSTEXPR_EXT counted_iterator& operator+=(difference_type_t<I> n) &
      noexcept(noexcept(declval<I&>() += n))
      requires RandomAccessIterator<I>()
    {
      STL2_ASSUME(n <= count_);
      current() += n;
      count_ -= n;
      return *this;
    }
    STL2_CONSTEXPR_EXT counted_iterator operator-(difference_type_t<I> n) const
      requires RandomAccessIterator<I>()
    {
      return *this + -n;
    }
    STL2_CONSTEXPR_EXT counted_iterator& operator-=(difference_type_t<I> n) &
      noexcept(noexcept(declval<I&>() += n))
      requires RandomAccessIterator<I>()
    {
      return (*this += -n);
    }

    STL2_CONSTEXPR_EXT decltype(auto) operator[](difference_type_t<I> n) const
      noexcept(noexcept(declval<const I&>()[n]))
      requires RandomAccessIterator<I>()
    {
      STL2_ASSUME(n < count_);
      return current()[n];
    }
  };

  struct __counted_iterator_access {
    static constexpr decltype(auto) base(auto&& ci) noexcept {
      return (__stl2::forward<decltype(ci)>(ci).current());
    }
    static constexpr decltype(auto) count(auto&& ci) noexcept {
      return (__stl2::forward<decltype(ci)>(ci).count_);
    }
  };

  WeakIterator{I}
  template <ConvertibleTo<I> U>
  STL2_CONSTEXPR_EXT counted_iterator<I>::counted_iterator(const counted_iterator<U>& u)
    noexcept(is_nothrow_constructible<I, const U&>::value) :
    box_t{__counted_iterator_access::base(u)},
    count_{__counted_iterator_access::count(u)} {}

  WeakIterator{I}
  template <ConvertibleTo<I> U>
  STL2_CONSTEXPR_EXT counted_iterator<I>&
    counted_iterator<I>::operator=(const counted_iterator<U>& u) &
      noexcept(is_nothrow_assignable<I&, const U&>::value)
    {
      current() = __counted_iterator_access::base(u);
      count_ = __counted_iterator_access::count(u);
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
  STL2_CONSTEXPR_EXT bool operator==(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return x.count() == y.count();
    }
  STL2_CONSTEXPR_EXT bool operator==(
    const counted_iterator<auto>& x, default_sentinel) noexcept {
      return x.count() == 0;
    }
  STL2_CONSTEXPR_EXT bool operator==(
    default_sentinel x, const counted_iterator<auto>& y) noexcept {
      return y == x;
    }

  Common{I1, I2}
  STL2_CONSTEXPR_EXT bool operator!=(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return !(x == y);
    }
  STL2_CONSTEXPR_EXT bool operator!=(
    const counted_iterator<auto>& x, default_sentinel y) noexcept {
      return !(x == y);
    }
  STL2_CONSTEXPR_EXT bool operator!=(
    default_sentinel x, const counted_iterator<auto>& y) noexcept {
      return !(x == y);
    }

  Common{I1, I2}
  STL2_CONSTEXPR_EXT bool operator<(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return x.count() > y.count();
    }
  STL2_CONSTEXPR_EXT bool operator<(
    const counted_iterator<auto>& x, default_sentinel) noexcept {
      return x.count() != 0;
    }
  STL2_CONSTEXPR_EXT bool operator<(
    default_sentinel, const counted_iterator<auto>&) noexcept {
      return false;
    }

  Common{I1, I2}
  STL2_CONSTEXPR_EXT bool operator<=(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return !(y < x);
    }
  STL2_CONSTEXPR_EXT bool operator<=(
    const counted_iterator<auto>& x, default_sentinel y) noexcept {
      return !(y < x);
    }
  STL2_CONSTEXPR_EXT bool operator<=(
    default_sentinel x, const counted_iterator<auto>& y) noexcept {
      return !(y < x);
    }

  Common{I1, I2}
  STL2_CONSTEXPR_EXT bool operator>(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return y < x;
    }
  STL2_CONSTEXPR_EXT bool operator>(
    const counted_iterator<auto>& x, default_sentinel y) noexcept {
      return y < x;
    }
  STL2_CONSTEXPR_EXT bool operator>(
    default_sentinel x, const counted_iterator<auto>& y) noexcept {
      return y < x;
    }

  Common{I1, I2}
  STL2_CONSTEXPR_EXT bool operator>=(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return !(x < y);
    }
  STL2_CONSTEXPR_EXT bool operator>=(
    const counted_iterator<auto>& x, default_sentinel y) noexcept {
      return !(x < y);
    }
  STL2_CONSTEXPR_EXT bool operator>=(
    default_sentinel x, const counted_iterator<auto>& y) noexcept {
      return !(x < y);
    }

  Common{I1, I2}
  STL2_CONSTEXPR_EXT difference_type_t<I2> operator-(
    const counted_iterator<I1>& x, const counted_iterator<I2>& y) noexcept {
      return y.count() - x.count();
    }
  STL2_CONSTEXPR_EXT auto operator-(
    const counted_iterator<auto>& x, default_sentinel) noexcept {
      return -x.count();
    }
  STL2_CONSTEXPR_EXT auto operator-(
    default_sentinel, const counted_iterator<auto>& y) noexcept {
      return y.count();
    }

  RandomAccessIterator{I}
  STL2_CONSTEXPR_EXT auto operator+(
    difference_type_t<I> n, const counted_iterator<I>& x)
      STL2_NOEXCEPT_RETURN(x + n)

  WeakIterator{I}
  STL2_CONSTEXPR_EXT counted_iterator<I>
  make_counted_iterator(I i, difference_type_t<I> n)
    noexcept(is_nothrow_constructible<
               counted_iterator<I>, I, difference_type_t<I>>::value) {
    return {i, n};
  }

  WeakIterator{I}
  STL2_CONSTEXPR_EXT void advance(counted_iterator<I>& i, difference_type_t<I> n)
    noexcept(noexcept(__stl2::advance(declval<I&>(), n))) {
      STL2_ASSUME(n <= i.count());
      __stl2::advance(__counted_iterator_access::base(i), n);
      __counted_iterator_access::count(i) -= n;
    }

  RandomAccessIterator{I}
  STL2_CONSTEXPR_EXT void advance(counted_iterator<I>& i, difference_type_t<I> n)
  STL2_NOEXCEPT_RETURN(void(i += n))

  namespace ext {
    constexpr auto uncounted(const WeakIterator& i) {
      return i;
    }

    STL2_CONSTEXPR_EXT auto uncounted(const counted_iterator<auto>& i) {
      return i.base();
    }

    WeakIterator{I}
    constexpr auto recounted(const I&, const I& i, difference_type_t<I> = 0) {
      return i;
    }

    WeakIterator{I}
    STL2_CONSTEXPR_EXT auto recounted(const counted_iterator<I>& o, I i) {
      return counted_iterator<I>{__stl2::move(i), o.count()};
    }

    WeakIterator{I}
    STL2_CONSTEXPR_EXT auto recounted(const counted_iterator<I>& o, I i, difference_type_t<I> n) {
      STL2_EXPENSIVE_ASSERT(!models::ForwardIterator<I> ||
                            i == __stl2::next(o.base(), n));
      return counted_iterator<I>{__stl2::move(i), o.count() - n};
    }
  }
} STL2_CLOSE_NAMESPACE

#endif
