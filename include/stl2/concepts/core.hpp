#ifndef STL2_CONCEPTS_CORE_HPP
#define STL2_CONCEPTS_CORE_HPP

#include <stl2/detail/config.hpp>
#include <stl2/detail/fwd.hpp>

#include <meta/meta.hpp>
#include <type_traits>

////////////////
// Core Concepts
//

#if defined(__GNUC__)
#define STL2_IS_SAME_AS(T, U) __is_same_as(T, U)
#define STL2_IS_BASE_OF(T, U) __is_base_of(T, U)
#else
#define STL2_IS_SAME_AS(T, U) std::is_same<T, U>::value
#define STL2_IS_BASE_OF(T, U) std::is_base_of<T, U>::value
#endif

namespace stl2 { namespace v1 { namespace concepts {

template <class T, class U>
concept bool Same =
  STL2_IS_SAME_AS(T, U);
// Types T and U model Same iff T and U are the same type.

template <class T, class U>
concept bool Derived =
  STL2_IS_BASE_OF(U, T);
// Types T and U model Derived iff T and U are class types
// and U is a base of T

template <class T, class U>
concept bool ExplicitlyConvertible =
  Same<T, U> ||
  requires(T&& t) {
    static_cast<U>(forward<T>(t));
  };

template <class T, class U>
concept bool Convertible =
  ExplicitlyConvertible<T, U> &&
  (Same<T, U> || std::is_convertible<T, U>::value);

template <class T, class U>
concept bool PubliclyDerived =
  Same<T, U> ||
  (Derived<T, U> && Convertible<T, U>);

template <class T, class U>
using CommonType =
  meta::let<
    meta::if_c<STL2_IS_SAME_AS(T, U), T,
      meta::defer_trait<std::common_type, T, U>>
  >;

template <class T, class U>
concept bool Common =
  Same<T, U> ||
  requires {
    typename CommonType<T, U>;
    typename CommonType<U, T>;
    requires Same<CommonType<T, U>, CommonType<U, T>>;
    requires ExplicitlyConvertible<T, CommonType<T, U>>;
    requires ExplicitlyConvertible<U, CommonType<T, U>>;
  };

// Same<T, U> subsumes Convertible<T, U> and PubliclyDerived<T, U> and Common<T, U>
// PubliclyDerived<T, U> subsumes Convertible<T, U>
// Convertible<T, U> (and transitively Same<T, U> and PubliclyDerived<T, U>) subsumes ExplicitlyConvertible<T, U>

template <class T>
concept bool Destructible =
  std::is_object<T>::value &&
  requires(T& t) {
    { t.~T() } noexcept;
  };

template <class T, class...Args>
concept bool Constructible =
  ExplicitlyConvertible<Args..., T> ||
  // requires(Args&&...args) { T(forward<Args>(args)...); }; // ICE
  std::is_constructible<T, Args...>::value;

// ExplictlyConvertible<T, U> (and transitively Convertible<T, U>, PubliclyDerived<T, U>,
// and Same<T, U>) subsumes Constructible<U, T>

template <class T>
concept bool DefaultConstructible =
  Constructible<T>;

template <class T>
concept bool MoveConstructible =
  Constructible<T, T&&>;

template <class T>
concept bool CopyConstructible =
  MoveConstructible<T> &&
  Constructible<T, T&> &&
  Constructible<T, const T&> &&
  Constructible<T, const T&&>;

template <class T, class U>
concept bool Assignable =
  requires(T& t, U&& u) {
    t = forward<U>(u);
    requires Same<T&, decltype(t = forward<U>(u))>;
  };

template <class T, class U>
concept bool AssignableTo =
  Assignable<U, T>;

template <class T>
concept bool MoveAssignable =
  Assignable<T, T&&>;

template <class T>
concept bool CopyAssignable =
  MoveAssignable<T> &&
  Assignable<T, T&> &&
  Assignable<T, const T&> &&
  Assignable<T, const T&&>;

#undef STL2_IS_SAME_AS
#undef STL2_IS_BASE_OF

namespace models {

template <class, class>
constexpr bool same() { return false; }

template <class T, Same<T> >
constexpr bool same() { return true; }


template <class, class>
constexpr bool convertible() { return false; }

template <class T, class U>
  requires Convertible<T, U>
constexpr bool convertible() { return true; }


template <class, class>
constexpr bool publicly_derived() { return false; }

template <class T, class U>
  requires PubliclyDerived<T, U>
constexpr bool publicly_derived() { return true; }


template <class, class>
constexpr bool common() { return false; }

template <class T, Common<T> >
constexpr bool common() { return true; }


template <class>
constexpr bool destructible() { return false; }

template <Destructible>
constexpr bool destructible() { return true; }


template <class, class...>
constexpr bool constructible() { return false; }

Constructible{T, ...Args}
constexpr bool constructible() { return false; }


template <class>
constexpr bool copy_constructible() { return false; }

template <CopyConstructible>
constexpr bool copy_constructible() { return true; }

}}}} // namespace stl2::v1::concepts::models

#endif // STL2_CONCEPTS_CORE_HPP
