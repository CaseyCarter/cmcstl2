#ifndef STL2_DETAIL_RANGE_ACCESS_HPP
#define STL2_DETAIL_RANGE_ACCESS_HPP

#include <initializer_list>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/iterator/concepts.hpp>

// HACKHACK: Disable constraints that cause the compiler to OOM
#define STL2_HACK_CONSTRAINTS 1

namespace stl2 { inline namespace v1 {
BidirectionalIterator{I} class reverse_iterator;
BidirectionalIterator{I}
reverse_iterator<I> make_reverse_iterator(I);

///////////////////////////////////////////////////////////////////////////
// Range access [iterator.range]
//
// begin
// 20150805: Not to spec: is an N4381-style customization point.
namespace __begin {
  // Use member if it returns Iterator.
  template <class R>
    requires requires (R& r) {
      STL2_DEDUCTION_CONSTRAINT(r.begin(), Iterator);
    }
  constexpr auto impl(R& r, ext::priority_tag<0>)
    noexcept(noexcept(r.begin())) {
    return r.begin();
  }

  // Prefer ADL if it returns Iterator.
  template <class R>
    requires requires (R& r) {
      STL2_DEDUCTION_CONSTRAINT(begin(r), Iterator);
    }
  constexpr auto impl(R& r, ext::priority_tag<1>)
    noexcept(noexcept(begin(r))) {
    return begin(r);
  }

  struct fn {
    template <class R, std::size_t N>
    constexpr R* operator()(R (&array)[N]) const noexcept {
      return array;
    }

    template <class R>
      requires requires (R& r) { __begin::impl(r, ext::max_priority_tag); }
    constexpr auto operator()(R& r) const
      noexcept(noexcept(__begin::impl(r, ext::max_priority_tag))) {
      return __begin::impl(r, ext::max_priority_tag);
    }

#if 1
    // Backwards compatibility: rvalues are treated as
    // const lvalues.
    template <class R>
      requires requires (const R& r) { __begin::impl(r, ext::max_priority_tag); }
    constexpr auto operator()(const R&& r) const
      noexcept(noexcept(__begin::impl(r, ext::max_priority_tag))) {
      return __begin::impl(r, ext::max_priority_tag);
    }
#else
    // Forget backwards compatibility, begin on rvalues is broken;
    // except for std::initializer_list.
    template <class E>
    constexpr const E* operator()(std::initializer_list<E> il) const noexcept {
      return il.begin();
    }
#endif
  };
}
namespace {
  constexpr auto& begin = detail::static_const<__begin::fn>::value;
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
      requires Sentinel<decltype(r.end()), decltype(stl2::begin(r))>();
#endif
    }
  constexpr auto end(R& r)
    noexcept(noexcept(r.end())) {
#ifdef STL2_HACK_CONSTRAINTS
    static_assert(ext::models::sentinel<decltype(r.end()), decltype(stl2::begin(r))>());
#endif
    return r.end();
  }

  template <class R>
    requires requires (const R& r) {
#ifdef STL2_HACK_CONSTRAINTS
      r.end();
#else
      requires Sentinel<decltype(r.end()), decltype(stl2::begin(r))>();
#endif
    }
  constexpr auto end(const R& r)
    noexcept(noexcept(r.end())) {
#ifdef STL2_HACK_CONSTRAINTS
    static_assert(ext::models::sentinel<decltype(r.end()), decltype(stl2::begin(r))>());
#endif
    return r.end();
  }

