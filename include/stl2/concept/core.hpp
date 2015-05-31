#ifndef STL2_CONCEPT_CORE_HPP
#define STL2_CONCEPT_CORE_HPP

#include <type_traits>

#include <stl2/detail/config.hpp>
#include <stl2/detail/fwd.hpp>

#if defined(__GNUC__)
#define IS_SAME_AS(T, U) __is_same_as(T, U)
#define IS_BASE_OF(T, U) __is_base_of(T, U)
#else
#define IS_SAME_AS(T, U) std::is_same<T, U>::value
#define IS_BASE_OF(T, U) std::is_base_of<T, U>::value
#endif

namespace stl2 { namespace v1 {
////////////////
// Core Concepts
//
template <class T, class U>
concept bool Same =
  IS_SAME_AS(T, U);
// Types T and U model Same iff T and U are the same type.

template <class T, class U>
concept bool Derived =
  IS_BASE_OF(U, T);
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
  Same<T, U> ||
  (std::is_convertible<T, U>::value &&
   ExplicitlyConvertible<T, U>);

template <class T, class U>
concept bool PubliclyDerived =
  Same<T, U> ||
  (Derived<T, U> &&
   Convertible<T, U>);

template <class T, class U>
using CommonType =
  meta::eval<std::conditional_t<
    IS_SAME_AS(T, U),
    meta::id<T>,
    std::common_type<T, U>
  >>;

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

#undef IS_SAME_AS
#undef IS_BASE_OF

template <class T>
concept bool Destructible =
  std::is_destructible<T>::value;

template <class T, class...Args>
concept bool Constructible =
  ExplicitlyConvertible<Args..., T> ||
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
}} // namespace stl2::v1

#endif // STL2_CONCEPT_CORE_HPP
