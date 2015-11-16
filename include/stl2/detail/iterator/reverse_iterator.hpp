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
#ifndef STL2_DETAIL_ITERATOR_REVERSE_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_REVERSE_ITERATOR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/operations.hpp>

///////////////////////////////////////////////////////////////////////////
// reverse_iterator [reverse.iterator]
//
STL2_OPEN_NAMESPACE {
  struct __ri_access {
    template <class RI>
    static constexpr auto& current(RI&& ri) noexcept {
      return __stl2::forward<RI>(ri).current_;
    }
  };

  BidirectionalIterator{I}
  class reverse_iterator {
    friend __ri_access;
    I current_{};
  public:
    using iterator_type = I;
    using difference_type = difference_type_t<I>;
    using value_type = value_type_t<I>;
    using iterator_category = iterator_category_t<I>;
    using reference = reference_t<I>;
    using pointer = I;

    reverse_iterator() = default;
    STL2_CONSTEXPR_EXT explicit reverse_iterator(I x)
    noexcept(is_nothrow_move_constructible<I>::value)
    : current_{__stl2::move(x)}
    {}

    template <ConvertibleTo<I> U>
    STL2_CONSTEXPR_EXT reverse_iterator(const reverse_iterator<U>& u)
    noexcept(is_nothrow_constructible<I, const U&>::value)
    : current_{__ri_access::current(u)}
    {}

    template <ConvertibleTo<I> U>
    STL2_CONSTEXPR_EXT reverse_iterator& operator=(const reverse_iterator<U>& u) &
    noexcept(is_nothrow_assignable<I&, const U&>::value)
    {
      current_ = __ri_access::current(u);
      return *this;
    }

    STL2_CONSTEXPR_EXT I base() const
    noexcept(is_nothrow_copy_constructible<I>::value)
    {
      return current_;
    }

    STL2_CONSTEXPR_EXT reference operator*() const
    STL2_NOEXCEPT_RETURN(
      *__stl2::prev(current_)
    )

    STL2_CONSTEXPR_EXT pointer operator->() const
    STL2_NOEXCEPT_RETURN(
      __stl2::prev(current_)
    )

    // 20150813: Extension.
    friend STL2_CONSTEXPR_EXT decltype(auto) iter_move(reverse_iterator i)
    STL2_NOEXCEPT_RETURN(
      __stl2::iter_move(__stl2::prev(i.current_))
    )

    STL2_CONSTEXPR_EXT reverse_iterator& operator++() &
    noexcept(noexcept(--current_))
    {
      --current_;
      return *this;
    }
    STL2_CONSTEXPR_EXT reverse_iterator operator++(int) &
    noexcept(is_nothrow_copy_constructible<I>::value &&
             is_nothrow_move_constructible<I>::value &&
             noexcept(++declval<reverse_iterator&>()))
    {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    STL2_CONSTEXPR_EXT reverse_iterator& operator--() &
    noexcept(noexcept(++current_))
    {
      ++current_;
      return *this;
    }
    STL2_CONSTEXPR_EXT reverse_iterator operator--(int) &
    noexcept(is_nothrow_copy_constructible<I>::value &&
             is_nothrow_move_constructible<I>::value &&
             noexcept(--declval<reverse_iterator&>()))
    {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    STL2_CONSTEXPR_EXT reverse_iterator operator+(difference_type n) const
    noexcept(is_nothrow_move_constructible<I>::value &&
             noexcept(reverse_iterator{current_ - n}))
    requires RandomAccessIterator<I>()
    {
      return reverse_iterator{current_ - n};
    }
    STL2_CONSTEXPR_EXT reverse_iterator& operator+=(difference_type n) &
    noexcept(noexcept(current_ -= n))
    requires RandomAccessIterator<I>()
    {
      current_ -= n;
      return *this;
    }
    STL2_CONSTEXPR_EXT reverse_iterator operator-(difference_type n) const
    noexcept(is_nothrow_move_constructible<I>::value &&
             noexcept(reverse_iterator{current_ + n}))
    requires RandomAccessIterator<I>()
    {
      return reverse_iterator{current_ + n};
    }
    STL2_CONSTEXPR_EXT reverse_iterator& operator-=(difference_type n) &
    noexcept(noexcept(current_ += n))
    requires RandomAccessIterator<I>()
    {
      current_ += n;
      return *this;
    }

    STL2_CONSTEXPR_EXT decltype(auto) operator[](difference_type n) const
    noexcept(noexcept(current_[-n - 1]))
    requires RandomAccessIterator<I>()
    {
      return current_[-n - 1];
    }
  };

  EqualityComparable{I1, I2}
  STL2_CONSTEXPR_EXT bool operator==(
    const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
  STL2_NOEXCEPT_RETURN(
    __ri_access::current(x) == __ri_access::current(y)
  )

  EqualityComparable{I1, I2}
  STL2_CONSTEXPR_EXT bool operator!=(
    const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
  STL2_NOEXCEPT_RETURN(
    !(x == y)
  )

  StrictTotallyOrdered{I1, I2}
  STL2_CONSTEXPR_EXT bool operator<(
    const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
  STL2_NOEXCEPT_RETURN(
    __ri_access::current(x) > __ri_access::current(y)
  )

  StrictTotallyOrdered{I1, I2}
  STL2_CONSTEXPR_EXT bool operator>(
    const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
  STL2_NOEXCEPT_RETURN(
    y < x
  )

  StrictTotallyOrdered{I1, I2}
  STL2_CONSTEXPR_EXT bool operator<=(
    const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
  STL2_NOEXCEPT_RETURN(
    !(y < x)
  )

  StrictTotallyOrdered{I1, I2}
  STL2_CONSTEXPR_EXT bool operator>=(
    const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
  STL2_NOEXCEPT_RETURN(
    !(x < y)
  )

  SizedSentinel{I1, I2}
  STL2_CONSTEXPR_EXT difference_type_t<I2> operator-(
    const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
  STL2_NOEXCEPT_RETURN(
    __ri_access::current(y) - __ri_access::current(x)
  )

  RandomAccessIterator{I}
  STL2_CONSTEXPR_EXT reverse_iterator<I> operator+(
    difference_type_t<I> n, const reverse_iterator<I>& x)
  STL2_NOEXCEPT_RETURN(
    x + n
  )

  template <class I>
  requires BidirectionalIterator<__f<I>>()
  STL2_CONSTEXPR_EXT auto make_reverse_iterator(I&& i)
  STL2_NOEXCEPT_RETURN(
    reverse_iterator<__f<I>>{__stl2::forward<I>(i)}
  )
} STL2_CLOSE_NAMESPACE

#endif
