#ifndef STL2_ITERATOR_HPP
#define STL2_ITERATOR_HPP

#include <cassert>
#include <initializer_list>
#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/concepts/range.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/insert_iterators.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>

namespace stl2 { inline namespace v1 {

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

}} // namespace stl2::v1

#endif
