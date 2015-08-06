#ifndef STL2_DETAIL_ITERATOR_REVERSE_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_REVERSE_ITERATOR_HPP

#include <type_traits>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/operations.hpp>

namespace stl2 { inline namespace v1 {

///////////////////////////////////////////////////////////////////////////
// reverse_iterator [reverse.iterator]
//
template <BidirectionalIterator I>
class reverse_iterator {
public:
  using iterator_type = I;
  using difference_type = DifferenceType<I>;
  using value_type = ValueType<I>;
  using iterator_category = IteratorCategory<I>;
  using reference = ReferenceType<I>;
  using pointer = I;

  reverse_iterator() = default;
  explicit reverse_iterator(I x)
    noexcept(std::is_nothrow_move_constructible<I>::value) :
    current{stl2::move(x)} {}

  template <ConvertibleTo<I> U>
  reverse_iterator(const reverse_iterator<U>& u)
    noexcept(std::is_nothrow_constructible<I, const U&>::value) :
    current{u.current} {}

  template <ConvertibleTo<I> U>
  reverse_iterator& operator=(const reverse_iterator<U>& u) &
    noexcept(std::is_nothrow_assignable<I, const U&>::value) {
    current = u.current;
    return *this;
  }

  I base() const
    noexcept(std::is_nothrow_copy_constructible<I>::value) {
    return current;
  }
  // 20150802: Extension
  explicit operator const I&() const& noexcept {
    return current;
  }
  // 20150802: Extension
  explicit operator I&&() && noexcept {
    return stl2::move(current);
  }

  reference operator*() const {
    return *stl2::prev(current);
  }

  pointer operator->() const
#if 0 // FIXME: hard error when I is a pointer.
    requires _Is<I, std::is_pointer> ||
      requires (const I& i) {
        i.operator->();
      }
#endif
  {
    return stl2::prev(current);
  }

  reverse_iterator& operator++() & {
    --current;
    return *this;
  }
  reverse_iterator operator++(int) & {
    auto tmp = *this;
    ++*this;
    return tmp;
  }

  reverse_iterator& operator--() & {
    ++current;
    return *this;
  }
  reverse_iterator operator--(int) & {
    auto tmp = *this;
    --*this;
    return tmp;
  }

  reverse_iterator operator+(difference_type n) const
    requires RandomAccessIterator<I>() {
    return reverse_iterator{current - n};
  }
  reverse_iterator& operator+=(difference_type n) &
    requires RandomAccessIterator<I>() {
    current -= n;
    return *this;
  }
  reverse_iterator operator-(difference_type n) const
    requires RandomAccessIterator<I>() {
    return reverse_iterator{current + n};
  }
  reverse_iterator& operator-=(difference_type n) &
    requires RandomAccessIterator<I>() {
    current += n;
    return *this;
  }

  decltype(auto) operator[](difference_type n) const
    requires RandomAccessIterator<I>() {
    return current[-n - 1];
  }

protected:
  I current{};
};

EqualityComparable{I1, I2}
bool operator==(const reverse_iterator<I1>& x,
                const reverse_iterator<I2>& y) {
  return static_cast<const I1&>(x) == static_cast<const I2&>(y);
}

EqualityComparable{I1, I2}
bool operator!=(const reverse_iterator<I1>& x,
                const reverse_iterator<I2>& y) {
  return static_cast<const I1&>(x) != static_cast<const I2&>(y);
}

TotallyOrdered{I1, I2}
bool operator<(const reverse_iterator<I1>& x,
               const reverse_iterator<I2>& y) {
  return static_cast<const I1&>(x) > static_cast<const I2&>(y);
}

TotallyOrdered{I1, I2}
bool operator>(const reverse_iterator<I1>& x,
               const reverse_iterator<I2>& y) {
  return static_cast<const I1&>(x) < static_cast<const I2&>(y);
}

TotallyOrdered{I1, I2}
bool operator<=(const reverse_iterator<I1>& x,
                const reverse_iterator<I2>& y) {
  return static_cast<const I1&>(x) >= static_cast<const I2&>(y);
}

TotallyOrdered{I1, I2}
bool operator>=(const reverse_iterator<I1>& x,
                const reverse_iterator<I2>& y) {
  return static_cast<const I1&>(x) <= static_cast<const I2&>(y);
}

SizedIteratorRange{I2, I1}
DifferenceType<I2> operator-(const reverse_iterator<I1>& x,
                             const reverse_iterator<I2>& y) {
  return static_cast<const I2&>(y) - static_cast<const I1&>(x);
}

template <class I>
reverse_iterator<I> operator+(DifferenceType<I> n,
                              const reverse_iterator<I>& x) {
  return reverse_iterator<I>{static_cast<const I&>(x) - n};
}

BidirectionalIterator{I}
auto make_reverse_iterator(I i) {
  return reverse_iterator<I>{stl2::move(i)};
}

}} // namespace stl2::v1

#endif
