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

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
  template <WeakInputIterator I>
    // requires Same<ReferenceType<I>, ValueType<I>&>()
  class move_iterator {
  public:
    using iterator_type = I;
    using difference_type = DifferenceType<I>;
    using value_type = ValueType<I>;
    using iterator_category = IteratorCategory<I>;
    using reference = RvalueReferenceType<I>;
    using pointer = I;
  
    move_iterator() = default;
    explicit move_iterator(I i)
      noexcept(is_nothrow_move_constructible<I>::value) :
      current_{__stl2::move(i)} {}
  
    move_iterator(const move_iterator<ConvertibleTo<I> >& u) :
      current_{u.base()} {}
  
    move_iterator& operator=(const move_iterator<ConvertibleTo<I> >& u) & {
      current_ = u.base();
      return *this;
    }
  
    iterator_type base() const
      noexcept(is_nothrow_copy_constructible<I>::value) {
      return current_;
    }
  
    reference operator*() const
      noexcept(is_nothrow_move_constructible<RvalueReferenceType<I>>::value &&
               noexcept(__stl2::iter_move(declval<I&>()))) {
      return __stl2::iter_move(current_);
    }
  
    // FIXME: constraints
    pointer operator->() const
      noexcept(is_nothrow_copy_constructible<I>::value) {
      return current_;
    }
  
    move_iterator& operator++() &
      noexcept(noexcept(++declval<I&>())) {
      ++current_;
      return *this;
    }
    move_iterator operator++(int) & {
      auto tmp = *this;
      ++*this;
      return tmp;
    }
  
    move_iterator& operator--() &
      noexcept(noexcept(--declval<I&>()))
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
  
    TotallyOrdered{I1, I2}
    friend bool operator<(const move_iterator<I1>&,
                          const move_iterator<I2>&);
  
    SizedIteratorRange{I1, I2}
    friend DifferenceType<I2> operator-(const move_iterator<I1>&,
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
  
  TotallyOrdered{I1, I2}
  bool operator<(const move_iterator<I1>& a,
                 const move_iterator<I2>& b) {
    return a.current_ < b.current_;
  }
  
  TotallyOrdered{I1, I2}
  bool operator>(const move_iterator<I1>& a,
                 const move_iterator<I2>& b) {
    return b < a;
  }
  
  TotallyOrdered{I1, I2}
  bool operator<=(const move_iterator<I1>& a,
                  const move_iterator<I2>& b) {
    return !(b < a);
  }
  
  TotallyOrdered{I1, I2}
  bool operator>=(const move_iterator<I1>& a,
                  const move_iterator<I2>& b) {
    return !(a < b);
  }
  
  SizedIteratorRange{I1, I2}
  DifferenceType<I2> operator-(const move_iterator<I1>& a,
                               const move_iterator<I2>& b) {
    return a.current_ - b.current_;
  }
  
  RandomAccessIterator{I}
  move_iterator<I> operator+(DifferenceType<I> n, const move_iterator<I>& x) {
    return x + n;
  }
  
  WeakInputIterator{I}
  auto make_move_iterator(I i) {
    return move_iterator<I>{__stl2::move(i)};
  }
} STL2_CLOSE_NAMESPACE

#endif
