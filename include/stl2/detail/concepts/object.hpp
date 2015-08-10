#ifndef STL2_DETAIL_CONCEPTS_OBJECT_HPP
#define STL2_DETAIL_CONCEPTS_OBJECT_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/type_traits.hpp>

////////////////////////////////////////
// Object Concepts [concepts.lib.object]
//
namespace stl2 { inline namespace v1 {

// 20150712: not to spec. is_object and accepting the
// parameters by reference are necessary to prevent hard
// errors in the requires clause with odd types.
template <class T>
concept bool Destructible() {
  return _Is<T, std::is_object> &&
    requires (T& t, const T& ct, T* const p) {
      { t.~T() } noexcept;
      STL2_EXACT_TYPE_CONSTRAINT(&t, T*); // not required to be equality preserving
      STL2_EXACT_TYPE_CONSTRAINT(&ct, const T*); // not required to be equality preserving
      delete p;
      delete[] p;
    };
}

namespace ext {
template <class T, class...Args>
concept bool ConstructibleObject =
  Destructible<T>() && requires (Args&&...args) {
    T{ (Args&&)args... }; // not required to be equality preserving
    new T{ (Args&&)args... }; // not required to be equality preserving
  };

// 20150718: Not to spec: spec is broken.
// FIXME: Correct wording.
template <class T, class...Args>
concept bool BindableReference =
  _Is<T, std::is_reference> &&
  // requires (Args&&...args) { T{ (Args&&)args... }; };
  // requires (Args&&...args) { T( (Args&&)args... ); };
  _Is<T, std::is_constructible, Args...>;
}

namespace detail {
template <class T, class...Args>
struct constructible_object_or_ref : std::false_type {};

template <class T, class...Args>
  requires ext::ConstructibleObject<T, Args...> ||
    ext::BindableReference<T, Args...>
struct constructible_object_or_ref<T, Args...> : std::true_type {};
}

template <class T, class...Args>
concept bool Constructible() {
  return meta::_v<detail::constructible_object_or_ref<T, Args...>>;
}

// There's implementation variance around DR1518, this may not
// enforce the requirement that the default constructor be
// non-explicit.
template <class T>
concept bool DefaultConstructible() {
  return Constructible<T>() &&
    requires (const std::size_t n) {
      new T[n]{}; // not required to be equality preserving
    };
}

template <class T>
concept bool MoveConstructible() {
  return Constructible<T, std::remove_cv_t<T>&&>() &&
    ext::ImplicitlyConvertibleTo<std::remove_cv_t<T>&&, T>();
}

template <class T>
concept bool CopyConstructible() {
  return MoveConstructible<T>() &&
    Constructible<T, const std::remove_cv_t<T>&>() &&
    ext::ImplicitlyConvertibleTo<std::remove_cv_t<T>&, T>() &&
    ext::ImplicitlyConvertibleTo<const std::remove_cv_t<T>&, T>() &&
    ext::ImplicitlyConvertibleTo<const std::remove_cv_t<T>&&, T>();
}

template <class T>
concept bool Movable() {
  return MoveConstructible<T>() &&
    Assignable<T&, T&&>();
}

template <class T>
concept bool Copyable() {
  return CopyConstructible<T>() &&
    Movable<T>() &&
    Assignable<T&, const T&>();
}

template <class T>
concept bool Semiregular() {
  return Copyable<T>() &&
    DefaultConstructible<T>();
}

template <class T>
concept bool Regular() {
  return Semiregular<T>() &&
    EqualityComparable<T>();
}

template <Movable T, class U = T>
  requires Assignable<T&, U>()
constexpr T exchange(T& t, U&& u)
  noexcept(std::is_nothrow_move_constructible<T>::value &&
           std::is_nothrow_assignable<T&, U>::value);

namespace __swap {
constexpr void swap(Movable& a, Movable& b)
  noexcept(noexcept(b = exchange(a, stl2::move(b))));

struct __try_swap_fn {
  auto operator()(auto &t, auto &u) const
    noexcept(noexcept(swap(t,u)))
    -> decltype(swap(t,u));

  template <std::size_t N, typename This = __try_swap_fn>
  auto operator()(auto (&t)[N], auto (&u)[N]) const
    noexcept(noexcept(This{}(*t, *u)))
    -> decltype(This{}(*t, *u));
};

namespace {
  constexpr auto& __try_swap = detail::static_const<__try_swap_fn>::value;
}

// 20150715: Conforming extension: can swap T(&)[N] with U(&)[N]
// if T& and U& are Swappable.
template <class T, class U, std::size_t N>
  requires requires (T &t, U &u) { __try_swap(t, u); }
constexpr void swap(T (&t)[N], U (&u)[N])
  noexcept(noexcept(__try_swap(*t, *u)));

struct fn {
  template <class T, class U>
    requires requires (T&& a, U&& b) { swap((T&&)a, (U&&)b); }
  constexpr void operator()(T&& a, U&& b) const
    noexcept(noexcept(swap(stl2::forward<T>(a), stl2::forward<U>(b))));
};
} // namespace __swap

namespace {
// 20150805: Not to spec: swap is a N4381-style function object customization point.
  constexpr auto& swap = detail::static_const<__swap::fn>::value;
}

namespace detail {
template <class T, class U>
concept bool Swappable_ =
  requires (T&& t, U&&u) {
    swap((T&&)t, (U&&)u);
  };
}

template <class T>
concept bool Swappable() {
  return detail::Swappable_<T, T>;
}

template <class T, class U>
concept bool Swappable() {
  return Swappable<T>() &&
    Swappable<U>() &&
    CommonReference<const T&, const U&>() &&
    detail::Swappable_<T, U> &&
    detail::Swappable_<U, T>;
}

namespace ext { namespace models {
template <class>
constexpr bool destructible() { return false; }
Destructible{T}
constexpr bool destructible() { return true; }

template <class T, class...Args>
constexpr bool constructible_object() { return false; }
ConstructibleObject{T, ...Args}
constexpr bool constructible_object() { return true; }

template <class T, class...Args>
constexpr bool bindable_reference() { return false; }
BindableReference{T, ...Args}
constexpr bool bindable_reference() { return true; }

template <class, class...>
constexpr bool constructible() { return false; }
Constructible{T, ...Args}
constexpr bool constructible() { return true; }

template <class>
constexpr bool default_constructible() { return false; }
DefaultConstructible{T}
constexpr bool default_constructible() { return true; }

template <class>
constexpr bool move_constructible() { return false; }
MoveConstructible{T}
constexpr bool move_constructible() { return true; }

template <class>
constexpr bool copy_constructible() { return false; }
CopyConstructible{T}
constexpr bool copy_constructible() { return true; }

template <class>
constexpr bool movable() { return false; }
Movable{T}
constexpr bool movable() { return true; }

template <class>
constexpr bool copyable() { return false; }
Copyable{T}
constexpr bool copyable() { return true; }

template <class>
constexpr bool semiregular() { return false; }
Semiregular{T}
constexpr bool semiregular() { return true; }

template <class>
constexpr bool regular() { return false; }
Regular{T}
constexpr bool regular() { return true; }

template <class>
constexpr bool swappable() { return false; }
Swappable{T}
constexpr bool swappable() { return true; }
template <class, class>
constexpr bool swappable() { return false; }
Swappable{T, U}
constexpr bool swappable() { return true; }
}}}} // namespace stl2::v1::ext::models

#endif
