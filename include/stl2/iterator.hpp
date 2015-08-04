#ifndef STL2_ITERATOR_HPP
#define STL2_ITERATOR_HPP

#include <cassert>
#include <initializer_list>
#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/iterator.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/concepts/range.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/utility.hpp>

namespace stl2 { inline namespace v1 {

///////////////////////////////////////////////////////////////////////////
// Iterator operations [iterator.operations]
//
// advance
WeakIterator{I}
void advance(I& i, DifferenceType<I> n) {
  while (n != 0) {
    --n;
    ++i;
  }
}

BidirectionalIterator{I}
void advance(I& i, DifferenceType<I> n) {
  if (n > 0) {
    while (n != 0) {
      --n;
      ++i;
    }
  } else {
    while (n != 0) {
      ++n;
      --i;
    }
  }
}

RandomAccessIterator{I}
void advance(I& i, DifferenceType<I> n) {
  i += n;
}

Sentinel{S, I}
void advance(I& i, S bound) {
  while (i != bound) {
    ++i;
  }
}

SizedIteratorRange{I, S}
void advance(I& i, S bound) {
  stl2::advance(i, bound - i);
}

Iterator{I}
void advance(I& i, I bound) {
  i = bound;
}

Sentinel{S, I}
DifferenceType<I> advance(I& i, DifferenceType<I> n, S bound) {
  while (i != bound && n != 0) {
    ++i;
    --n;
  }
  return n;
}

template <BidirectionalIterator I, Sentinel<I> S>
DifferenceType<I> advance(I& i, DifferenceType<I> n, S bound) {
  if (n > 0) {
    while (i != bound && n != 0) {
      ++i;
      --n;
    }
  } else {
    while (i != bound && n != 0) {
      --i;
      ++n;
    }
  }
  return n;
}

SizedIteratorRange{I, S}
DifferenceType<I> advance(I& i, DifferenceType<I> n, S bound) {
  const auto D = DifferenceType<I>{bound - i};
  if (n >= 0 ? n >= D : n <= D) {
    stl2::advance(i, bound);
    return n - D;
  } else {
    stl2::advance(i, n);
    return 0;
  }
}

// distance
Sentinel{S, I}
DifferenceType<I> distance(I first, S last) {
  DifferenceType<I> n = 0;
  while (first != last) {
    ++n;
    ++first;
  }
  return n;
}

SizedIteratorRange{I, S}
DifferenceType<I> distance(I first, S last) {
  return last - first;
}

Range{R}
DifferenceType<R> distance(R&& r) {
  return stl2::distance(begin(r), end(r));
}

SizedRange{R}
DifferenceType<R> distance(R&& r) {
  return size(r);
}

// next
WeakIterator{I}
I next(I x, DifferenceType<I> n = 1) {
  stl2::advance(x, n);
  return x;
}

Sentinel{S, I}
I next(I x, S bound) {
  stl2::advance(x, bound);
  return x;
}

Sentinel{S, I}
I next(I x, DifferenceType<I> n, S bound) {
  stl2::advance(x, n, bound);
  return x;
}

// prev
BidirectionalIterator{I}
I prev(I x, DifferenceType<I> n = 1) {
  stl2::advance(x, -n);
  return x;
}

template <BidirectionalIterator I, Sentinel<I> S>
I prev(I x, DifferenceType<I> n, S bound) {
  stl2::advance(x, -n, bound);
  return x;
}

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
  explicit reverse_iterator(I x) :
    current{stl2::move(x)} {}

  template <ConvertibleTo<I> U>
  reverse_iterator(const reverse_iterator<U>& u) :
    current{u.current} {}

  template <ConvertibleTo<I> U>
  reverse_iterator& operator=(const reverse_iterator<U>& u) & {
    current = u.current;
    return *this;
  }

