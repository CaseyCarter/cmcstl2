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
#ifndef STL2_DETAIL_ITERATOR_BASIC_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_BASIC_ITERATOR_HPP

#include <stl2/detail/cheap_storage.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>

// TODO:
// * contiguous iterators?

STL2_OPEN_NAMESPACE {
  struct cursor_access {
    template <class>
    struct difference_type {
      using type = std::ptrdiff_t;
    };
    template <detail::MemberDifferenceType C>
    struct difference_type<C> {
      using type = typename C::difference_type;
    };
    template <class C>
      requires !detail::MemberDifferenceType<C> &&
        requires (const C& lhs, const C& rhs) {
          rhs.distance_to(lhs);
        }
    struct difference_type<C> {
      using type = decltype(declval<const C&>().distance_to(declval<const C&>()));
    };
    template <class C>
    using DifferenceType = meta::_t<difference_type<C>>;

    template <class C>
    struct value_type {};
    template <detail::MemberValueType C>
    struct value_type<C> {
      using type = typename C::value_type;
    };
    template <class C>
    using ValueType = meta::_t<value_type<C>>;

    template <class Cursor>
      requires requires (Cursor& c) {
        c.current();
      }
    static decltype(auto) current(Cursor& c)
    STL2_NOEXCEPT_RETURN(c.current())

    template <class Cursor, class T>
      requires requires (Cursor& c, T&& t) {
        c.write(__stl2::forward<T>(t));
      }
    static void write(Cursor& c, T&& t)
    STL2_NOEXCEPT_RETURN((void)c.write(__stl2::forward<T>(t)))

    template <class Cursor>
      requires requires (Cursor& c) {
        c.next();
      }
    static void next(Cursor& c)
    STL2_NOEXCEPT_RETURN((void)c.next())

    template <class Cursor>
      requires requires (Cursor& c) {
        c.prev();
      }
    static void prev(Cursor& c)
    STL2_NOEXCEPT_RETURN((void)c.prev())

    template <class Cursor>
      requires requires (const Cursor& c) {
        { c.done() } -> bool;
      }
    static bool done(const Cursor& c)
    STL2_NOEXCEPT_RETURN(bool(c.done()))

    template <class Cursor>
      requires requires (const Cursor& lhs, const Cursor& rhs) {
        { lhs.equal(rhs) } -> bool;
      }
    static bool equal(const Cursor& lhs, const Cursor& rhs)
    STL2_NOEXCEPT_RETURN(bool(lhs.equal(rhs)))

    template <class Cursor>
      requires requires (Cursor& c, DifferenceType<Cursor> n) {
        c.advance(n);
      }
    static void advance(Cursor& c, DifferenceType<Cursor> n)
    STL2_NOEXCEPT_RETURN((void)c.advance(n))

    template <class Cursor>
      requires requires (const Cursor& lhs, const Cursor& rhs) {
        { lhs.distance_to(rhs) } -> DifferenceType<Cursor>;
      }
    static DifferenceType<Cursor> distance(const Cursor& lhs, const Cursor& rhs)
    STL2_NOEXCEPT_RETURN(DifferenceType<Cursor>(lhs.distance_to(rhs)))
  };

  namespace detail {
    template <class C>
    concept bool CursorCurrent = requires (C& c) {
      cursor_access::current(c);
    };
    template <class C>
    concept bool CursorDone = requires (const C& c) {
      cursor_access::done(c);
    };
    template <class C>
    concept bool CursorNext = requires (C& c) {
      cursor_access::next(c);
    };
    template <class C>
    concept bool CursorPrev = requires (C& c) {
      cursor_access::prev(c);
    };
    template <class C>
    concept bool CursorEqual = requires (const C& l, const C& r) {
      cursor_access::equal(l, r);
    };
    template <class C>
    concept bool CursorAdvance = requires (C& c, cursor_access::DifferenceType<C> n) {
      cursor_access::advance(c, n);
    };
    template <class C>
    concept bool CursorDistance = requires (const C& l, const C& r) {
      cursor_access::distance(l, r);
    };
    template <class C, class T>
    concept bool CursorWrite = requires (C& c, T&& t) {
      cursor_access::write(c, __stl2::forward<T>(t));
    };

    template <class C>
    concept bool InputCursor =
      Semiregular<C>() && CursorCurrent<C> && CursorNext<C> && requires {
        typename cursor_access::ValueType<C>;
      };
    template <class C>
    concept bool ForwardCursor =
      InputCursor<C> && CursorEqual<C>;
    template <class C>
    concept bool BidirectionalCursor =
      ForwardCursor<C> && CursorPrev<C>;
    template <class C>
    concept bool RandomAccessCursor =
      BidirectionalCursor<C> && CursorAdvance<C> && CursorDistance<C>;

    template <class>
    struct cursor_category {};
    template <InputCursor C>
    struct cursor_category<C> {
      using type = weak_input_iterator_tag;
    };
    template <ForwardCursor C>
    struct cursor_category<C> {
      using type = forward_iterator_tag;
    };
    template <BidirectionalCursor C>
    struct cursor_category<C> {
      using type = bidirectional_iterator_tag;
    };
    template <RandomAccessCursor C>
    struct cursor_category<C> {
      using type = random_access_iterator_tag;
    };
    template <class C>
    using CursorCategory = meta::_t<cursor_category<C>>;

    template <Semiregular Cursor>
    class basic_iterator_base : ebo_box<Cursor> {
    public:
      using difference_type = cursor_access::DifferenceType<Cursor>;
      basic_iterator_base() = default;
      using ebo_box<Cursor>::ebo_box;

      basic_iterator_base(const basic_iterator_base&) = default;
      basic_iterator_base(basic_iterator_base&&) = default;
      basic_iterator_base& operator=(const basic_iterator_base&) & = default;
      basic_iterator_base& operator=(basic_iterator_base&&) & = default;

      friend bool operator==(const basic_iterator_base& i, default_sentinel)
        noexcept(noexcept(cursor_access::done(i.cursor())))
        requires detail::CursorDone<Cursor> {
        return cursor_access::done(i.cursor());
      }
      friend bool operator==(default_sentinel, const basic_iterator_base& i)
        noexcept(noexcept(cursor_access::done(i.cursor())))
        requires detail::CursorDone<Cursor> {
        return cursor_access::done(i.cursor());
      }
      friend bool operator!=(const basic_iterator_base& i, default_sentinel)
        noexcept(noexcept(cursor_access::done(i.cursor())))
        requires detail::CursorDone<Cursor> {
        return !cursor_access::done(i.cursor());
      }
      friend bool operator!=(default_sentinel, const basic_iterator_base& i)
        noexcept(noexcept(cursor_access::done(i.cursor())))
        requires detail::CursorDone<Cursor> {
        return !cursor_access::done(i.cursor());
      }

      friend bool operator<(const basic_iterator_base& i, default_sentinel)
        noexcept(noexcept(cursor_access::done(i.cursor())))
        requires detail::CursorDone<Cursor> {
        return !cursor_access::done(i.cursor());
      }
      friend bool operator<(default_sentinel, const basic_iterator_base&)
        noexcept requires detail::CursorDone<Cursor> {
        return false;
      }

      friend bool operator>(default_sentinel, const basic_iterator_base& i)
        noexcept(noexcept(cursor_access::done(i.cursor())))
        requires detail::CursorDone<Cursor> {
        return !cursor_access::done(i.cursor());
      }
      friend bool operator>(const basic_iterator_base&, default_sentinel)
        noexcept requires detail::CursorDone<Cursor> {
        return false;
      }

      friend bool operator<=(default_sentinel, const basic_iterator_base& i)
        noexcept(noexcept(cursor_access::done(i.cursor())))
        requires detail::CursorDone<Cursor> {
        return cursor_access::done(i.cursor());
      }
      friend bool operator<=(const basic_iterator_base&, default_sentinel)
        noexcept requires detail::CursorDone<Cursor> {
        return true;
      }

      friend bool operator>=(const basic_iterator_base& i, default_sentinel)
        noexcept(noexcept(cursor_access::done(i.cursor())))
        requires detail::CursorDone<Cursor> {
        return cursor_access::done(i.cursor());
      }
      friend bool operator>=(default_sentinel, const basic_iterator_base&)
        noexcept requires detail::CursorDone<Cursor> {
        return true;
      }

      friend bool operator==(const basic_iterator_base& lhs, const basic_iterator_base& rhs)
        noexcept(noexcept(cursor_access::equal(lhs.cursor(), rhs.cursor())))
        requires detail::CursorEqual<Cursor> {
        return cursor_access::equal(lhs.cursor(), rhs.cursor());
      }
      friend bool operator!=(const basic_iterator_base& lhs, const basic_iterator_base& rhs)
        noexcept(noexcept(cursor_access::equal(lhs.cursor(), rhs.cursor())))
        requires detail::CursorEqual<Cursor> {
        return !cursor_access::equal(lhs.cursor(), rhs.cursor());
      }

      friend bool operator<(const basic_iterator_base& lhs, const basic_iterator_base& rhs)
        noexcept(noexcept(cursor_access::distance(rhs.cursor(), lhs.cursor())))
        requires detail::CursorDistance<Cursor> {
        return cursor_access::distance(rhs.cursor(), lhs.cursor()) > difference_type{0};
      }
      friend bool operator>(const basic_iterator_base& lhs, const basic_iterator_base& rhs)
        noexcept(noexcept(rhs < lhs))
        requires detail::CursorDistance<Cursor> {
        return rhs < lhs;
      }
      friend bool operator<=(const basic_iterator_base& lhs, const basic_iterator_base& rhs)
        noexcept(noexcept(rhs < lhs))
        requires detail::CursorDistance<Cursor> {
        return !(rhs < lhs);
      }
      friend bool operator>=(const basic_iterator_base& lhs, const basic_iterator_base& rhs)
        noexcept(noexcept(lhs < rhs))
        requires detail::CursorDistance<Cursor> {
        return !(lhs < rhs);
      }

    protected:
      ~basic_iterator_base() = default;

      Cursor& cursor() & { return ebo_box<Cursor>::get(); }
      const Cursor& cursor() const& { return ebo_box<Cursor>::get(); }
      Cursor&& cursor() && { return ebo_box<Cursor>::get(); }
    };
  }

  template <Semiregular Cursor>
  class basic_iterator :
    public detail::basic_iterator_base<Cursor> {
    using base_t = detail::basic_iterator_base<Cursor>;
    using base_t::cursor;
  public:
    using difference_type = typename base_t::difference_type;

    using base_t::base_t;

    basic_iterator& operator*() {
      return *this;
    }
    basic_iterator& operator++() & {
      return *this;
    }
    basic_iterator operator++(int) & {
      return *this;
    }

    template <class T>
      requires !Same<decay_t<T>, basic_iterator>() &&
        detail::CursorWrite<Cursor, T>
    basic_iterator& operator=(T&& t) & {
      cursor_access::write(cursor(), __stl2::forward<T>(t));
      return *this;
    }
  };

  template <detail::InputCursor Cursor>
  class basic_iterator<Cursor> :
    public detail::basic_iterator_base<Cursor> {
    using base_t = detail::basic_iterator_base<Cursor>;
    using base_t::cursor;
  public:
    using difference_type = typename base_t::difference_type;
    using value_type = cursor_access::ValueType<Cursor>;
    using iterator_category = detail::CursorCategory<Cursor>;

    using base_t::base_t;

    decltype(auto) operator*() const
      noexcept(noexcept(cursor_access::current(declval<const Cursor&>())))
      requires detail::CursorCurrent<const Cursor> {
      return cursor_access::current(cursor());
    }

    basic_iterator& operator++() & {
      cursor_access::next(cursor());
      return *this;
    }
    basic_iterator operator++(int) & {
      auto tmp = *this;
      cursor_access::next(cursor());
      return tmp;
    }

    basic_iterator& operator--() &
      requires detail::CursorPrev<Cursor> {
      cursor_access::prev(cursor());
      return *this;
    }
    basic_iterator operator--(int) &
      requires detail::CursorPrev<Cursor> {
      auto tmp = *this;
      cursor_access::prev(cursor());
      return *this;
    }

    basic_iterator& operator+=(difference_type n) &
      requires detail::CursorAdvance<Cursor> {
      cursor_access::advance(cursor(), n);
      return *this;
    }
    basic_iterator& operator-=(difference_type n) &
      requires detail::CursorAdvance<Cursor> {
      cursor_access::advance(cursor(), -n);
      return *this;
    }

    friend basic_iterator operator+(const basic_iterator& i, difference_type n)
      requires detail::CursorAdvance<Cursor> {
      auto tmp = i;
      cursor_access::advance(tmp.cursor(), n);
      return tmp;
    }
    friend basic_iterator operator+(difference_type n, const basic_iterator& i)
      requires detail::CursorAdvance<Cursor> {
      return i + n;
    }
    friend basic_iterator operator-(const basic_iterator& i, difference_type n)
      requires detail::CursorAdvance<Cursor> {
      return i + -n;
    }

    friend difference_type operator-(const basic_iterator& lhs, const basic_iterator& rhs)
      requires detail::CursorDistance<Cursor> {
      return cursor_access::distance(rhs.cursor(), lhs.cursor());
    }

    decltype(auto) operator[](difference_type n) const
      requires detail::CursorAdvance<Cursor> {
      return *(*this + n);
    }
  };
} STL2_CLOSE_NAMESPACE

#endif
