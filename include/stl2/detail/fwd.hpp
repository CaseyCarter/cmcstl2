#ifndef STL2_DETAIL_FWD_HPP
#define STL2_DETAIL_FWD_HPP

#include <type_traits>

#include <stl2/detail/config.hpp>

namespace stl2 { inline namespace v1 {
template <class T>
constexpr T&& forward(std::remove_reference_t<T>& t) noexcept {
  return static_cast<T&&>(t);
}

template <class T>
constexpr T&& forward(std::remove_reference_t<T>&& t) noexcept {
  static_assert(!std::is_lvalue_reference<T>(),
                "[forward]/3: \"If the second form is instantiated with "
                "an lvalue reference type, the program is ill-formed.\"");
  return static_cast<T&&>(t);
}

template <class T>
constexpr std::remove_reference_t<T>&& move(T&& t) noexcept {
  return static_cast<std::remove_reference_t<T>&&>(t);
}

template <class T>
std::add_rvalue_reference_t<T> declval() noexcept;
}} // namespace stl2::v1

#endif // STL2_DETAIL_FWD_HPP
