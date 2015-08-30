#ifndef STL2_DETAIL_ITERATOR_COMMON_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_COMMON_ITERATOR_HPP

#include <cassert>
#include <type_traits>

#include <stl2/variant.hpp>
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

template <InputIterator I, __WeakSentinel<I> S>
  requires !Same<I, S>()
class common_iterator {
  friend struct __ci_access;
  variant<I, S> v_;
public:
  using difference_type = DifferenceType<I>;
  using value_type = ValueType<I>;
  using iterator_category =
    conditional_t<__forward_iter<I>,
    std::forward_iterator_tag,
    std::input_iterator_tag>;
  using reference = ReferenceType<I>;
  common_iterator() = default;
  common_iterator(I i) : v_{stl2::move(i)} {}
  common_iterator(S s) : v_{stl2::move(s)} {}
  template <ConvertibleTo<I> U, ConvertibleTo<S> V>
  common_iterator(const common_iterator<U, V>& u) : common_iterator() {
    *this = u;
  }
  template <ConvertibleTo<I> U, ConvertibleTo<S> V>
  common_iterator& operator=(const common_iterator<U, V>& u) {
    stl2::visit_with_index([this](auto i, auto& t) {
      v_.emplace<i()>(t);
    }, u.v_);
    return *this;
  }
  ~common_iterator() = default;
  reference operator*() const {
    assert(holds_alternative<I>(v_));
    return *stl2::get_unchecked<I>(v_);
  }
  common_iterator& operator++() {
    assert(holds_alternative<I>(v_));
    ++stl2::get_unchecked<I>(v_);
    return *this;
  }
  common_iterator operator++(int) {
    common_iterator tmp(*this);
    ++*this;
    return tmp;
  }
};

struct __ci_access {
  template <class I1, class S1, class I2, class S2>
    requires EqualityComparable<I1, I2>() &&
      ext::WeaklyEqualityComparable<I1, S2>() &&
      ext::WeaklyEqualityComparable<I2, S1>()
  static bool equals(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y) {
      return stl2::visit([](auto& l, auto& r) { return l == r; }, x.v_, y.v_);
  }
  template <class I1, class S1, class I2, class S2>
    requires
      __CompatibleSizedIteratorRange<I1, I2, DifferenceType<I2>> &&
      __CompatibleSizedIteratorRange<I1, S2, DifferenceType<I2>> &&
      __CompatibleSizedIteratorRange<I2, S1, DifferenceType<I2>>
  static DifferenceType<I2> difference(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y) {
      return stl2::visit([](auto& l, auto& r) {
        return static_cast<DifferenceType<I2>>(l - r);
      }, x.v_, y.v_);
  }
};

template <class I1, class S1, class I2, class S2>
  requires EqualityComparable<I1, I2>() &&
    ext::WeaklyEqualityComparable<I1, S2>() &&
    ext::WeaklyEqualityComparable<I2, S1>()
bool operator==(
  const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y) {
    return __ci_access::equals(x, y);
}

template <class I1, class S1, class I2, class S2>
  requires EqualityComparable<I1, I2>() &&
    ext::WeaklyEqualityComparable<I1, S2>() &&
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
    return __ci_access::difference(x, y);
}

}}

#endif
