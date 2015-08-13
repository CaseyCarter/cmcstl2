#ifndef STL2_DETAIL_ITERATOR_INSERT_ITERATORS_HPP
#define STL2_DETAIL_ITERATOR_INSERT_ITERATORS_HPP

#include <cstddef>
#include <memory>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>

namespace stl2 { inline namespace v1 {

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

}} // namespace stl2::v1

#endif
