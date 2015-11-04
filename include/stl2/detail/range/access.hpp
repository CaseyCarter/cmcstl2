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
#ifndef STL2_DETAIL_RANGE_ACCESS_HPP
#define STL2_DETAIL_RANGE_ACCESS_HPP

#include <initializer_list>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>

// Backwards compatibility: rvalues are treated as const lvalues.
// They should probably be ill-formed.
#define STL2_TREAT_RVALUES_AS_CONST 1

///////////////////////////////////////////////////////////////////////////
// Range access [iterator.range]
//
STL2_OPEN_NAMESPACE {
  // begin
  // 20150805: Not to spec: is an N4381-style customization point.
  namespace __begin {
    // Poison pill for std::begin. (See the detailed discussion at
    // https://github.com/ericniebler/stl2/issues/139)
    void begin(auto&) = delete;

    template <class R>
    constexpr bool has_member = false;
    template <class R>
      requires requires (R& r) {
        STL2_DEDUCTION_CONSTRAINT(r.begin(), Iterator);
      }
    constexpr bool has_member<R> = true;

    template <class R>
    constexpr bool has_non_member = false;
    template <class R>
      requires requires (R& r) {
        STL2_DEDUCTION_CONSTRAINT(begin(r), Iterator);
      }
    constexpr bool has_non_member<R> = true;

    struct fn {
      template <class R, std::size_t N>
      constexpr R* operator()(R (&array)[N]) const noexcept {
        return array;
      }

      template <class E>
      constexpr const E* operator()(std::initializer_list<E> il) const noexcept {
        return il.begin();
      }

      // Prefer member if it returns Iterator.
      template <class R>
        requires has_member<R>
      constexpr STL2_CONSTRAINED_RETURN(Iterator)
      operator()(R& r) const
      STL2_NOEXCEPT_RETURN(r.begin())

      // Use ADL if it returns Iterator.
      template <class R>
        requires !has_member<R> && has_non_member<R>
      constexpr STL2_CONSTRAINED_RETURN(Iterator)
      operator()(R& r) const
      STL2_NOEXCEPT_RETURN(begin(r))

#if STL2_TREAT_RVALUES_AS_CONST
      template <_IsNot<is_array> R>
      [[deprecated]] constexpr STL2_CONSTRAINED_RETURN(Iterator)
      operator()(const R&& r) const
        noexcept(noexcept(declval<const fn&>()(r))) 
        requires has_member<const R> || has_non_member<const R>
      {
        return (*this)(r);
      }
#endif
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& begin = detail::static_const<__begin::fn>::value;
  }

  // end
  namespace __end {
    // Poison pill for std::end. (See the detailed discussion at
    // https://github.com/ericniebler/stl2/issues/139)
    void end(auto&) = delete;

    template <class R>
    constexpr bool has_member = false;
    template <class R>
      requires requires (R& r) {
        requires Sentinel<decltype(r.end()), decltype(__stl2::begin(r))>();
      }
    constexpr bool has_member<R> = true;

    template <class R>
    constexpr bool has_non_member = false;
    template <class R>
      requires requires (R& r) {
        // { end(r) } -> Sentinel<decltype(__stl2::begin(r))>;
        requires Sentinel<decltype(end(r)), decltype(__stl2::begin(r))>();
      }
    constexpr bool has_non_member<R> = true;

    struct fn {
      template <class R, std::size_t N>
      constexpr R* operator()(R (&array)[N]) const noexcept {
        return array + N;
      }

      template <class E>
      constexpr const E* operator()(std::initializer_list<E> il) const noexcept {
        return il.end();
      }

      // Prefer member if it returns Sentinel.
      template <class R>
        requires has_member<R>
      constexpr auto operator()(R& r) const
      STL2_NOEXCEPT_RETURN(r.end())

      // Use ADL if it returns Sentinel.
      template <class R>
        requires !has_member<R> && has_non_member<R>
      constexpr auto operator()(R& r) const
      STL2_NOEXCEPT_RETURN(end(r))

#if STL2_TREAT_RVALUES_AS_CONST
      template <_IsNot<is_array> R>
      [[deprecated]] constexpr auto operator()(const R&& r) const
        noexcept(noexcept(declval<const fn&>()(r)))
        requires has_member<const R> || has_non_member<const R>
      {
        return (*this)(r);
      }
#endif
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& end = detail::static_const<__end::fn>::value;
  }

  // cbegin
  namespace __cbegin {
    struct fn {
      template <class R>
        requires requires (const R& r) { __stl2::begin(r); }
      constexpr auto operator()(const R& r) const
      STL2_NOEXCEPT_RETURN(__stl2::begin(r))

#if !STL2_TREAT_RVALUES_AS_CONST
      template <class R>
      constexpr auto operator()(const R&&) const = delete;
#endif
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& cbegin = detail::static_const<__cbegin::fn>::value;
  }

  // cend
  namespace __cend {
    struct fn {
      template <class R>
        requires requires (const R& r) { __stl2::end(r); }
      constexpr auto operator()(const R& r) const
      STL2_NOEXCEPT_RETURN(__stl2::end(r))

#if !STL2_TREAT_RVALUES_AS_CONST
      template <class R>
      constexpr auto operator()(const R&&) const = delete;
#endif
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& cend = detail::static_const<__cend::fn>::value;
  }

  // rbegin
  namespace __rbegin {
    // Prefer member if it returns Iterator
    template <class R>
    constexpr bool has_member = false;
    template <class R>
      requires requires (R& r) {
        // { r.rbegin() } -> Iterator;
        STL2_DEDUCTION_CONSTRAINT(r.rbegin(), Iterator);
      }
    constexpr bool has_member<R> = true;

    // Default to make_reverse_iterator(end(r)) for Bounded ranges of
    // Bidirectional iterators.
    template <class R>
    constexpr bool can_make_reverse = false;
    template <class R>
      requires requires (R& r) {
        requires Same<decltype(__stl2::begin(r)), decltype(__stl2::end(r))>();
        make_reverse_iterator(__stl2::end(r));
      }
    constexpr bool can_make_reverse<R> = true;

    struct fn {
      template <class E>
      constexpr reverse_iterator<const E*>
      operator()(std::initializer_list<E> il) const
      STL2_NOEXCEPT_RETURN(
        reverse_iterator<const E*>{il.end()}
      )

      template <class R>
        requires has_member<R>
      constexpr auto operator()(R& r) const
      STL2_NOEXCEPT_RETURN(r.rbegin())

      template <class R>
        requires !has_member<R> && can_make_reverse<R>
      constexpr auto operator()(R& r) const
      STL2_NOEXCEPT_RETURN(
        make_reverse_iterator(__stl2::end(r))
      )

#if STL2_TREAT_RVALUES_AS_CONST
      template <class R>
      [[deprecated]] constexpr auto operator()(const R&& r) const
        noexcept(noexcept(declval<const fn&>()(r)))
        requires has_member<const R> || can_make_reverse<const R>
      {
        return (*this)(r);
      }
#endif
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& rbegin = detail::static_const<__rbegin::fn>::value;
  }

  // rend
  namespace __rend {
    template <class R>
    constexpr bool has_member = false;
    template <class R>
      requires requires (R& r) {
        // { r.rend() } -> Sentinel<decltype(__stl2::rbegin(r))>;
        requires Sentinel<decltype(r.rend()), decltype(__stl2::rbegin(r))>();
      }
    constexpr bool has_member<R> = true;

    template <class R>
    constexpr bool can_make_reverse = false;
    template <class R>
      requires requires (R& r) {
        requires Same<decltype(__stl2::begin(r)), decltype(__stl2::end(r))>();
        make_reverse_iterator(__stl2::begin(r));
      }
    constexpr bool can_make_reverse<R> = true;

    struct fn {
      template <class E>
      constexpr reverse_iterator<const E*>
      operator()(std::initializer_list<E> il) const noexcept {
        return reverse_iterator<const E*>{il.begin()};
      }

      // Prefer member if it returns Sentinel
      template <class R>
        requires has_member<R>
      constexpr auto operator()(R& r) const
      STL2_NOEXCEPT_RETURN(r.rend())

      // Default to make_reverse_iterator(begin(r)) for Bounded ranges of
      // Bidirectional iterators.
      template <class R>
        requires !has_member<R> && can_make_reverse<R>
      constexpr auto operator()(R& r) const
      STL2_NOEXCEPT_RETURN(
        __stl2::make_reverse_iterator(__stl2::begin(r))
      )

#if STL2_TREAT_RVALUES_AS_CONST
      template <class R>
      [[deprecated]] constexpr auto operator()(const R&& r) const
        noexcept(noexcept(declval<const fn&>()(r)))
        requires has_member<const R> || can_make_reverse<const R>
      {
        return (*this)(r);
      }
#endif
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& rend = detail::static_const<__rend::fn>::value;
  }

  // crbegin
  namespace __crbegin {
    struct fn {
      template <class R>
        requires requires (const R& r) { __stl2::rbegin(r); }
      constexpr auto operator()(const R& r) const
      STL2_NOEXCEPT_RETURN(
        __stl2::rbegin(r)
      )

#if !STL2_TREAT_RVALUES_AS_CONST
      template <class R>
      constexpr auto operator()(const R&&) const = delete;
#endif
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& crbegin = detail::static_const<__crbegin::fn>::value;
  }

  // crend
  namespace __crend {
    struct fn {
      template <class R>
        requires requires (const R& r) { __stl2::rend(r); }
      constexpr auto operator()(const R& r) const
      STL2_NOEXCEPT_RETURN(
        __stl2::rend(r)
      )

#if !STL2_TREAT_RVALUES_AS_CONST
      template <class R>
      constexpr auto operator()(const R&&) const = delete;
#endif
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& crend = detail::static_const<__crend::fn>::value;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Container access [iterator.container]
  //
  // size
  namespace __size {
    // Poison pill for std::size. (See the detailed discussion at
    // https://github.com/ericniebler/stl2/issues/139)
    void size(const auto&) = delete;

    template <class R>
    constexpr bool has_member = false;
    template <class R>
      requires requires (const R& r) {
        STL2_DEDUCTION_CONSTRAINT(r.size(), Integral);
      }
    constexpr bool has_member<R> = true;

    template <class R>
    constexpr bool has_non_member = false;
    template <class R>
      requires requires (const R& r) {
        STL2_DEDUCTION_CONSTRAINT(size(r), Integral);
      }
    constexpr bool has_non_member<R> = true;

    template <class R>
    constexpr bool has_difference = false;
    template <class R>
      requires requires (const R& r) {
        requires SizedIteratorRange<decltype(__stl2::begin(r)),
                                    decltype(__stl2::end(r))>();
      }
    constexpr bool has_difference<R> = true;

    struct fn {
      template <class T, std::size_t N>
      constexpr std::size_t operator()(T(&)[N]) const noexcept {
        return N;
      }

      // Prefer member
      template <class R>
        requires has_member<R>
      constexpr auto operator()(const R& r) const
      STL2_NOEXCEPT_RETURN(r.size())

      // Use non-member
      template <class R>
        requires !has_member<R> && has_non_member<R>
      constexpr auto operator()(const R& r) const
      STL2_NOEXCEPT_RETURN(size(r))

      // Extension: Use (end - begin) for SizedIteratorRange
      template <class R>
        requires !has_member<R> && !has_non_member<R> && has_difference<R>
      constexpr auto operator()(const R& r) const
      STL2_NOEXCEPT_RETURN(
        __stl2::end(r) - __stl2::begin(r)
      )
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& size = detail::static_const<__size::fn>::value;
  }

  // empty
  namespace __empty {
    template <class R>
    constexpr bool has_member = false;
    template <class R>
      requires requires (const R& r) {
        STL2_CONVERSION_CONSTRAINT(r.empty(), bool);
      }
    constexpr bool has_member<R> = true;

    template <class R>
    constexpr bool has_begin_end = false;
    template <class R>
      requires requires (const R& r) {
        // This is sufficient: if __stl2::end(r) is a valid expression,
        // __stl2::begin(r) must also be valid and
        // Sentinel<decltype(end(r)), decltype(begin(r))>() must be satisfied.
        __stl2::end(r);
      }
    constexpr bool has_begin_end<R> = true;

    struct fn {
      template <class T, std::size_t N>
      constexpr bool operator()(T(&)[N]) const noexcept {
        return N == 0;
      }

      template <class E>
      constexpr bool operator()(std::initializer_list<E> il) const noexcept {
        return il.size() == 0;
      }

      // Prefer member
      template <class R>
        requires has_member<R>
      constexpr bool operator()(const R& r) const
      STL2_NOEXCEPT_RETURN(
        static_cast<bool>(r.empty())
      )

      // Use begin == end
      template <class R>
        requires !has_member<R> && has_begin_end<R>
      constexpr bool operator()(const R& r) const
      STL2_NOEXCEPT_RETURN(
        static_cast<bool>(__stl2::begin(r) == __stl2::end(r))
      )
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& empty = detail::static_const<__empty::fn>::value;
  }

  // data
  namespace __data {
    template <class R>
    constexpr bool has_member = false;
    template <class R>
      requires requires (R& r) {
        //{ r.data() } -> _Is<std::is_pointer>;
        requires _Is<decltype(r.data()), std::is_pointer>;
      }
    constexpr bool has_member<R> = true;

    template <class R>
    constexpr bool has_contiguous_iterator = false;
    template <class R>
      requires requires (R& r) {
        requires ext::ContiguousIterator<decltype(__stl2::begin(r))>();
      }
    constexpr bool has_contiguous_iterator<R> = true;

    struct fn {
      template <class T, std::size_t N>
      constexpr T* operator()(T (&array)[N]) const noexcept {
        return array;
      }

      template <class E>
      constexpr const E* operator()(std::initializer_list<E> il) const noexcept {
        return il.begin();
      }

      // Prefer member
      template <class R>
        requires has_member<R>
      constexpr auto operator()(R& r) const
      STL2_NOEXCEPT_RETURN(r.data())

      // Default implementation for contiguous ranges
      template <class R>
        requires !has_member<R> && has_contiguous_iterator<R>
      auto operator()(R& r) const
        noexcept(noexcept(
        __stl2::begin(r) == __stl2::end(r)
          ? nullptr : std::addressof(*__stl2::begin(r))))
      {
        auto i = __stl2::begin(r);
        return i == __stl2::end(r) ? nullptr : std::addressof(*i);
      }

#if STL2_TREAT_RVALUES_AS_CONST
      template <class R>
      [[deprecated]] constexpr auto operator()(const R&& r) const
        noexcept(noexcept(declval<const fn&>()(r)))
        requires has_member<const R> || has_contiguous_iterator<const R>
      {
        return (*this)(r);
      }
#endif
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    constexpr auto& data = detail::static_const<__data::fn>::value;
  }

  namespace ext {
    // cdata
    namespace __cdata {
      struct fn {
        template <class R>
          requires requires (const R& r) { __stl2::data(r); }
        constexpr auto operator()(const R& r) const
        STL2_NOEXCEPT_RETURN(__stl2::data(r))

#if !STL2_TREAT_RVALUES_AS_CONST
        template <class R>
        constexpr auto operator()(const R&&) const = delete;
#endif
      };
    }
    // Workaround GCC PR66957 by declaring this unnamed namespace inline.
    inline namespace {
      constexpr auto& cdata = detail::static_const<__cdata::fn>::value;
    }
  }
} STL2_CLOSE_NAMESPACE

#undef STL2_TREAT_RVALUES_AS_CONST

#endif