  I base() const {
    return current;
  }
  // 20150802: Extension
  explicit operator const I&() const& {
    return current;
  }
  // 20150802: Extension
  explicit operator I&&() && {
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

namespace detail {
  template <class>
  concept bool _Auto = true;
}

template <detail::_Auto C>
constexpr auto cbegin(const C& c)
  noexcept(noexcept(begin(c))) ->
  decltype(begin(c)) {
  return begin(c);
}

template <detail::_Auto C>
constexpr auto cend(const C& c)
  noexcept(noexcept(end(c))) ->
  decltype(end(c)) {
  return end(c);
}

template <detail::_Auto C>
constexpr auto rbegin(C& c) -> decltype(c.rbegin()) {
  return c.rbegin();
}

template <detail::_Auto C>
constexpr auto rbegin(const C& c) -> decltype(c.rbegin()) {
  return c.rbegin();
}

template <detail::_Auto C>
constexpr auto rend(C& c) -> decltype(c.rend()) {
  return c.rend();
}

template <detail::_Auto C>
constexpr auto rend(const C& c) -> decltype(c.rend()) {
  return c.rend();
}

template <detail::_Auto T, size_t N>
reverse_iterator<T*> rbegin(T (&array)[N]) {
  return reverse_iterator<T*>{array + N};
}

template <detail::_Auto T, size_t N>
reverse_iterator<T*> rend(T (&array)[N]) {
  return reverse_iterator<T*>{array};
}

template <detail::_Auto E>
reverse_iterator<const E*> rbegin(std::initializer_list<E> il) {
  return reverse_iterator<const E*>{il.end()};
}

template <detail::_Auto E>
reverse_iterator<const E*> rend(std::initializer_list<E> il) {
  return reverse_iterator<const E*>{il.begin()};
}

template <detail::_Auto C>
constexpr auto crbegin(const C& c) -> decltype(stl2::rbegin(c)) {
  return stl2::rbegin(c);
}

template <detail::_Auto C>
constexpr auto crend(const C& c) -> decltype(stl2::rend(c)) {
  return stl2::rend(c);
}

template <class Derived, class Container>
  requires requires { typename Container::value_type; }
class __insert_iterator_base {
public:
  using container_type = Container;
  using difference_type = std::ptrdiff_t;
  using iterator_category = output_iterator_tag;

  // Extension
  using value_type = typename Container::value_type;

  __insert_iterator_base() = default;
  explicit __insert_iterator_base(Container& x) :
    container{std::addressof(x)} {}

  Derived& operator*() {
    return static_cast<Derived&>(*this);
  }

  Derived& operator++() & {
    return static_cast<Derived&>(*this);
    return *this;
  }
  Derived operator++(int) & {
    return static_cast<Derived&>(*this);
  }

protected:
  Container* container = nullptr;
};

///////////////////////////////////////////////////////////////////////////
// back_insert_iterator [back.insert.iterator]
//
namespace detail {
template <class C, class V>
concept bool back_insertable =
  requires (C& c, V&& v) {
    c.push_back((V&&)v);
  };
}

template <class Container>
  requires requires { typename Container::value_type; }
class back_insert_iterator :
  public __insert_iterator_base<back_insert_iterator<Container>, Container> {
  using __base_t = __insert_iterator_base<back_insert_iterator<Container>, Container>;

public:
  using typename __base_t::value_type;
  using __base_t::__base_t;

  back_insert_iterator&
  operator=(const value_type& value) &
    requires detail::back_insertable<Container, const value_type&> {
    assert(this->container);
    this->container->push_back(value);
    return *this;
  }

  back_insert_iterator&
  operator=(value_type&& value) &
    requires detail::back_insertable<Container, value_type&&> {
    assert(this->container);
    this->container->push_back(stl2::move(value));
    return *this;
  }
};

template <class Container>
auto back_inserter(Container& x) {
  return back_insert_iterator<Container>{x};
}

///////////////////////////////////////////////////////////////////////////
// front_insert_iterator [front.insert.iterator]
//
namespace detail {
template <class C, class V>
concept bool front_insertable =
  requires (C& c, V&& v) {
    c.push_front((V&&)v);
  };
}

template <class Container>
  requires requires { typename Container::value_type; }
class front_insert_iterator :
  public __insert_iterator_base<front_insert_iterator<Container>, Container> {
  using __base_t = __insert_iterator_base<front_insert_iterator<Container>, Container>;
public:
  using typename __base_t::value_type;
  using __base_t::__base_t;

  front_insert_iterator&
  operator=(const value_type& value) &
    requires detail::front_insertable<Container, const value_type&> {
    assert(this->container);
    this->container->push_front(value);
    return *this;
  }

  front_insert_iterator&
  operator=(value_type&& value) &
    requires detail::front_insertable<Container, value_type&&> {
    assert(this->container);
    this->container->push_front(stl2::move(value));
    return *this;
  }
};

template <class Container>
auto front_inserter(Container& x) {
  return front_insert_iterator<Container>{x};
}

///////////////////////////////////////////////////////////////////////////
// insert_iterator [insert.iterator]
//
namespace detail {
template <class C, class V>
concept bool insertable =
  requires (C& c, typename C::iterator i, V&& v) {
    {  c.insert(i, (V&&)v) } -> typename C::iterator;
  };
}

template <class Container>
  requires requires {
    typename Container::value_type;
    typename Container::iterator;
  }
class insert_iterator :
  public __insert_iterator_base<insert_iterator<Container>, Container> {
  using __base_t = __insert_iterator_base<insert_iterator<Container>, Container>;
public:
  using typename __base_t::value_type;
  // Extension
  using iterator_type = typename Container::iterator;

  insert_iterator() = default;
  explicit insert_iterator(Container& x, iterator_type i):
    __base_t{x}, iter{stl2::move(i)} {}

  insert_iterator&
  operator=(const value_type& value) &
    requires detail::insertable<Container, const value_type&> {
    assert(this->container);
    iter = this->container->insert(iter, value);
    ++iter;
    return *this;
  }

  insert_iterator&
  operator=(value_type&& value) &
    requires detail::insertable<Container, value_type&&> {
    assert(this->container);
    iter = this->container->insert(iter, stl2::move(value));
    ++iter;
    return *this;
  }

protected:
  iterator_type iter{};
};

template <class Container>
auto inserter(Container& x, typename Container::iterator i) {
  return insert_iterator<Container>{x, stl2::move(i)};
}

///////////////////////////////////////////////////////////////////////////
// iterator_traits [iterator.assoc]
//
template <WeakInputIterator I>
struct __pointer_type {
  using type = std::add_pointer_t<ReferenceType<I>>;
};

template <WeakInputIterator I>
  requires requires (I i) { { i.operator->() } -> auto&&; }
struct __pointer_type<I> {
  using type = decltype(declval<I>().operator->());
};

template <class>
struct __iterator_traits { };

template <WeakIterator I>
struct __iterator_traits<I> {
  using difference_type = DifferenceType<I>;
  using value_type = void;
  using reference = void;
  using pointer = void;
  using iterator_category = output_iterator_tag;
};

template <WeakInputIterator I>
struct __iterator_traits<I> {
  using difference_type = DifferenceType<I>;
  using value_type = ValueType<I>;
  using reference = ReferenceType<I>;
  using pointer = meta::_t<__pointer_type<I>>;
  using iterator_category = IteratorCategory<I>;
};

template <class I>
using iterator_traits = __iterator_traits<I>;

///////////////////////////////////////////////////////////////////////////
// Standard iterator traits [iterator.stdtraits]
// 20150802: Not to spec: Adds constraints to ::iterator_category so as to
//           apply only to STL2 iterators and avoid "partial specialization
//           after instantiation" errors.
//
template <stl2::WeakIterator W>
struct __std_out_value_type {
  using type = void;
};
template <stl2::WeakIterator W>
  requires detail::MemberValueType<W>
struct __std_out_value_type<W> {
  using type = typename W::value_type;
};

template <stl2::WeakIterator W, class Default = void>
struct __std_reference_type {
  using type = Default;
};
template <stl2::WeakIterator W, class Default>
  requires requires { typename W::reference; }
struct __std_reference_type<W, Default> {
  using type = typename W::reference;
};

template <stl2::WeakIterator W, class Default = void>
struct __std_pointer_type {
  using type = Default;
};
template <stl2::WeakIterator W, class Default>
  requires requires { typename W::pointer; }
struct __std_pointer_type<W, Default> {
  using type = typename W::pointer;
};

namespace detail {
namespace stl2_to_std_iterator_category_ {
  std::input_iterator_tag f(input_iterator_tag*);
  std::forward_iterator_tag f(forward_iterator_tag*);
  std::bidirectional_iterator_tag f(bidirectional_iterator_tag*);
  std::random_access_iterator_tag f(random_access_iterator_tag*);
}

template <class T>
using stl2_to_std_iterator_category =
  decltype(stl2_to_std_iterator_category_::f((T*)nullptr));
}}}

namespace std {
template <::stl2::WeakIterator Out>
  requires !::stl2::detail::MemberIteratorCategory<Out>
struct iterator_traits<Out> {
  using difference_type = ::stl2::DifferenceType<Out>;
  using value_type = meta::_t<::stl2::__std_out_value_type<Out>>;
  using reference = meta::_t<::stl2::__std_reference_type<Out>>;
  using pointer = meta::_t<::stl2::__std_pointer_type<Out>>;
  using iterator_category = std::output_iterator_tag;
};

template <::stl2::WeakInputIterator WI>
  requires ::stl2::DerivedFrom<typename WI::iterator_category,
                               ::stl2::weak_input_iterator_tag>() &&
    !::stl2::DerivedFrom<typename WI::iterator_category,
                               ::stl2::input_iterator_tag>()
struct iterator_traits<WI> {};

template <::stl2::InputIterator I>
  requires ::stl2::DerivedFrom<typename I::iterator_category,
                               ::stl2::input_iterator_tag>()
struct iterator_traits<I> {
  using difference_type = ::stl2::DifferenceType<I>;
  using value_type = ::stl2::ValueType<I>;
  using reference = meta::_t<
    ::stl2::__std_reference_type<I, ::stl2::ReferenceType<I>>>;
  using pointer = meta::_t<
    ::stl2::__std_pointer_type<I, typename ::stl2::iterator_traits<I>::pointer>>;
  using iterator_category = meta::if_<
    is_reference<::stl2::ReferenceType<I>>,
    ::stl2::detail::stl2_to_std_iterator_category<typename I::iterator_category>,
    std::input_iterator_tag>;
};
}

#endif
