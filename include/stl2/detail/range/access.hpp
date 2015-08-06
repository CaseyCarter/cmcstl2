#ifndef STL2_DETAIL_RANGE_ACCESS_HPP
#define STL2_DETAIL_RANGE_ACCESS_HPP

#include <initializer_list>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>

// Disable constraints that cause the compiler to OOM
#define STL2_HACK_CONSTRAINTS

namespace stl2 { namespace v1 {

BidirectionalIterator{I} class reverse_iterator;

///////////////////////////////////////////////////////////////////////////
// Range access [iterator.range]
//
// begin
// 20150805: Not to spec: is an N4381-style customization point.
namespace __begin {
  template <class T, std::size_t N>
  constexpr T* begin(T (&array)[N]) noexcept {
    return array;
  }

  template <class R>
    requires requires (R& r) {
#ifdef STL2_HACK_CONSTRAINTS
      r.begin();
#else
      STL2_DEDUCTION_CONSTRAINT(r.begin(), Iterator);
#endif
    }
  constexpr STL2_CONSTRAINED_AUTO(Iterator) begin(R& r)
    noexcept(noexcept(r.begin())) {
    return r.begin();
  }

  template <class R>
    requires requires (const R& r) {
#ifdef STL2_HACK_CONSTRAINTS
      r.begin();
#else
      STL2_DEDUCTION_CONSTRAINT(r.begin(), Iterator);
#endif
    }
  constexpr STL2_CONSTRAINED_AUTO(Iterator) begin(const R& r)
    noexcept(noexcept(r.begin())) {
    return r.begin();
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

  template <class R>
    requires requires (R& r) {
#ifdef STL2_HACK_CONSTRAINTS
      r.end();
#else
      requires Sentinel<decltype(r.end()), decltype(r.begin())>();
#endif
    }
  constexpr auto end(R& r)
    noexcept(noexcept(r.end())) ->
    STL2_CONSTRAINED_AUTO(Sentinel<decltype(r.begin())>) {
    return r.end();
  }

  template <class R>
    requires requires (const R& r) {
#ifdef STL2_HACK_CONSTRAINTS
      r.end();
#else
      requires Sentinel<decltype(r.end()), decltype(r.begin())>();
#endif
    }
  constexpr auto end(const R& r)
    noexcept(noexcept(r.end())) ->
    STL2_CONSTRAINED_AUTO(Sentinel<decltype(r.begin())>) {
    return r.end();
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
    constexpr auto operator()(R&& r) const
      noexcept(noexcept(end(stl2::forward<R>(r)))) ->
      STL2_CONSTRAINED_AUTO(Sentinel<decltype(begin((R&&)r))>) {
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
constexpr auto cend(const R& r)
  noexcept(noexcept(stl2::end(r))) ->
  STL2_CONSTRAINED_AUTO(Sentinel<decltype(stl2::begin(r))>) {
  return stl2::end(r);
}

// rbegin
template <class T, size_t N>
reverse_iterator<T*> rbegin(T (&array)[N]) {
  return reverse_iterator<T*>{end(array)};
}

template <class E>
reverse_iterator<const E*> rbegin(std::initializer_list<E> il) {
  return reverse_iterator<const E*>{il.end()};
}

template <class R>
  requires requires(R& r) {
#ifdef STL2_HACK_CONSTRAINTS
    r.rbegin();
#else
    STL2_DEDUCTION_CONSTRAINT(r.rbegin(), Iterator);
#endif
  }
constexpr STL2_CONSTRAINED_AUTO(Iterator) rbegin(R& r)
  noexcept(noexcept(r.rbegin())) {
  return r.rbegin();
}

template <class R>
  requires requires(const R& r) {
#ifdef STL2_HACK_CONSTRAINTS
    r.rbegin();
#else
    STL2_DEDUCTION_CONSTRAINT(r.rbegin(), Iterator);
#endif
  }
constexpr STL2_CONSTRAINED_AUTO(Iterator) rbegin(const R& r)
  noexcept(noexcept(r.rbegin())) {
  return r.rbegin();
}

// rend
template <class T, size_t N>
reverse_iterator<T*> rend(T (&array)[N]) {
  return reverse_iterator<T*>{begin(array)};
}

template <class E>
reverse_iterator<const E*> rend(std::initializer_list<E> il) {
  return reverse_iterator<const E*>{il.begin()};
}

template <class R>
  requires requires(R& r) {
#ifdef STL2_HACK_CONSTRAINTS
    r.rend();
#else
    requires Sentinel<decltype(r.rend()), decltype(r.rbegin())>();
#endif
  }
constexpr auto rend(R& r)
  noexcept(noexcept(r.rend())) ->
  STL2_CONSTRAINED_AUTO(Sentinel<decltype(r.rbegin(r))>) {
  return r.rend();
}

template <class R>
  requires requires(const R& r) {
#ifdef STL2_HACK_CONSTRAINTS
    r.rend();
#else
    requires Sentinel<decltype(r.rend()), decltype(r.rbegin())>();
#endif
  }
constexpr auto rend(const R& r)
  noexcept(noexcept(r.rend())) ->
  STL2_CONSTRAINED_AUTO(Sentinel<decltype(r.rbegin(r))>) {
  return r.rend();
}

// crbegin
template <class R>
  requires requires(const R& r) { stl2::rbegin(r); }
constexpr STL2_CONSTRAINED_AUTO(Iterator) crbegin(const R& r)
  noexcept(noexcept(stl2::rbegin(r))) {
  return stl2::rbegin(r);
}

// crend
template <class R>
  requires requires(const R& r) { stl2::rend(r); }
constexpr auto crend(const R& r)
  noexcept(noexcept(stl2::rend(r))) ->
  STL2_CONSTRAINED_AUTO(Sentinel<decltype(stl2::rbegin(r))>) {
  return stl2::rend(r);
}

///////////////////////////////////////////////////////////////////////////
// Container access [iterator.container]
//
// size
namespace __size {
  template <class T, std::size_t N>
  constexpr std::size_t size(T(&)[N]) noexcept {
    return N;
  }

  template <class R>
    requires requires (const R& r) { r.size(); }
  constexpr auto size(const R& r)
    noexcept(noexcept(r.size())) {
    return r.size();
  }

  struct fn {
    template <class R>
      requires requires (const R& r) { size(r); }
    constexpr auto operator()(const R& r) const
      noexcept(noexcept(size(r))) {
      return size(r);
    }
  };
}
namespace {
  constexpr auto& size = detail::static_const<__size::fn>;
}

// empty
template <class T, std::size_t N>
constexpr bool empty(T(&)[N]) noexcept {
  return N == 0;
}

template <class E>
constexpr bool empty(std::initializer_list<E> il) noexcept {
  return il.size() == 0;
}

template <class R>
  requires requires (const R& r) {
    STL2_CONVERSION_CONSTRAINT(r.empty(), bool);
  }
constexpr bool empty(const R& r)
  noexcept(noexcept(bool(r.empty()))) {
  return r.empty();
}

// data
template <class T, std::size_t N>
constexpr T* data(T (&array)[N]) noexcept {
  return array;
}

template <class E>
constexpr const E* data(std::initializer_list<E> il) noexcept {
  return il.begin();
}

template <class R>
  requires requires (R& r) { r.data(); }
constexpr auto data(R& r)
  noexcept(noexcept(r.data())) {
  return r.data();
}

template <class R>
  requires requires (const R& r) { r.data(); }
constexpr auto data(const R& r)
  noexcept(noexcept(r.data())) {
  return r.data();
}

}} // namespace stl2::v1

#undef STL2_HACK_CONSTRAINTS

#endif
