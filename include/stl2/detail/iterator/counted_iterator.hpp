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
#ifndef STL2_DETAIL_ITERATOR_COUNTED_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_COUNTED_ITERATOR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>
#include <stl2/detail/iterator/operations.hpp>

STL2_OPEN_NAMESPACE {
  namespace __counted_iterator {
    Iterator{I} class cursor;
  }

  Iterator{I}
  using counted_iterator = basic_iterator<__counted_iterator::cursor<I>>;

  namespace __counted_iterator {
    struct access {
      template <class CC>
      requires meta::is<__uncvref<CC>, cursor>::value
      static constexpr decltype(auto) base(CC&& cc) noexcept {
        return __stl2::forward<CC>(cc).get();
      }
      template <class CC>
      requires meta::is<__uncvref<CC>, cursor>::value
      static constexpr decltype(auto) count(CC&& cc) noexcept {
        return (__stl2::forward<CC>(cc).count_);
      }

      template <class CI>
      requires meta::is<__uncvref<CI>, counted_iterator>::value
      static constexpr decltype(auto) base(CI&& ci) noexcept {
        return access::base(__stl2::get_cursor(__stl2::forward<CI>(ci)));
      }
      template <class CI>
      requires meta::is<__uncvref<CI>, counted_iterator>::value
      static constexpr decltype(auto) count(CI&& ci) noexcept {
        return access::count(__stl2::get_cursor(__stl2::forward<CI>(ci)));
      }
    };

    Iterator{I}
    class cursor : detail::ebo_box<I> {
    public:
      friend access;
      using difference_type = difference_type_t<I>;
      using single_pass = meta::bool_<!models::ForwardIterator<I>>;
      using contiguous = meta::bool_<models::ContiguousIterator<I>>;

    private:
      using box_t = detail::ebo_box<I>;
      using box_t::get;
      difference_type count_;

    public:
      struct mixin : protected detail::ebo_box<cursor> {
        using iterator_type = I;
        using difference_type = cursor::difference_type;

        mixin() = default;
        using mixin::ebo_box::ebo_box;

        STL2_CONSTEXPR_EXT I base() const
        noexcept(is_nothrow_copy_constructible<I>::value)
        {
          return mixin::ebo_box::get().get();
        }
        STL2_CONSTEXPR_EXT difference_type count() const noexcept {
          return mixin::ebo_box::get().count_;
        }
      };

      STL2_CONSTEXPR_EXT cursor()
      noexcept(is_nothrow_default_constructible<I>::value)
      : box_t{}, count_{0}
      {}
      STL2_CONSTEXPR_EXT cursor(I&& i, difference_type n)
      noexcept(is_nothrow_move_constructible<I>::value)
      : box_t(__stl2::move(i)), count_{n}
      {
        STL2_ASSUME_CONSTEXPR(0 <= n);
      }
      STL2_CONSTEXPR_EXT cursor(const I& i, difference_type n)
      noexcept(is_nothrow_copy_constructible<I>::value)
      : box_t(i), count_{n}
      {}
      template <ConvertibleTo<I> O>
      STL2_CONSTEXPR_EXT cursor(cursor<O>&& that)
      noexcept(is_nothrow_constructible<I, O&&>::value)
        : box_t(access::base(__stl2::move(that))), count_{access::count(that)}
      {}
      template <ConvertibleTo<I> O>
      STL2_CONSTEXPR_EXT cursor(const cursor<O>& that)
      noexcept(is_nothrow_constructible<I, const O&>::value)
        : box_t(access::base(that)), count_{access::count(that)}
      {}
      // Possibly ill-conceived Extension
      STL2_CONSTEXPR_EXT cursor(default_sentinel)
      noexcept(is_nothrow_default_constructible<I>::value)
      requires !models::BidirectionalIterator<I>
      : cursor{}
      {}

      STL2_CONSTEXPR_EXT decltype(auto) read() const
      noexcept(noexcept(*declval<const I&>()))
      requires InputIterator<I>()
      {
        return *get();
      }

      // FIXME: test
      // Extension
      STL2_CONSTEXPR_EXT decltype(auto) move() const
      noexcept(noexcept(__stl2::iter_move(declval<const I&>())))
      requires InputIterator<I>()
      {
        return __stl2::iter_move(get());
      }

#if 0 // FIXME
      // Extension
      friend STL2_CONSTEXPR_EXT void iter_swap(
        const counted_iterator& x, const counted_iterator& y)
      noexcept(noexcept(__stl2::iter_swap(x.get(), y.get())))
      requires IndirectlySwappable<I, I>()
      {
        __stl2::iter_swap(x.get(), y.get());
      }
#endif

      template <class T>
      requires
        !InputIterator<I>() &&
        Writable<I, T&&>()
      STL2_CONSTEXPR_EXT auto write(T&& t)
      noexcept(noexcept(*declval<I&>() = static_cast<T&&>(t)))
      {
        return *get() = static_cast<T&&>(t);
      }

      STL2_CONSTEXPR_EXT auto equal(default_sentinel) const noexcept {
        return count_ == 0;
      }
      STL2_CONSTEXPR_EXT auto equal(
        const cursor<Common<I> >& that) const noexcept
      {
        return count_ == access::count(that);
      }

      STL2_CONSTEXPR_EXT void next()
      noexcept(noexcept(++declval<I&>()))
      {
        STL2_ASSUME_CONSTEXPR(0 < count_);
        ++get();
        --count_;
      }

      STL2_CONSTEXPR_EXT auto prev()
      noexcept(noexcept(--declval<I&>()))
      requires BidirectionalIterator<I>()
      {
        --get();
        ++count_;
      }

      STL2_CONSTEXPR_EXT auto advance(difference_type n)
      noexcept(noexcept(declval<I&>() += n))
      requires RandomAccessIterator<I>()
      {
        STL2_ASSUME_CONSTEXPR(n <= count_);
        get() += n;
        count_ -= n;
      }

      STL2_CONSTEXPR_EXT auto distance_to(
        const cursor<Common<I> >& that) const noexcept
      {
        return count_ - access::count(that);
      }
      STL2_CONSTEXPR_EXT auto distance_to(default_sentinel) const noexcept {
        return count_;
      }
    };
  }

  Iterator{I}
  STL2_CONSTEXPR_EXT auto make_counted_iterator(I i, difference_type_t<I> n)
  STL2_NOEXCEPT_RETURN(
    counted_iterator<I>{__stl2::move(i), n}
  )

  Iterator{I}
  STL2_CONSTEXPR_EXT void advance(counted_iterator<I>& i, difference_type_t<I> n)
  noexcept(is_nothrow_copy_constructible<I>::value &&
           is_nothrow_move_assignable<I>::value &&
           noexcept(__stl2::next(declval<const I&>(), n)))
  {
    STL2_ASSUME_CONSTEXPR(n <= i.count());
    __counted_iterator::access::base(i) =
      __stl2::next(__counted_iterator::access::base(i), n);
    __counted_iterator::access::count(i) -= n;
  }

  RandomAccessIterator{I}
  STL2_CONSTEXPR_EXT void advance(counted_iterator<I>& i, difference_type_t<I> n)
  STL2_NOEXCEPT_RETURN(
    (void)(STL2_ASSUME_CONSTEXPR(n <= i.count()), i += n)
  )

  namespace ext {
    Iterator{I}
    constexpr auto uncounted(const I& i)
    noexcept(is_nothrow_copy_constructible<I>::value)
    {
      return i;
    }

    constexpr auto uncounted(const counted_iterator<auto>& i)
    STL2_NOEXCEPT_RETURN(
      i.base()
    )

    Iterator{I}
    constexpr auto recounted(const I&, const I& i, difference_type_t<I> = 0)
    noexcept(is_nothrow_copy_constructible<I>::value)
    {
      return i;
    }

    template <class I>
    constexpr auto recounted(const counted_iterator<I>& o, I i)
    STL2_NOEXCEPT_RETURN(
      counted_iterator<I>{__stl2::move(i), o.count()}
    )

    template <class I>
    constexpr auto recounted(
      const counted_iterator<I>& o, I i, difference_type_t<I> n)
    noexcept(noexcept(counted_iterator<I>{__stl2::move(i), o.count() - n}))
    {
      STL2_EXPENSIVE_ASSERT(!models::ForwardIterator<I> ||
                            i == __stl2::next(o.base(), n));
      return counted_iterator<I>{__stl2::move(i), o.count() - n};
    }
  }
} STL2_CLOSE_NAMESPACE

#endif
