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
  template <WeakInputIterator I>
  class move_iterator {
  public:
    using iterator_type = I;
    using difference_type = difference_type_t<I>;
    using value_type = value_type_t<I>;
    using iterator_category = meta::if_c<
      models::DerivedFrom<iterator_category_t<I>, input_iterator_tag>,
      input_iterator_tag,
      weak_input_iterator_tag>;
    using reference = rvalue_reference_t<I>;

    move_iterator() = default;

    template <class II>
      requires Constructible<I, II&&>()
    explicit move_iterator(II&& i) :
      current_{__stl2::forward<II>(i)} {}

    move_iterator(const move_iterator<ConvertibleTo<I> >& u) :
      current_{u.base()} {}

    move_iterator& operator=(const move_iterator<ConvertibleTo<I> >& u) & {
      current_ = u.base();
      return *this;
    }

    I base() const {
      return current_;
    }

    reference operator*() const {
      return __stl2::iter_move(current_);
    }

    move_iterator& operator++() & {
      ++current_;
      return *this;
    }
    move_iterator operator++(int) & {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    move_iterator& operator--() &
      requires BidirectionalIterator<I>() {
      --current_;
      return *this;
    }
    move_iterator operator--(int) &
      requires BidirectionalIterator<I>() {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    move_iterator operator+(difference_type n) const
      requires RandomAccessIterator<I>() {
      return move_iterator{current_ + n};
    }
    move_iterator& operator+=(difference_type n) &
      requires RandomAccessIterator<I>() {
      current_ += n;
      return *this;
    }

    move_iterator operator-(difference_type n) const
      requires RandomAccessIterator<I>() {
      return move_iterator{current_ - n};
    }
    move_iterator& operator-=(difference_type n) &
      requires RandomAccessIterator<I>() {
      current_ -= n;
      return *this;
    }

    reference operator[](difference_type n) const
      requires RandomAccessIterator<I>() {
      return __stl2::iter_move(current_ + n);
    }

    EqualityComparable{I1, I2}
    friend bool operator==(const move_iterator<I1>&,
                           const move_iterator<I2>&);

    StrictTotallyOrdered{I1, I2}
    friend bool operator<(const move_iterator<I1>&,
                          const move_iterator<I2>&);

    SizedIteratorRange{I1, I2}
    friend difference_type_t<I2> operator-(const move_iterator<I1>&,
                                           const move_iterator<I2>&);

  private:
    I current_{};
  };

  EqualityComparable{I1, I2}
  bool operator==(const move_iterator<I1>& a,
                  const move_iterator<I2>& b) {
    return a.current_ == b.current_;
  }

  EqualityComparable{I1, I2}
  bool operator!=(const move_iterator<I1>& a,
                  const move_iterator<I2>& b) {
    return !(a == b);
  }

  StrictTotallyOrdered{I1, I2}
  bool operator<(const move_iterator<I1>& a,
                 const move_iterator<I2>& b) {
    return a.current_ < b.current_;
  }

  StrictTotallyOrdered{I1, I2}
  bool operator>(const move_iterator<I1>& a,
                 const move_iterator<I2>& b) {
    return b < a;
  }

  StrictTotallyOrdered{I1, I2}
  bool operator<=(const move_iterator<I1>& a,
                  const move_iterator<I2>& b) {
    return !(b < a);
  }

  StrictTotallyOrdered{I1, I2}
  bool operator>=(const move_iterator<I1>& a,
                  const move_iterator<I2>& b) {
    return !(a < b);
  }

  SizedIteratorRange{I1, I2}
  difference_type_t<I2> operator-(const move_iterator<I1>& a,
                                  const move_iterator<I2>& b) {
    return a.current_ - b.current_;
  }

  RandomAccessIterator{I}
  move_iterator<I> operator+(difference_type_t<I> n,
                             const move_iterator<I>& x) {
    return x + n;
  }

  template <class I>
    requires WeakInputIterator<decay_t<I>>()
      && Constructible<decay_t<I>, I&&>()
  auto make_move_iterator(I&& i) {
    return move_iterator<decay_t<I>>{__stl2::forward<I>(i)};
  }
} STL2_CLOSE_NAMESPACE

#endif
