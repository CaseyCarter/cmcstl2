#ifndef STL2_DETAIL_ITERATOR_MOVE_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_MOVE_ITERATOR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>

namespace stl2 { inline namespace v1 {
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
    noexcept(stl2::is_nothrow_move_constructible<I>::value) :
    current_{stl2::move(i)} {}

  move_iterator(const move_iterator<ConvertibleTo<I> >& u) :
    current_{u.base()} {}

  move_iterator& operator=(const move_iterator<ConvertibleTo<I> >& u) & {
    current_ = u.base();
    return *this;
  }

  iterator_type base() const {
    return current_;
  }

  reference operator*() const {
    using stl2::iter_move;
    return iter_move(current_);
  }

  // FIXME: constraints
  pointer operator->() const {
    return current_;
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
    return stl2::iter_move(current_ + n);
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
  return move_iterator<I>{stl2::move(i)};
}
}} // namespace stl2::v1

#endif
