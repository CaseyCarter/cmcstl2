#ifndef STL2_CONCEPTS_OBJECT_HPP
#define STL2_CONCEPTS_OBJECT_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/compare.hpp>
#include <stl2/concepts/core.hpp>

#include <type_traits>
#include <utility>

////////////////////////////////////////
// Object Concepts [concepts.lib.object]
//
namespace stl2 { inline namespace v1 {

// 20150712: not to spec. is_object and accepting the
// parameters by reference are necessary to prevent hard
// errors in the requires clause with odd types.
template <class T>
concept bool Destructible() {
  return std::is_object<T>::value &&
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
  std::is_reference<T>::value &&
  // requires (Args&&...args) { T{ (Args&&)args... }; };
  // requires (Args&&...args) { T( (Args&&)args... ); };
  std::is_constructible<T, Args...>::value;
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

constexpr void swap(Movable& a, Movable& b)
  noexcept(noexcept(b = exchange(a, stl2::move(b))));

namespace detail {
  constexpr struct __try_swap_fn {
    template <class T, class U>
    auto operator()(T &t, U &u) const
      noexcept(noexcept(swap(t,u)))
      -> decltype(swap(t,u));

    template <class T, class U, std::size_t N,
      typename This = __try_swap_fn>
    auto operator()(T (&t)[N], U (&u)[N]) const
      noexcept(noexcept(This{}(*t, *u)))
      -> decltype(This{}(*t, *u));
  } __try_swap{};
}

// 20150715: Conforming extension: can swap T(&)[N] with U(&)[N]
// if T& and U& are Swappable.
template <class T, class U, std::size_t N>
  requires requires (T &t, U &u) { detail::__try_swap(t, u); }
constexpr void swap(T (&t)[N], U (&u)[N])
  noexcept(noexcept(detail::__try_swap(*t, *u)));

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

namespace ext {

template <class T, class U = T>
struct is_nothrow_swappable : std::false_type { };

Swappable{T, U}
struct is_nothrow_swappable<T, U> :
  meta::bool_<
    noexcept(swap(stl2::declval<T>(), stl2::declval<U>())) &&
    noexcept(swap(stl2::declval<U>(), stl2::declval<T>())) &&
    noexcept(swap(stl2::declval<T>(), stl2::declval<T>())) &&
    noexcept(swap(stl2::declval<U>(), stl2::declval<U>()))> { };

template <class T, class U = T>
constexpr bool is_nothrow_swappable_v =
  meta::_v<is_nothrow_swappable<T, U>>;

template <class T, class U>
using is_nothrow_swappable_t =
  meta::_t<is_nothrow_swappable<T, U>>;

}


#if 0
namespace ext {

template <class T>
concept bool Scalar() {
  return std::is_scalar<T>::value && Regular<T>();
}

template <class T>
concept bool Arithmetic() {
  return std::is_arithmetic<T>::value && Scalar<T>() && TotallyOrdered<T>();
}

}

template <class T>
concept bool Integral() {
  return std::is_integral<T>::value && ext::Arithmetic<T>();
}

#else

template <class T>
concept bool Integral() {
  return std::is_integral<T>::value;
}
#endif

// 20150712: Not to spec.
template <class T>
concept bool SignedIntegral() {
  return Integral<T>() && (T(-1) < T(0));
}

template <class T>
concept bool UnsignedIntegral() {
  return Integral<T>() && !SignedIntegral<T>();
}

// Integral<T> subsumes SignedIntegral<T> and UnsignedIntegral<T>
// SignedIntegral<T> and UnsignedIntegral<T> are mutually exclusive

namespace ext { namespace models {

template <class>
constexpr bool destructible() { return false; }
Destructible{T}
constexpr bool destructible() { return true; }

template <class T, class...Args>
constexpr bool constructible_object() { return false; }
template <class T, class...Args>
  requires ConstructibleObject<T, Args...>
constexpr bool constructible_object() { return true; }

template <class T, class...Args>
constexpr bool bindable_reference() { return false; }
template <class T, class...Args>
  requires BindableReference<T, Args...>
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
template <Swappable T>
constexpr bool swappable() { return true; }
template <class, class>
constexpr bool swappable() { return false; }
template <class T, class U>
  requires Swappable<T, U>()
constexpr bool swappable() { return true; }

template <class>
constexpr bool integral() { return false; }
Integral{T}
constexpr bool integral() { return true; }

template <class>
constexpr bool signed_integral() { return false; }
SignedIntegral{T}
constexpr bool signed_integral() { return true; }

template <class>
constexpr bool unsigned_integral() { return false; }
UnsignedIntegral{T}
constexpr bool unsigned_integral() { return true; }

}}}} // namespace stl2::v1::ext::models

#endif // STL2_CONCEPTS_OBJECT_HPP
