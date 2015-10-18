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
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
  namespace detail {
    template <class Container>
      requires MemberValueType<Container>
    class insert_cursor_base {
    public:
      using container_type = Container;

      constexpr insert_cursor_base() noexcept = default;
      explicit insert_cursor_base(Container& x) noexcept :
        container{std::addressof(x)} {}

    protected:
      Container* container = nullptr;

      Container& get() {
        STL2_ASSUME(container);
        return *container;
      }
    };
  }

  ///////////////////////////////////////////////////////////////////////////
  // back_insert_iterator [back.insert.iterator]
  //
  namespace detail {
    template <class C, class V>
    concept bool BackInsertable =
      requires (C& c, V&& v) {
        c.push_back((V&&)v);
      };

    template <class Container>
      requires MemberValueType<Container>
    class back_insert_cursor : public insert_cursor_base<Container> {
      using base_t = insert_cursor_base<Container>;
    public:
      back_insert_cursor() = default;
      using base_t::base_t;

    private:
      friend cursor_access;
      template <class T>
        requires BackInsertable<Container, T>
      constexpr void write(T&& t)
        noexcept(noexcept(
          __stl2::declval<Container&>().push_back(__stl2::forward<T>(t)))) {
        base_t::get().push_back(__stl2::forward<T>(t));
      }
    };
  }

  template <class Container>
  using back_insert_iterator =
    basic_iterator<detail::back_insert_cursor<Container>>;

  template <class Container>
    requires detail::MemberValueType<Container>
  auto back_inserter(Container& c) noexcept {
    return back_insert_iterator<Container>{c};
  }

  ///////////////////////////////////////////////////////////////////////////
  // front_insert_iterator [front.insert.iterator]
  //
  namespace detail {
    template <class C, class V>
    concept bool FrontInsertable =
      requires (C& c, V&& v) {
        c.push_front((V&&)v);
      };

    template <class Container>
      requires MemberValueType<Container>
    class front_insert_cursor : public insert_cursor_base<Container> {
      using base_t = insert_cursor_base<Container>;
    public:
      front_insert_cursor() = default;
      using base_t::base_t;

    private:
      friend cursor_access;
      template <class T>
        requires FrontInsertable<Container, T>
      constexpr void write(T&& t)
        noexcept(noexcept(
          __stl2::declval<Container&>().push_front(__stl2::forward<T>(t)))) {
        base_t::get().push_front(__stl2::forward<T>(t));
      }
    };
  }

  template <class Container>
  using front_insert_iterator =
    basic_iterator<detail::front_insert_cursor<Container>>;

  template <class Container>
    requires detail::MemberValueType<Container>
  auto front_inserter(Container& x) noexcept {
    return front_insert_iterator<Container>{x};
  }

  ///////////////////////////////////////////////////////////////////////////
  // insert_iterator [insert.iterator]
  //
  namespace detail {
    template <class C, class V>
    concept bool Insertable =
      requires (C& c, typename C::iterator i, V&& v) {
        {  c.insert(i, (V&&)v) } -> typename C::iterator;
      };

    template <class Container>
      requires MemberValueType<Container> &&
        requires { typename Container::iterator; }
    class insert_cursor : public insert_cursor_base<Container> {
      using base_t = insert_cursor_base<Container>;
    public:
      // Extension
      using iterator_type = typename Container::iterator;

      insert_cursor() = default;
      explicit insert_cursor(Container& x, iterator_type i)
        noexcept(is_nothrow_move_constructible<iterator_type>::value) :
        base_t{x}, iter{__stl2::move(i)} {}

    protected:
      iterator_type iter{};

    private:
      template <class T>
        requires Insertable<Container, T>
      void write(T&& t) {
        iter = base_t::get().insert(iter, __stl2::forward<T>(t));
        ++iter;
      }
    };
  }

  template <class Container>
  using insert_iterator =
    basic_iterator<detail::insert_cursor<Container>>;

  template <class Container, class I = typename Container::iterator>
    requires detail::MemberValueType<Container>
  auto inserter(Container& x, I i)
  STL2_NOEXCEPT_RETURN(insert_iterator<Container>{x, __stl2::move(i)})
} STL2_CLOSE_NAMESPACE

#endif
