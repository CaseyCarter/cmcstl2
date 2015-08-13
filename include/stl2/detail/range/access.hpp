#ifndef STL2_DETAIL_RANGE_ACCESS_HPP
#define STL2_DETAIL_RANGE_ACCESS_HPP

#include <initializer_list>
#include <iterator>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>

namespace stl2 { inline namespace v1 {
using std::begin;
using std::end;

namespace detail {
  template <class>
  concept bool _Auto = true;
}

template <detail::_Auto C>
constexpr auto cbegin(const C& c)
  noexcept(noexcept(begin(c))) -> decltype(begin(c)) {
  return begin(c);
}

template <detail::_Auto C>
constexpr auto cend(const C& c)
  noexcept(noexcept(end(c))) -> decltype(end(c)) {
  return end(c);
}

template <detail::_Auto C>
constexpr auto rbegin(C& c) -> decltype(c.rbegin()) {
  return c.rbegin();
}

template <detail::_Auto C>
constexpr auto rbegin(const C& c) -> decltype(c.rbegin()) {
  return c.rbegin();
}

template <detail::_Auto C>
constexpr auto rend(C& c) -> decltype(c.rend()) {
  return c.rend();
}

template <detail::_Auto C>
constexpr auto rend(const C& c) -> decltype(c.rend()) {
  return c.rend();
}

template <detail::_Auto T, size_t N>
reverse_iterator<T*> rbegin(T (&array)[N]) {
  return reverse_iterator<T*>{array + N};
}

template <detail::_Auto T, size_t N>
reverse_iterator<T*> rend(T (&array)[N]) {
  return reverse_iterator<T*>{array};
}

template <detail::_Auto E>
reverse_iterator<const E*> rbegin(std::initializer_list<E> il) {
  return reverse_iterator<const E*>{il.end()};
}

template <detail::_Auto E>
reverse_iterator<const E*> rend(std::initializer_list<E> il) {
  return reverse_iterator<const E*>{il.begin()};
}

template <detail::_Auto C>
constexpr auto crbegin(const C& c) -> decltype(stl2::rbegin(c)) {
  return stl2::rbegin(c);
}

template <detail::_Auto C>
constexpr auto crend(const C& c) -> decltype(stl2::rend(c)) {
  return stl2::rend(c);
}

// size
template <class T, std::size_t N>
constexpr std::size_t size(T(&)[N]) noexcept {
  return N;
}

template <class C>
  requires requires (const C& c) {
    STL2_DEDUCTION_CONSTRAINT(c.size(), Integral);
  }
constexpr auto /* Integral */ size(const C& c)
  noexcept(noexcept(c.size())) {
  return c.size();
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

template <class C>
  requires requires (const C& c) {
    STL2_CONVERSION_CONSTRAINT(c.empty(), bool);
  }
constexpr bool empty(const C& c)
  noexcept(noexcept(bool(c.empty()))) {
  return bool(c.empty());
}

// data
template <class T, std::size_t N>
constexpr T* data(T (&a)[N]) noexcept {
  return a + 0;
}

template <class E>
constexpr const E* data(std::initializer_list<E> il) noexcept {
  return il.begin();
}

template <class C>
  requires requires (C& c) {
    requires _Is<decltype(c.data()), std::is_pointer>;
  }
constexpr auto data(C& c)
  noexcept(noexcept(c.data())) {
  return c.data();
}

template <class C>
  requires requires (const C& c) {
    requires _Is<decltype(c.data()), std::is_pointer>;
  }
constexpr auto data(const C& c)
  noexcept(noexcept(c.data())) {
  return c.data();
}
}} // namespace stl2::v1

#endif
