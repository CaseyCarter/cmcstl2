#ifndef STL2_DETAIL_RANGE_ACCESS_HPP
#define STL2_DETAIL_RANGE_ACCESS_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
//#include <stl2/detail/iterator/reverse_iterator.hpp>

namespace stl2 { namespace v1 {

BidirectionalIterator{I} class reverse_iterator;

// begin
// 20150805: Not to spec. Rejects rvalues, is an N4381-style customization
// point, etc.
namespace __begin {
  template <class T, std::size_t N>
  constexpr T* begin(T (&array)[N]) noexcept {
    return array;
  }

  template <class C>
    requires requires (C& c) {
      STL2_DEDUCTION_CONSTRAINT(c.begin(), Iterator);
    }
  constexpr auto /* Iterator */ begin(C& c)
    noexcept(noexcept(c.begin())) {
    return c.begin();
  }

  template <class C>
    requires requires (const C& c) {
      STL2_DEDUCTION_CONSTRAINT(c.begin(), Iterator);
    }
  constexpr auto /* Iterator */ begin(const C& c)
    noexcept(noexcept(c.begin())) {
    return c.begin();
  }

  struct fn {
    template <class R>
      requires requires (R&& r) {
      STL2_DEDUCTION_CONSTRAINT(begin((R&&)r), Iterator);
      }
    constexpr auto /* Iterator */ operator()(R&& r) const
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
      requires Sentinel<decltype(c.end()), decltype(c.begin())>();
    }
  constexpr auto end(C& c)
    noexcept(noexcept(c.end())) {
    return c.end();
  }

  struct fn {
    template <class R>
      requires requires (R&& r) {
        requires Sentinel<decltype(end((R&&)r)), decltype(begin((R&&)r))>();
      }
    constexpr auto operator()(R&& r) const
      noexcept(noexcept(end(stl2::forward<R>(r)))) {
      return end(r);
    }
  };
}
namespace {
  constexpr auto& end = detail::static_const<__end::fn>;
}

// cbegin
namespace __cbegin {
  struct fn {
    template <class R>
      requires requires (const R& r) { begin(r); }
    constexpr decltype(auto) operator()(const R& r) const {
      return begin(r);
    }
  };
}
namespace {
  constexpr auto& cbegin = detail::static_const<__cbegin::fn>;
}

// cend
namespace __cend {
  struct fn {
    template <class R>
      requires requires (const R& r) { end(r); }
    constexpr decltype(auto) operator()(const R& r) const {
      return end(r);
    }
  };
}
namespace {
  constexpr auto& cend = detail::static_const<__cend::fn>;
}

template <class C>
constexpr auto rbegin(C& c) -> decltype(c.rbegin()) {
  return c.rbegin();
}
template <class C>
constexpr auto rbegin(const C& c) -> decltype(c.rbegin()) {
  return c.rbegin();
}

template <class C>
constexpr auto rend(C& c) -> decltype(c.rend()) {
  return c.rend();
}

template <class C>
constexpr auto rend(const C& c) -> decltype(c.rend()) {
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
constexpr auto crbegin(const C& c) -> decltype(stl2::rbegin(c)) {
  return stl2::rbegin(c);
}

template <class C>
constexpr auto crend(const C& c) -> decltype(stl2::rend(c)) {
  return stl2::rend(c);
}

}} // namespace stl2::v1

#endif
