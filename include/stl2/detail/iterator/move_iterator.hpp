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
#ifndef STL2_DETAIL_ITERATOR_MOVE_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_MOVE_ITERATOR_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
  InputIterator{I} class move_iterator;
  Semiregular{S} class move_sentinel;

  struct __mi_access {
    template <_InstanceOf<move_iterator> MI>
    static constexpr auto&& current(MI&& mi) noexcept {
      return std::forward<MI>(mi).current_;
    }
    template <_InstanceOf<move_sentinel> MS>
    static constexpr auto&& sent(MS&& ms) noexcept {
      return std::forward<MS>(ms).get();
    }
  };

  InputIterator{I}
  class move_iterator {
    friend __mi_access;
    I current_{};
  public:
    using iterator_type = I;
    using difference_type = difference_type_t<I>;
    using value_type = value_type_t<I>;
    using iterator_category = input_iterator_tag;
    using reference = rvalue_reference_t<I>;

    move_iterator() = default;

    explicit STL2_CONSTEXPR_EXT move_iterator(I i)
    noexcept(is_nothrow_move_constructible<I>::value)
    : current_{__stl2::move(i)}
    {}

    template <ConvertibleTo<I> U>
    STL2_CONSTEXPR_EXT move_iterator(const move_iterator<U>& u)
    noexcept(is_nothrow_constructible<I, const U&>::value)
    : current_{__mi_access::current(u)}
    {}

    template <ConvertibleTo<I> U>
    STL2_CONSTEXPR_EXT move_iterator&
    operator=(const move_iterator<U>& u) &
    noexcept(is_nothrow_assignable<I&, const U&>::value)
    {
      current_ = __mi_access::current(u);
      return *this;
    }

    STL2_CONSTEXPR_EXT I base() const
    noexcept(is_nothrow_copy_constructible<I>::value)
    {
      return current_;
    }

    STL2_CONSTEXPR_EXT reference operator*() const
    STL2_NOEXCEPT_RETURN(
      __stl2::iter_move(current_)
    )

    STL2_CONSTEXPR_EXT move_iterator& operator++() &
    noexcept(noexcept(++current_))
    {
      ++current_;
      return *this;
    }
    STL2_CONSTEXPR_EXT move_iterator operator++(int) &
    noexcept(is_nothrow_copy_constructible<move_iterator>::value &&
             noexcept(++declval<move_iterator&>()))
    {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    STL2_CONSTEXPR_EXT move_iterator& operator--() &
    noexcept(noexcept(--current_))
    requires BidirectionalIterator<I>()
    {
      --current_;
      return *this;
    }
    STL2_CONSTEXPR_EXT move_iterator operator--(int) &
    noexcept(is_nothrow_copy_constructible<move_iterator>::value &&
             noexcept(--declval<move_iterator&>()))
    requires BidirectionalIterator<I>()
    {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    STL2_CONSTEXPR_EXT move_iterator operator+(difference_type n) const
    noexcept(is_nothrow_move_constructible<I>::value &&
             noexcept(current_ + n))
    requires RandomAccessIterator<I>()
    {
      return move_iterator{current_ + n};
    }
    STL2_CONSTEXPR_EXT move_iterator& operator+=(difference_type n) &
    noexcept(noexcept(current_ += n))
    requires RandomAccessIterator<I>()
    {
      current_ += n;
      return *this;
    }

    STL2_CONSTEXPR_EXT move_iterator operator-(difference_type n) const
    noexcept(is_nothrow_move_constructible<I>::value &&
             noexcept(current_ - n))
    requires RandomAccessIterator<I>()
    {
      return move_iterator{current_ - n};
    }
    STL2_CONSTEXPR_EXT move_iterator& operator-=(difference_type n) &
    noexcept(noexcept(current_ -= n))
    requires RandomAccessIterator<I>()
    {
      current_ -= n;
      return *this;
    }

    STL2_CONSTEXPR_EXT reference operator[](difference_type n) const
    noexcept(noexcept(__stl2::iter_move(current_ + n)))
    requires RandomAccessIterator<I>()
    {
      return __stl2::iter_move(current_ + n);
    }

    // Extension
    friend STL2_CONSTEXPR_EXT decltype(auto) iter_move(const move_iterator& i)
    STL2_NOEXCEPT_RETURN(
      __stl2::iter_move(i.current_)
    )
    // Extension
    friend STL2_CONSTEXPR_EXT void iter_swap(
      const move_iterator& x, const move_iterator& y)
    noexcept(noexcept(__stl2::iter_swap(x.current_, y.current_)))
    requires IndirectlySwappable<I, I>()
    {
      return __stl2::iter_swap(x.current_, y.current_);
    }
  };

  EqualityComparable{I1, I2}
  STL2_CONSTEXPR_EXT bool
  operator==(const move_iterator<I1>& a, const move_iterator<I2>& b)
  STL2_NOEXCEPT_RETURN(
    __mi_access::current(a) == __mi_access::current(b)
  )

  EqualityComparable{I1, I2}
  STL2_CONSTEXPR_EXT bool
  operator!=(const move_iterator<I1>& a, const move_iterator<I2>& b)
  STL2_NOEXCEPT_RETURN(
    !(a == b)
  )

  StrictTotallyOrdered{I1, I2}
  STL2_CONSTEXPR_EXT bool
  operator<(const move_iterator<I1>& a, const move_iterator<I2>& b)
  STL2_NOEXCEPT_RETURN(
    __mi_access::current(a) < __mi_access::current(b)
  )

  StrictTotallyOrdered{I1, I2}
  STL2_CONSTEXPR_EXT bool
  operator>(const move_iterator<I1>& a, const move_iterator<I2>& b)
  STL2_NOEXCEPT_RETURN(
    b < a
  )

  StrictTotallyOrdered{I1, I2}
  STL2_CONSTEXPR_EXT bool
  operator<=(const move_iterator<I1>& a, const move_iterator<I2>& b)
  STL2_NOEXCEPT_RETURN(
    !(b < a)
  )

  StrictTotallyOrdered{I1, I2}
  STL2_CONSTEXPR_EXT bool
  operator>=(const move_iterator<I1>& a, const move_iterator<I2>& b)
  STL2_NOEXCEPT_RETURN(
    !(a < b)
  )

  SizedSentinel{I1, I2}
  STL2_CONSTEXPR_EXT difference_type_t<I2>
  operator-(const move_iterator<I1>& a, const move_iterator<I2>& b)
  STL2_NOEXCEPT_RETURN(
    __mi_access::current(a) - __mi_access::current(b)
  )

  RandomAccessIterator{I}
  STL2_CONSTEXPR_EXT move_iterator<I>
  operator+(difference_type_t<I> n, const move_iterator<I>& x)
  STL2_NOEXCEPT_RETURN(
    x + n
  )

  template <class I>
  requires InputIterator<__f<I>>()
  STL2_CONSTEXPR_EXT auto make_move_iterator(I&& i)
  STL2_NOEXCEPT_RETURN(
    move_iterator<__f<I>>{__stl2::forward<I>(i)}
  )

  Semiregular{S}
  class move_sentinel : detail::ebo_box<S> {
    friend __mi_access;
    using box_t = detail::ebo_box<S>;
  public:
    constexpr move_sentinel()
    noexcept(is_nothrow_default_constructible<S>::value)
    : box_t{}
    {}
    explicit STL2_CONSTEXPR_EXT move_sentinel(S s)
    noexcept(is_nothrow_move_constructible<S>::value)
    : box_t(__stl2::move(s))
    {}
    template <ConvertibleTo<S> T>
    STL2_CONSTEXPR_EXT move_sentinel(const move_sentinel<T>& s)
    noexcept(is_nothrow_constructible<S, const T&>::value)
    : box_t{__mi_access::sent(s)}
    {}

    template <ConvertibleTo<S> T>
    STL2_CONSTEXPR_EXT move_sentinel& operator=(const move_sentinel<T>& s) &
    noexcept(is_nothrow_assignable<S&, const T&>::value)
    { box_t::get() = __mi_access::sent(s); }

    STL2_CONSTEXPR_EXT S base() const
    noexcept(is_nothrow_copy_constructible<S>::value)
    { return box_t::get(); }
  };

  Sentinel{S, I}
  STL2_CONSTEXPR_EXT bool
  operator==(const move_iterator<I>& i, const move_sentinel<S>& s)
  STL2_NOEXCEPT_RETURN(
    __mi_access::current(i) == __mi_access::sent(s)
  )
  Sentinel{S, I}
  STL2_CONSTEXPR_EXT bool
  operator==(const move_sentinel<S>& s, const move_iterator<I>& i)
  STL2_NOEXCEPT_RETURN(
    i == s
  )

  Sentinel{S, I}
  STL2_CONSTEXPR_EXT bool
  operator!=(const move_iterator<I>& i, const move_sentinel<S>& s)
  STL2_NOEXCEPT_RETURN(
    !(i == s)
  )
  Sentinel{S, I}
  STL2_CONSTEXPR_EXT bool
  operator!=(const move_sentinel<S>& s, const move_iterator<I>& i)
  STL2_NOEXCEPT_RETURN(
    !(i == s)
  )

  SizedSentinel{S, I}
  STL2_CONSTEXPR_EXT difference_type_t<I>
  operator-(const move_sentinel<S>& s, const move_iterator<I>& i)
  STL2_NOEXCEPT_RETURN(
    __mi_access::sent(s) - __mi_access::current(i)
  )
  SizedSentinel{S, I}
  STL2_CONSTEXPR_EXT difference_type_t<I>
    operator-(const move_iterator<I>& i, const move_sentinel<S>& s)
  STL2_NOEXCEPT_RETURN(
    -(s - i)
  )

  template <class S>
  requires
    models::Semiregular<__f<S>>
  STL2_CONSTEXPR_EXT auto make_move_sentinel(S&& s)
  STL2_NOEXCEPT_RETURN(
    move_sentinel<__f<S>>(__stl2::forward<S>(s))
  )
} STL2_CLOSE_NAMESPACE

#endif
