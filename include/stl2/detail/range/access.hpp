#ifndef STL2_DETAIL_RANGE_ACCESS_HPP
#define STL2_DETAIL_RANGE_ACCESS_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
//#include <stl2/detail/iterator/reverse_iterator.hpp>

namespace stl2 { namespace v1 {

BidirectionalIterator{I} class reverse_iterator;

// Disable constraints that cause the compiler to OOM
#define STL2_HACK_CONSTRAINTS

// begin
// 20150805: Not to spec: is an N4381-style customization point.
namespace __begin {
  template <class T, std::size_t N>
  constexpr T* begin(T (&array)[N]) noexcept {
    return array;
  }

  template <class C>
    requires requires (C& c) {
#ifdef STL2_HACK_CONSTRAINTS
      c.begin();
#else
      STL2_DEDUCTION_CONSTRAINT(c.begin(), Iterator);
#endif
    }
  constexpr STL2_CONSTRAINED_AUTO(Iterator) begin(C& c)
    noexcept(noexcept(c.begin())) {
    return c.begin();
  }

  template <class C>
    requires requires (const C& c) {
#ifdef STL2_HACK_CONSTRAINTS
      c.begin();
#else
      STL2_DEDUCTION_CONSTRAINT(c.begin(), Iterator);
#endif
    }
  constexpr STL2_CONSTRAINED_AUTO(Iterator) begin(const C& c)
    noexcept(noexcept(c.begin())) {
    return c.begin();
  }

  struct fn {
    template <class R>
      requires requires (R&& r) {
#ifdef STL2_HACK_CONSTRAINTS
        begin((R&&)r);
#else
        STL2_DEDUCTION_CONSTRAINT(begin((R&&)r), Iterator);
#endif
      }
    constexpr STL2_CONSTRAINED_AUTO(Iterator) operator()(R&& r) const
      noexcept(noexcept(begin(stl2::forward<R>(r)))) {
      return begin(stl2::forward<R>(r));
    }
  };
}
namespace {
  constexpr auto& begin = detail::static_const<__begin::fn>;
}

// end
namespace __end {
  template <class T, std::size_t N>
  constexpr T* end(T (&array)[N]) noexcept {
    return array + N;
  }

  template <class C>
    requires requires (C& c) {
#ifdef STL2_HACK_CONSTRAINTS
      c.end();
#else
      requires Sentinel<decltype(c.end()), decltype(c.begin())>();
#endif
    }
  constexpr STL2_CONSTRAINED_AUTO(Iterator) end(C& c)
    noexcept(noexcept(c.end())) {
    return c.end();
  }

  template <class C>
    requires requires (const C& c) {
#ifdef STL2_HACK_CONSTRAINTS
      c.end();
#else
      requires Sentinel<decltype(c.end()), decltype(c.begin())>();
#endif
    }
  constexpr STL2_CONSTRAINED_AUTO(Iterator) end(const C& c)
    noexcept(noexcept(c.end())) {
    return c.end();
  }

  struct fn {
    template <class R>
      requires requires (R&& r) {
#ifdef STL2_HACK_CONSTRAINTS
        end((R&&)r);
#else
        requires Sentinel<decltype(end((R&&)r)), decltype(begin((R&&)r))>();
#endif
      }
    constexpr STL2_CONSTRAINED_AUTO(Iterator) operator()(R&& r) const
      noexcept(noexcept(end(stl2::forward<R>(r)))) {
      return end(r);
    }
  };
}
namespace {
  constexpr auto& end = detail::static_const<__end::fn>;
}

// cbegin
template <class R>
  requires requires (const R& r) { stl2::begin(r); }
constexpr STL2_CONSTRAINED_AUTO(Iterator) cbegin(const R& r)
  noexcept(noexcept(stl2::begin(r))) {
  return stl2::begin(r);
}

// cend
template <class R>
  requires requires (const R& r) { stl2::end(r); }
constexpr STL2_CONSTRAINED_AUTO(Iterator) cend(const R& r)
  noexcept(noexcept(stl2::end(r))) {
  return stl2::end(r);
}

template <class C>
  requires requires(C& c) {
#ifdef STL2_HACK_CONSTRAINTS
    c.rbegin();
#else
    STL2_DEDUCTION_CONSTRAINT(c.rbegin(), Iterator);
#endif
  }
constexpr STL2_CONSTRAINED_AUTO(Iterator) rbegin(C& c) {
  return c.rbegin();
}

template <class C>
  requires requires(const C& c) {
#ifdef STL2_HACK_CONSTRAINTS
    c.rbegin();
#else
    STL2_DEDUCTION_CONSTRAINT(c.rbegin(), Iterator);
#endif
  }
constexpr STL2_CONSTRAINED_AUTO(Iterator) rbegin(const C& c) {
  return c.rbegin();
}

template <class C>
  requires requires(C& c) {
#ifdef STL2_HACK_CONSTRAINTS
    c.rend();
#else
    STL2_DEDUCTION_CONSTRAINT(c.rend(), Iterator);
#endif
  }
constexpr STL2_CONSTRAINED_AUTO(Iterator) rend(C& c) {
  return c.rend();
}

template <class C>
  requires requires(const C& c) {
#ifdef STL2_HACK_CONSTRAINTS
    c.rend();
#else
    STL2_DEDUCTION_CONSTRAINT(c.rend(), Iterator);
#endif
  }
constexpr STL2_CONSTRAINED_AUTO(Iterator) rend(const C& c) {
  return c.rend();
}

template <class T, size_t N>
reverse_iterator<T*> rbegin(T (&array)[N]) {
  return reverse_iterator<T*>{stl2::end(array)};
}

template <class T, size_t N>
reverse_iterator<T*> rend(T (&array)[N]) {
  return reverse_iterator<T*>{stl2::begin(array)};
}

template <class E>
reverse_iterator<const E*> rbegin(std::initializer_list<E> il) {
  return reverse_iterator<const E*>{il.end()};
}

template <class E>
reverse_iterator<const E*> rend(std::initializer_list<E> il) {
  return reverse_iterator<const E*>{il.begin()};
}

template <class C>
  requires requires(const C& c) { stl2::rbegin(c); }
constexpr STL2_CONSTRAINED_AUTO(Iterator) crbegin(const C& c) {
  return stl2::rbegin(c);
}

template <class C>
  requires requires(const C& c) { stl2::rend(c); }
constexpr STL2_CONSTRAINED_AUTO(Iterator) crend(const C& c) {
  return stl2::rend(c);
}

#undef STL2_HACK_CONSTRAINTS

}} // namespace stl2::v1

#endif
