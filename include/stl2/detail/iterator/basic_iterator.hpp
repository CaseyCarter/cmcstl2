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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>

// TODO:
// * noexcept
// * contiguous cursors

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
      requires !detail::MemberValueType<C> &&
        requires (const C& c) {
          STL2_DEDUCE_AUTO_REF_REF(c.current());
        }
    struct value_type<C> {
      using type = decay_t<decltype(declval<const C&>().current())>;
    };
    template <class C>
    using ValueType = meta::_t<value_type<C>>;

    template <class I>
      requires requires (I&& i) {
        STL2_DEDUCE_AUTO_REF_REF(((I&&)i).cursor());
      }
    static auto&& cursor(I&& i)
    STL2_NOEXCEPT_RETURN(__stl2::forward<I>(i).cursor())

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

    template <class Cursor, class Other>
      requires requires (const Cursor& lhs, const Other& rhs) {
        { lhs.equal(rhs) } -> bool;
      }
    static bool equal(const Cursor& lhs, const Other& rhs)
    STL2_NOEXCEPT_RETURN(bool(lhs.equal(rhs)))

    template <class Cursor>
      requires requires (Cursor& c, DifferenceType<Cursor> n) {
        c.advance(n);
      }
    static void advance(Cursor& c, DifferenceType<Cursor> n)
    STL2_NOEXCEPT_RETURN((void)c.advance(n))

    template <class Cursor, class Other>
      requires requires (const Cursor& lhs, const Other& rhs) {
        { lhs.distance_to(rhs) } -> DifferenceType<Cursor>;
      }
    static DifferenceType<Cursor> distance(const Cursor& lhs, const Other& rhs)
    STL2_NOEXCEPT_RETURN(DifferenceType<Cursor>(lhs.distance_to(rhs)))
  };

  namespace detail {
    template <class C>
    concept bool CursorCurrent = requires (C& c) {
      cursor_access::current(c);
    };
    template <class C>
    concept bool CursorNext = requires (C& c) {
      cursor_access::next(c);
    };
    template <class C>
    concept bool CursorPrev = requires (C& c) {
      cursor_access::prev(c);
    };
    template <class C, class O>
    concept bool CursorEqual =
      requires (const C& l, const O& r) {
        cursor_access::equal(l, r);
      };
    template <class C>
    concept bool CursorAdvance =
      requires (C& c, cursor_access::DifferenceType<C> n) {
        cursor_access::advance(c, n);
      };
    template <class C, class O>
    concept bool CursorDistance =
      requires (const C& l, const O& r) {
        cursor_access::distance(l, r);
      };
    template <class C, class T>
    concept bool CursorWrite =
      requires (C& c, T&& t) {
        cursor_access::write(c, __stl2::forward<T>(t));
      };

    template <class C>
    concept bool InputCursor =
      Semiregular<C>() && CursorCurrent<C> &&
      CursorNext<C> && requires {
        typename cursor_access::ValueType<C>;
      };
    template <class C>
    concept bool ForwardCursor =
      InputCursor<C> && CursorEqual<C, C>;
    template <class C>
    concept bool BidirectionalCursor =
      ForwardCursor<C> && CursorPrev<C>;
    template <class C>
    concept bool RandomAccessCursor =
      BidirectionalCursor<C> && CursorAdvance<C> && CursorDistance<C, C>;

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
  }

  template <Semiregular Cursor>
  class basic_iterator : public Cursor {
  public:
    friend cursor_access;
    using difference_type = cursor_access::DifferenceType<Cursor>;

    basic_iterator() = default;
    basic_iterator(Cursor c)
      noexcept(is_nothrow_move_constructible<Cursor>::value) :
      Cursor(__stl2::move(c)) {}

    using Cursor::Cursor;

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

  private:
    Cursor& cursor() & { return static_cast<Cursor&>(*this); }
    const Cursor& cursor() const& { return static_cast<const Cursor&>(*this); }
    Cursor&& cursor() && { return static_cast<Cursor&&>(*this); }
  };

  template <detail::InputCursor Cursor>
  class basic_iterator<Cursor> : public Cursor {
  public:
    friend cursor_access;
    using difference_type = cursor_access::DifferenceType<Cursor>;
    using value_type = cursor_access::ValueType<Cursor>;
    using iterator_category = detail::CursorCategory<Cursor>;

    basic_iterator() = default;
    basic_iterator(Cursor c)
      noexcept(is_nothrow_move_constructible<Cursor>::value) :
      Cursor(__stl2::move(c)) {}

    using Cursor::Cursor;

    decltype(auto) operator*() const
      noexcept(noexcept(cursor_access::current(declval<const Cursor&>())))
      requires detail::CursorCurrent<const Cursor> {
      return cursor_access::current(cursor());
    }

    basic_iterator& operator++() &
    noexcept(noexcept(cursor_access::next(declval<Cursor&>()))) {
      cursor_access::next(cursor());
      return *this;
    }

    basic_iterator operator++(int) &
      noexcept(is_nothrow_copy_constructible<basic_iterator>::value &&
               is_nothrow_move_constructible<basic_iterator>::value &&
               noexcept(cursor_access::next(declval<Cursor&>()))) {
      auto tmp = *this;
      cursor_access::next(cursor());
      return tmp;
    }

    basic_iterator& operator--() &
      noexcept(noexcept(cursor_access::prev(declval<Cursor&>())))
      requires detail::CursorPrev<Cursor> {
      cursor_access::prev(cursor());
      return *this;
    }
    basic_iterator operator--(int) &
      noexcept(is_nothrow_copy_constructible<basic_iterator>::value &&
               is_nothrow_move_constructible<basic_iterator>::value &&
               noexcept(cursor_access::prev(declval<Cursor&>())))
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

    decltype(auto) operator[](difference_type n) const
      requires detail::CursorAdvance<Cursor> {
      return *(*this + n);
    }

  private:
    Cursor& cursor() & { return static_cast<Cursor&>(*this); }
    const Cursor& cursor() const& { return static_cast<const Cursor&>(*this); }
    Cursor&& cursor() && { return static_cast<Cursor&&>(*this); }
  };

  template <class Cursor>
    requires detail::CursorEqual<Cursor, Cursor>
  constexpr bool operator==(const basic_iterator<Cursor>& lhs,
                            const basic_iterator<Cursor>& rhs)
  STL2_NOEXCEPT_RETURN(
    cursor_access::equal(cursor_access::cursor(lhs),
                         cursor_access::cursor(rhs))
  )

  template <class Cursor, class Other>
    requires detail::CursorEqual<Cursor, Other>
  constexpr bool operator==(const basic_iterator<Cursor>& lhs, const Other& rhs)
  STL2_NOEXCEPT_RETURN(
    cursor_access::equal(cursor_access::cursor(lhs), rhs)
  )

  template <class Cursor, class Other>
    requires detail::CursorEqual<Cursor, Other>
  constexpr bool operator==(const Other& lhs, const basic_iterator<Cursor>& rhs)
  STL2_NOEXCEPT_RETURN(
    cursor_access::equal(cursor_access::cursor(rhs), lhs)
  )

  template <class Cursor>
    requires detail::CursorEqual<Cursor, Cursor>
  constexpr bool operator!=(const basic_iterator<Cursor>& lhs,
                            const basic_iterator<Cursor>& rhs)
  STL2_NOEXCEPT_RETURN(
    !cursor_access::equal(cursor_access::cursor(lhs),
                          cursor_access::cursor(rhs))
  )

  template <class Cursor, class Other>
    requires detail::CursorEqual<Cursor, Other>
  constexpr bool operator!=(const basic_iterator<Cursor>& lhs, const Other& rhs)
  STL2_NOEXCEPT_RETURN(
    !cursor_access::equal(cursor_access::cursor(lhs), rhs)
  )

  template <class Cursor, class Other>
    requires detail::CursorEqual<Cursor, Other>
  constexpr bool operator!=(const Other& lhs, const basic_iterator<Cursor>& rhs)
  STL2_NOEXCEPT_RETURN(
    !cursor_access::equal(cursor_access::cursor(rhs), lhs)
  )

  template <class Cursor>
    requires detail::CursorDistance<Cursor, Cursor>
  constexpr cursor_access::DifferenceType<Cursor>
  operator-(const basic_iterator<Cursor>& lhs, const basic_iterator<Cursor>& rhs)
  STL2_NOEXCEPT_RETURN(
    cursor_access::distance(cursor_access::cursor(rhs),
                            cursor_access::cursor(lhs))
  )

  template <class Cursor, class Other>
    requires detail::CursorDistance<Cursor, Other>
  constexpr cursor_access::DifferenceType<Cursor>
  operator-(const basic_iterator<Cursor>& lhs, const Other& rhs)
  STL2_NOEXCEPT_RETURN(
    -cursor_access::distance(cursor_access::cursor(lhs), rhs)
  )

  template <class Cursor, class Other>
    requires detail::CursorDistance<Cursor, Other>
  constexpr cursor_access::DifferenceType<Cursor>
  operator-(const Other& lhs, const basic_iterator<Cursor>& rhs)
  STL2_NOEXCEPT_RETURN(
    cursor_access::distance(cursor_access::cursor(rhs), lhs)
  )

  template <class Cursor>
    requires detail::CursorDistance<Cursor, Cursor>
  constexpr bool operator<(const basic_iterator<Cursor>& lhs,
                           const basic_iterator<Cursor>& rhs)
  STL2_NOEXCEPT_RETURN(
    cursor_access::distance(cursor_access::cursor(rhs),
                            cursor_access::cursor(lhs)) > 0
  )

  template <class Cursor>
    requires detail::CursorEqual<Cursor, default_sentinel>
  constexpr bool operator<(const basic_iterator<Cursor>& lhs,
                           default_sentinel rhs)
  STL2_NOEXCEPT_RETURN(
    !cursor_access::equal(cursor_access::cursor(lhs), rhs)
  )

  template <class Cursor>
    requires detail::CursorEqual<Cursor, default_sentinel>
  constexpr bool operator<(default_sentinel,
                           const basic_iterator<Cursor>&) noexcept {
    return false;
  }

  template <class Cursor>
    requires detail::CursorDistance<Cursor, Cursor>
  constexpr bool operator>(const basic_iterator<Cursor>& lhs,
                           const basic_iterator<Cursor>& rhs)
  STL2_NOEXCEPT_RETURN(
    rhs < lhs
  )

  template <class Cursor>
    requires detail::CursorEqual<Cursor, default_sentinel>
  constexpr bool operator>(default_sentinel lhs,
                           const basic_iterator<Cursor>& rhs)
  STL2_NOEXCEPT_RETURN(
    !cursor_access::equal(cursor_access::cursor(rhs), lhs)
  )

  template <class Cursor>
    requires detail::CursorEqual<Cursor, default_sentinel>
  constexpr bool operator>(const basic_iterator<Cursor>&,
                           default_sentinel) noexcept {
    return false;
  }

  template <class Cursor>
    requires detail::CursorDistance<Cursor, Cursor>
  constexpr bool operator<=(const basic_iterator<Cursor>& lhs,
                            const basic_iterator<Cursor>& rhs)
  STL2_NOEXCEPT_RETURN(
    !(rhs < lhs)
  )

  template <class Cursor>
    requires detail::CursorEqual<Cursor, default_sentinel>
  constexpr bool operator<=(default_sentinel lhs,
                            const basic_iterator<Cursor>& rhs)
  STL2_NOEXCEPT_RETURN(
    cursor_access::equal(cursor_access::cursor(rhs), lhs)
  )

  template <class Cursor>
    requires detail::CursorEqual<Cursor, default_sentinel>
  constexpr bool operator<=(const basic_iterator<Cursor>&,
                            default_sentinel) noexcept {
    return true;
  }

  template <class Cursor>
    requires detail::CursorDistance<Cursor, Cursor>
  constexpr bool operator>=(const basic_iterator<Cursor>& lhs,
                            const basic_iterator<Cursor>& rhs)
  STL2_NOEXCEPT_RETURN(
    !(lhs < rhs)
  )

  template <class Cursor>
    requires detail::CursorEqual<Cursor, default_sentinel>
  constexpr bool operator>=(default_sentinel,
                            const basic_iterator<Cursor>&) noexcept {
    return true;
  }

  template <class Cursor>
    requires detail::CursorEqual<Cursor, default_sentinel>
  constexpr bool operator>=(const basic_iterator<Cursor>& lhs,
                            default_sentinel rhs)
  STL2_NOEXCEPT_RETURN(
    cursor_access::equal(cursor_access::cursor(lhs), rhs)
  )
} STL2_CLOSE_NAMESPACE

#endif
