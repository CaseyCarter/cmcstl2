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
#ifndef STL2_DETAIL_ITERATOR_INSERT_ITERATORS_HPP
#define STL2_DETAIL_ITERATOR_INSERT_ITERATORS_HPP

#include <cstddef>
#include <memory>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
  template <class Derived, detail::MemberValueType Container>
  class __insert_iterator_base {
  public:
    using container_type = Container;
    using difference_type = std::ptrdiff_t;
    using iterator_category = output_iterator_tag;

    // Extension
    using value_type = typename Container::value_type;

    constexpr __insert_iterator_base() = default;
    STL2_CONSTEXPR_EXT explicit
    __insert_iterator_base(Container& x) noexcept :
      container_{std::addressof(x)} {}

    Derived& operator*() noexcept {
      return derived();
    }

    Derived& operator++() & noexcept {
      return derived();
    }

    Derived operator++(int) &
      noexcept(is_nothrow_copy_constructible<Derived>::value)
    {
      return derived();
    }

  protected:
    detail::raw_ptr<Container> container_ = nullptr;

  private:
    static constexpr void check() noexcept {
      static_assert(models::DerivedFrom<Derived, __insert_iterator_base>);
    }
    constexpr Derived& derived() noexcept {
      check();
      return static_cast<Derived&>(*this);
    }
    constexpr const Derived& derived() const noexcept {
      check();
      return static_cast<const Derived&>(*this);
    }
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

  template <detail::MemberValueType Container>
  class back_insert_iterator :
    public __insert_iterator_base<back_insert_iterator<Container>, Container> {
    using __base_t = __insert_iterator_base<back_insert_iterator<Container>, Container>;
    using __base_t::container_;
  public:
    using typename __base_t::value_type;

    using __base_t::__base_t;
    constexpr back_insert_iterator() = default;

    back_insert_iterator&
    operator=(const value_type& value) &
      requires detail::back_insertable<Container, const value_type&> {
      container_->push_back(value);
      return *this;
    }

    back_insert_iterator&
    operator=(value_type&& value) &
      requires detail::back_insertable<Container, value_type&&> {
      container_->push_back(__stl2::move(value));
      return *this;
    }
  };

  template <detail::MemberValueType Container>
  auto back_inserter(Container& x) noexcept {
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

  template <detail::MemberValueType Container>
  class front_insert_iterator :
    public __insert_iterator_base<front_insert_iterator<Container>, Container> {
    using __base_t = __insert_iterator_base<front_insert_iterator<Container>, Container>;
    using __base_t::container_;
  public:
    using typename __base_t::value_type;

    using __base_t::__base_t;
    constexpr front_insert_iterator() = default;

    front_insert_iterator&
    operator=(const value_type& value) &
      requires detail::front_insertable<Container, const value_type&> {
      container_->push_front(value);
      return *this;
    }

    front_insert_iterator&
    operator=(value_type&& value) &
      requires detail::front_insertable<Container, value_type&&> {
      container_->push_front(__stl2::move(value));
      return *this;
    }
  };

  template <detail::MemberValueType Container>
  auto front_inserter(Container& x) noexcept {
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

  template <detail::MemberValueType Container>
    requires requires { typename Container::iterator; }
  class insert_iterator :
    public __insert_iterator_base<insert_iterator<Container>, Container> {
  private:
    using __base_t = __insert_iterator_base<insert_iterator<Container>, Container>;
    using __base_t::container_;
    using I = typename Container::iterator;
    I iter_;

  public:
    using typename __base_t::value_type;

    insert_iterator() = default;
    STL2_CONSTEXPR_EXT explicit insert_iterator(Container& x, I i)
      noexcept(is_nothrow_move_constructible<I>::value) :
      __base_t{x}, iter_{__stl2::move(i)} {}

    insert_iterator&
    operator=(const value_type& value) &
      requires detail::insertable<Container, const value_type&> {
      iter_ = container_->insert(iter_, value);
      ++iter_;
      return *this;
    }

    insert_iterator&
    operator=(value_type&& value) &
      requires detail::insertable<Container, value_type&&> {
      iter_ = container_->insert(iter_, __stl2::move(value));
      ++iter_;
      return *this;
    }
  };

  template <detail::MemberValueType Container,
            class I = typename Container::iterator>
  auto inserter(Container& x, I i)
    noexcept(is_nothrow_move_constructible<I>::value) {
    return insert_iterator<Container>{x, __stl2::move(i)};
  }
} STL2_CLOSE_NAMESPACE

#endif