  struct fn {
    template <class R>
      requires requires (R&& r) {
#ifdef STL2_HACK_CONSTRAINTS
        end((R&&)r);
#else
        requires Sentinel<decltype(end((R&&)r)), decltype(stl2::begin((R&&)r))>();
#endif
      }
    constexpr auto operator()(R&& r) const
      noexcept(noexcept(end(stl2::forward<R>(r)))) ->
      STL2_CONSTRAINED_RETURN(Sentinel<decltype(stl2::begin((R&&)r))>) {
#ifdef STL2_HACK_CONSTRAINTS
      static_assert(ext::models::sentinel<decltype(end((R&&)r)), decltype(stl2::begin((R&&)r))>());
#endif
      return end(r);
    }
  };
}
namespace {
  constexpr auto& end = detail::static_const<__end::fn>::value;
}

// cbegin
template <class R>
  requires requires (const R& r) { stl2::begin(r); }
constexpr STL2_CONSTRAINED_RETURN(Iterator) cbegin(const R& r)
  noexcept(noexcept(stl2::begin(r))) {
  return stl2::begin(r);
}

// cend
template <class R>
  requires requires (const R& r) { stl2::end(r); }
constexpr auto cend(const R& r)
  noexcept(noexcept(stl2::end(r))) ->
  STL2_CONSTRAINED_RETURN(Sentinel<decltype(stl2::begin(r))>) {
  return stl2::end(r);
}

// rbegin
template <class R>
  requires requires(R& r) {
    // BoundedRange
    requires Same<decltype(stl2::begin(r)), decltype(stl2::end(r))>();
    make_reverse_iterator(stl2::end(r));
  }
constexpr STL2_CONSTRAINED_RETURN(Iterator)
__rbegin(R& r, ext::priority_tag<0>)
  noexcept(noexcept(make_reverse_iterator(stl2::end(r)))) {
  return make_reverse_iterator(stl2::end(r));
}

template <class R>
  requires requires(R& r) {
#ifdef STL2_HACK_CONSTRAINTS
    r.rbegin();
#else
    STL2_DEDUCTION_CONSTRAINT(r.rbegin(), Iterator);
#endif
  }
constexpr STL2_CONSTRAINED_RETURN(Iterator)
__rbegin(R& r, ext::priority_tag<1>)
  noexcept(noexcept(r.rbegin())) {
#ifdef STL2_HACK_CONSTRAINTS
  static_assert(ext::models::iterator<decltype(r.rbegin())>());
#endif
  return r.rbegin();
}

template <class R>
  requires requires(const R& r) {
    // BoundedRange
    requires Same<decltype(stl2::begin(r)), decltype(stl2::end(r))>();
    make_reverse_iterator(stl2::end(r));
  }
constexpr STL2_CONSTRAINED_RETURN(Iterator)
__rbegin(const R& r, ext::priority_tag<0>)
  noexcept(noexcept(make_reverse_iterator(stl2::end(r)))) {
  return make_reverse_iterator(stl2::end(r));
}

template <class R>
  requires requires(const R& r) {
#ifdef STL2_HACK_CONSTRAINTS
    r.rbegin();
#else
    STL2_DEDUCTION_CONSTRAINT(r.rbegin(), Iterator);
#endif
  }
constexpr STL2_CONSTRAINED_RETURN(Iterator)
__rbegin(const R& r, ext::priority_tag<1>)
  noexcept(noexcept(r.rbegin())) {
#ifdef STL2_HACK_CONSTRAINTS
  static_assert(ext::models::iterator<decltype(r.rbegin())>());
#endif
  return r.rbegin();
}

template <class R>
  requires requires (R&& r) {
    stl2::__rbegin((R&&)r, ext::max_priority_tag);
  }
constexpr STL2_CONSTRAINED_RETURN(Iterator)
rbegin(R&& r)
  noexcept(noexcept(stl2::__rbegin(stl2::forward<R>(r), ext::max_priority_tag))) {
  return stl2::__rbegin(stl2::forward<R>(r), ext::max_priority_tag);
}

// rend
template <class R>
  requires requires(R& r) {
    // BoundedRange
    requires Same<decltype(stl2::begin(r)), decltype(stl2::end(r))>();
    make_reverse_iterator(stl2::begin(r));
  }
constexpr STL2_CONSTRAINED_RETURN(Iterator)
__rend(R& r, ext::priority_tag<0>)
  noexcept(noexcept(make_reverse_iterator(stl2::begin(r)))) {
  return make_reverse_iterator(stl2::begin(r));
}

template <class R>
  requires requires(R& r) {
#ifdef STL2_HACK_CONSTRAINTS
    r.rend();
#else
    requires Sentinel<decltype(r.rend()), decltype(r.rbegin())>();
#endif
  }
constexpr auto __rend(R& r, ext::priority_tag<1>)
  noexcept(noexcept(r.rend())) ->
  STL2_CONSTRAINED_RETURN(Sentinel<decltype(r.rbegin(r))>) {
#ifdef STL2_HACK_CONSTRAINTS
  static_assert(ext::models::sentinel<decltype(r.rend()), decltype(r.rbegin())>());
#endif
  return r.rend();
}

template <class R>
  requires requires(const R& r) {
    // BoundedRange
    requires Same<decltype(stl2::begin(r)), decltype(stl2::end(r))>();
    make_reverse_iterator(stl2::begin(r));
  }
constexpr STL2_CONSTRAINED_RETURN(Iterator)
__rend(const R& r, ext::priority_tag<0>)
  noexcept(noexcept(make_reverse_iterator(stl2::begin(r)))) {
  return make_reverse_iterator(stl2::begin(r));
}

template <class R>
  requires requires(const R& r) {
#ifdef STL2_HACK_CONSTRAINTS
    r.rend();
#else
    requires Sentinel<decltype(r.rend()), decltype(r.rbegin())>();
#endif
  }
constexpr auto __rend(const R& r, ext::priority_tag<1>)
  noexcept(noexcept(r.rend())) ->
  STL2_CONSTRAINED_RETURN(Sentinel<decltype(r.rbegin(r))>) {
#ifdef STL2_HACK_CONSTRAINTS
  static_assert(ext::models::sentinel<decltype(r.rend()), decltype(r.rbegin())>());
#endif
  return r.rend();
}

template <class R>
  requires requires (R&& r) {
    stl2::__rend((R&&)r, ext::max_priority_tag);
  }
constexpr auto
rend(R&& r)
  noexcept(noexcept(stl2::__rend(stl2::forward<R>(r), ext::max_priority_tag))) ->
  STL2_CONSTRAINED_RETURN(Sentinel<decltype(stl2::rbegin(r))>) {
  return stl2::__rend(stl2::forward<R>(r), ext::max_priority_tag);
}

// crbegin
template <class R>
  requires requires(const R& r) { stl2::rbegin(r); }
constexpr STL2_CONSTRAINED_RETURN(Iterator) crbegin(const R& r)
  noexcept(noexcept(stl2::rbegin(r))) {
  return stl2::rbegin(r);
}

// crend
template <class R>
  requires requires(const R& r) { stl2::rend(r); }
constexpr auto crend(const R& r)
  noexcept(noexcept(stl2::rend(r))) ->
  STL2_CONSTRAINED_RETURN(Sentinel<decltype(stl2::rbegin(r))>) {
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
  constexpr auto& size = detail::static_const<__size::fn>::value;
}

// empty
namespace __empty {
template <class T, std::size_t N>
constexpr bool empty(T(&)[N]) noexcept {
  return N == 0;
}

template <class E>
constexpr bool empty(const std::initializer_list<E>& il) noexcept {
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

struct fn {
  template <class R>
    requires requires (const R& r) {
      STL2_CONVERSION_CONSTRAINT(empty(r), bool);
    }
  constexpr bool operator()(const R& r) const
    noexcept(noexcept(empty(r))) {
    return empty(r);
  }
};
}
namespace {
constexpr auto& empty = detail::static_const<__empty::fn>::value;
}

// data
namespace __data {
template <class T, std::size_t N>
constexpr T* data(T (&array)[N]) noexcept {
  return array;
}

template <class E>
constexpr const E* data(std::initializer_list<E> il) noexcept {
  return il.begin();
}

template <class R>
  requires requires (R& r) {
    //{ r.data() } -> _Is<std::is_pointer>;
    requires _Is<decltype(r.data()), std::is_pointer>;
  }
constexpr STL2_CONSTRAINED_RETURN(_Is<std::is_pointer>)
data(R& r) noexcept(noexcept(r.data())) {
  return r.data();
}

template <class R>
  requires requires (const R& r) {
    //{ r.data() } -> _Is<std::is_pointer>;
    requires _Is<decltype(r.data()), std::is_pointer>;
  }
constexpr STL2_CONSTRAINED_RETURN(_Is<std::is_pointer>)
data(const R& r) noexcept(noexcept(r.data())) {
  return r.data();
}

struct fn {
  template <class R>
    requires requires (R& r) {
      //{ data(r) } -> _Is<std::is_pointer>;
      requires _Is<decltype(data(r)), std::is_pointer>;
    }
  constexpr STL2_CONSTRAINED_RETURN(_Is<std::is_pointer>)
  operator()(R& r) const
    noexcept(noexcept(data(r))) {
    return data(r);
  }
};
}
namespace {
constexpr auto& data = detail::static_const<__data::fn>::value;
}
}} // namespace stl2::v1

#undef STL2_HACK_CONSTRAINTS
#endif
