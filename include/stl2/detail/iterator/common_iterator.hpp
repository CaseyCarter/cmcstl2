#ifndef STL2_DETAIL_ITERATOR_COMMON_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_COMMON_ITERATOR_HPP

#include <cassert>
#include <type_traits>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/operations.hpp>

namespace stl2 {
inline namespace v1 {

///////////////////////////////////////////////////////////////////////////
// common_iterator [common.iterators]
//
template<class S, class I>
concept bool __WeakSentinel =
  Iterator<I>() && Regular<S>() &&
  ext::WeaklyEqualityComparable<I, S>();

template <Iterator I>
constexpr bool __forward_iter = false;

template <ForwardIterator I>
constexpr bool __forward_iter<I> = true;

template <InputIterator I, __WeakSentinel<I> S>
  requires !Same<I, S>()
class common_iterator;

template <InputIterator I1, __WeakSentinel<I1> S1, InputIterator I2, __WeakSentinel<I2> S2>
  requires EqualityComparable<I1, I2>() && ext::WeaklyEqualityComparable<I1, S2>() &&
    ext::WeaklyEqualityComparable<I2, S1>()
bool operator==(
  const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y);

template <InputIterator I1, __WeakSentinel<I1> S1, InputIterator I2, __WeakSentinel<I2> S2>
  requires SizedIteratorRange<I1, I1>() && SizedIteratorRange<I2, I2>() &&
    requires(const I1 i1, const S1 s1, const I2 i2, const S2 s2) {
      {i1 - i2} -> DifferenceType<I2>;
      {i2 - i1} -> DifferenceType<I2>;
      {i1 - s2} -> DifferenceType<I2>;
      {s2 - i1} -> DifferenceType<I2>;
      {i2 - s1} -> DifferenceType<I2>;
      {s1 - i2} -> DifferenceType<I2>;
    }
DifferenceType<I2> operator-(
  const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y);

template <InputIterator I, __WeakSentinel<I> S>
  requires !Same<I, S>()
class common_iterator {
  template <InputIterator I1, __WeakSentinel<I1> S1, InputIterator I2, __WeakSentinel<I2> S2>
    requires EqualityComparable<I1, I2>() && ext::WeaklyEqualityComparable<I1, S2>() &&
      ext::WeaklyEqualityComparable<I2, S1>()
  friend bool operator==(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y);

  template <InputIterator I1, __WeakSentinel<I1> S1, InputIterator I2, __WeakSentinel<I2> S2>
    requires SizedIteratorRange<I1, I1>() && SizedIteratorRange<I2, I2>() &&
      requires(const I1 i1, const S1 s1, const I2 i2, const S2 s2) {
        {i1 - i2} -> DifferenceType<I2>;
        {i2 - i1} -> DifferenceType<I2>;
        {i1 - s2} -> DifferenceType<I2>;
        {s2 - i1} -> DifferenceType<I2>;
        {i2 - s1} -> DifferenceType<I2>;
        {s1 - i2} -> DifferenceType<I2>;
      }
  friend DifferenceType<I2> operator-(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y);
public:
  using difference_type = DifferenceType<I>;
  using value_type = ValueType<I>;
  using iterator_category =
    conditional_t<__forward_iter<I>,
    std::forward_iterator_tag,
    std::input_iterator_tag>;
  using reference = ReferenceType<I>;
  common_iterator() : is_sentinel{}, iter{}, sent{} {}
  common_iterator(I i) : is_sentinel{false}, iter(stl2::move(i)), sent{} {}
  common_iterator(S s) : is_sentinel{true}, iter{}, sent(stl2::move(s)) {}
  template <InputIterator U, __WeakSentinel<U> V>
    requires ConvertibleTo<U, I>() && ConvertibleTo<V, S>()
  common_iterator(const common_iterator<U, V>& u) : common_iterator() {
    *this = u;
  }
  template <InputIterator U, __WeakSentinel<U> V>
    requires ConvertibleTo<U, I>() && ConvertibleTo<V, S>()
  common_iterator& operator=(const common_iterator<U, V>& u) {
    is_sentinel = u.is_sentinel;
    if(is_sentinel)
      sent = u.sent;
    else
      iter = u.iter;
    return *this;
  }
  ~common_iterator() = default;
  reference operator*() const {
    assert(!is_sentinel);
    return *iter;
  }
  common_iterator& operator++() {
    assert(!is_sentinel);
    ++iter;
    return *this;
  }
  common_iterator operator++(int) {
    common_iterator tmp(*this);
    ++*this;
    return tmp;
  }
private:
  bool is_sentinel;
  I iter;
  S sent;
};

template <InputIterator I1, __WeakSentinel<I1> S1, InputIterator I2, __WeakSentinel<I2> S2>
  requires EqualityComparable<I1, I2>() && ext::WeaklyEqualityComparable<I1, S2>() &&
    ext::WeaklyEqualityComparable<I2, S1>()
bool operator==(
  const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y) {
    return x.is_sentinel ?
      (y.is_sentinel || y.iter == x.sent) :
        (y.is_sentinel ?
          x.iter == y.sent :
          x.iter == y.iter);
}

template <InputIterator I1, __WeakSentinel<I1> S1, InputIterator I2, __WeakSentinel<I2> S2>
  requires EqualityComparable<I1, I2>() && ext::WeaklyEqualityComparable<I1, S2>() &&
    ext::WeaklyEqualityComparable<I2, S1>()
bool operator!=(
  const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y) {
    return !(x == y);
}

template <InputIterator I1, __WeakSentinel<I1> S1, InputIterator I2, __WeakSentinel<I2> S2>
  requires SizedIteratorRange<I1, I1>() && SizedIteratorRange<I2, I2>() &&
    requires(const I1 i1, const S1 s1, const I2 i2, const S2 s2) {
      {i1 - i2} -> DifferenceType<I2>;
      {i2 - i1} -> DifferenceType<I2>;
      {i1 - s2} -> DifferenceType<I2>;
      {s2 - i1} -> DifferenceType<I2>;
      {i2 - s1} -> DifferenceType<I2>;
      {s1 - i2} -> DifferenceType<I2>;
    }
DifferenceType<I2> operator-(
  const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y) {
    return x.is_sentinel ?
      (y.is_sentinel ? 0 : x.sent - y.iter) :
        (y.is_sentinel ?
          x.iter - y.sent :
          x.iter - y.iter);
}

}}

#endif
