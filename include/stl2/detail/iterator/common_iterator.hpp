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

template <class I, class S, class D>
concept bool __CompatibleSizedIteratorRange =
  SizedIteratorRange<I, S>() &&
  requires (const I i, const S s) {
    {i - s} -> D;
    {s - i} -> D;
  };

template <Iterator I>
constexpr bool __forward_iter = false;
template <ForwardIterator I>
constexpr bool __forward_iter<I> = true;

struct __ci_access;

template <InputIterator I, __WeakSentinel<I> S>
  requires !Same<I, S>()
class common_iterator {
  friend __ci_access;
  bool is_sentinel;
  I iter;
  S sent;
  template <InputIterator U, __WeakSentinel<U> V>
    requires ConvertibleTo<U, I>() && ConvertibleTo<V, S>()
  void assign_(const common_iterator<U, V>& u) {
    if(u.is_sentinel)
      sent = u.sent;
    else
      iter = u.iter;
    is_sentinel = u.is_sentinel;
  }
public:
  using difference_type = DifferenceType<I>;
  using value_type = ValueType<I>;
  using iterator_category =
    conditional_t<__forward_iter<I>,
    stl2::forward_iterator_tag,
    stl2::input_iterator_tag>;
  using reference = ReferenceType<I>;
  common_iterator() : is_sentinel{}, iter{}, sent{} {}
  common_iterator(I i) : is_sentinel{false}, iter(stl2::move(i)), sent{} {}
  common_iterator(S s) : is_sentinel{true}, iter{}, sent(stl2::move(s)) {}
  common_iterator(const common_iterator& u) : common_iterator() {
    this->assign_(u);
  }
  template <InputIterator U, __WeakSentinel<U> V>
    requires ConvertibleTo<U, I>() && ConvertibleTo<V, S>()
  common_iterator(const common_iterator<U, V>& u) : common_iterator() {
    this->assign_(u);
  }
  common_iterator& operator=(const common_iterator& u) {
    this->assign_(u);
    return *this;
  }
  template <InputIterator U, __WeakSentinel<U> V>
    requires ConvertibleTo<U, I>() && ConvertibleTo<V, S>()
  common_iterator& operator=(const common_iterator<U, V>& u) {
    this->assign_(u);
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
};

struct __ci_access {
  template <class I, class S>
  static bool is_sentinel(const common_iterator<I, S>& ci) {
    return ci.is_sentinel;
  }
  template <class I, class S>
  static const I& iter(const common_iterator<I, S>& ci) {
    return ci.iter;
  }
  template <class I, class S>
  static const S& sent(const common_iterator<I, S>& ci) {
    return ci.sent;
  }
};

template <class I1, class S1, class I2, class S2>
  requires EqualityComparable<I1, I2>() && ext::WeaklyEqualityComparable<I1, S2>() &&
    ext::WeaklyEqualityComparable<I2, S1>()
bool operator==(
  const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y) {
    return __ci_access::is_sentinel(x) ?
      (__ci_access::is_sentinel(y) || __ci_access::iter(y) == __ci_access::sent(x)) :
        (__ci_access::is_sentinel(y) ?
          __ci_access::iter(x) == __ci_access::sent(y) :
          __ci_access::iter(x) == __ci_access::iter(y));
}

template <class I1, class S1, class I2, class S2>
  requires EqualityComparable<I1, I2>() && ext::WeaklyEqualityComparable<I1, S2>() &&
    ext::WeaklyEqualityComparable<I2, S1>()
bool operator!=(
  const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y) {
    return !(x == y);
}

template <class I1, class S1, class I2, class S2>
  requires
    __CompatibleSizedIteratorRange<I1, I2, DifferenceType<I2>> &&
    __CompatibleSizedIteratorRange<I1, S2, DifferenceType<I2>> &&
    __CompatibleSizedIteratorRange<I2, S1, DifferenceType<I2>>
DifferenceType<I2> operator-(
  const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y) {
    return __ci_access::is_sentinel(x) ?
      (__ci_access::is_sentinel(y) ? 0 : __ci_access::sent(x) - __ci_access::iter(y)) :
        (__ci_access::is_sentinel(y) ?
          __ci_access::iter(x) - __ci_access::sent(y) :
          __ci_access::iter(x) - __ci_access::iter(y));
}

}}

#endif
