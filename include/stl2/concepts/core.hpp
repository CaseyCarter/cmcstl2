#ifndef STL2_CONCEPTS_CORE_HPP
#define STL2_CONCEPTS_CORE_HPP

#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/common_type.hpp>

////////////////////////////////////////
// Core Concepts [concepts.lib.corelang]
//

// Subsumption relationships (#ASCIIart):
//
//                                           /- ext::ExplicitlyConvertible <= ext::core::Constructible
//     /- ext::PubliclyDerived <= Convertible
// Same                                      \- ext::ImplicitlyConvertible
//     \- Common

#if defined(__GNUC__)
#define STL2_IS_SAME_AS(T, U) __is_same_as(T, U)
#define STL2_IS_BASE_OF(T, U) __is_base_of(T, U)
#else
#define STL2_IS_SAME_AS(T, U) std::is_same<T, U>::value
#define STL2_IS_BASE_OF(T, U) std::is_base_of<T, U>::value
#endif

namespace stl2 { inline namespace v1 {

// Types T and U model Same iff T and U are the same type.
template <class T, class U>
concept bool Same() {
  return STL2_IS_SAME_AS(T, U);
}

// Types T and U model Derived iff T and U are class types
// and U is a base of T
template <class T, class U>
concept bool Derived() {
  return STL2_IS_BASE_OF(U, T);
}

namespace ext {

template <class T, class U>
concept bool ExplicitlyConvertible() {
  return Same<T, U>() ||
    requires (T&& t) {
      static_cast<U>(stl2::forward<T>(t));
    };
}

template <class T, class U>
concept bool ImplicitlyConvertible() {
  return Same<T, U>() ||
    std::is_convertible<T, U>::value; // Maybe { t } -> U ?
}

} // namespace ext

// Convertible is not to spec as of 20150711. The draft specifies
// that Convertible has the same value as is_convertible, but Casey
// strongly suspects that we (a) want Same to subsume Convertible,
// and (b) want Convertible to require both implicit and explicit
// conversion with equal results.
template <class T, class U>
concept bool Convertible() {
  return ext::ExplicitlyConvertible<T, U>() &&
    ext::ImplicitlyConvertible<T, U>();
}

namespace ext {

// "PubliclyAndUnambiguouslyDerived" would be a very long name.
template <class T, class U>
concept bool PubliclyDerived() {
  return Same<T, U>() ||
    (Derived<T, U>() && Convertible<T, U>());
}

} // namespace ext

template <class T, class U>
using CommonType =
  meta::let<
    meta::if_c<STL2_IS_SAME_AS(T, U), T,
      meta::defer_trait<stl2::common_type, T, U>>>;

// Common is not to spec as of 20150711. Casey strongly suspects
// that we want Same to subsume Common (see the discussion at
// https://github.com/ericniebler/stl2/issues/50).
template <class T, class U>
concept bool Common() {
  return Same<T, U>() ||
    requires {
      typename CommonType<T, U>;
      typename CommonType<U, T>;
      requires Same<CommonType<T, U>, CommonType<U, T>>();
      requires ext::ExplicitlyConvertible<T, CommonType<T, U>>();
      requires ext::ExplicitlyConvertible<U, CommonType<T, U>>();
    };
}

template <class T, class U>
concept bool Assignable() {
  return requires (T&& t, U&& u) {
    STL2_EXACT_TYPE_CONSTRAINT(stl2::forward<T>(t) = stl2::forward<U>(u), T&);
  };
}

namespace ext { namespace core {

template <class T>
concept bool Constructible() {
  return requires {
    T{ }; // not equality preserving
  };
}

template <class T, class U>
concept bool Constructible() {
  return ExplicitlyConvertible<U, T>() ||
    requires (U&& u) {
      T{ (U&&)u }; // not equality preserving
    };
}

template <class T, class U, class V, class...Args>
concept bool Constructible() {
  return requires (U&& u, V&& v, Args&&...args) {
    T{ (U&&)u, (V&&)v, (Args&&)args... }; // not equality preserving
  };
}

}} // namespace ext::core

#undef STL2_IS_SAME_AS
#undef STL2_IS_BASE_OF

namespace ext { namespace models {

template <class, class>
constexpr bool same() { return false; }
Same{T, U}
constexpr bool same() { return true; }

template <class, class>
constexpr bool convertible() { return false; }
Convertible{T, U}
constexpr bool convertible() { return true; }

template <class, class>
constexpr bool publicly_derived() { return false; }
PubliclyDerived{T, U}
constexpr bool publicly_derived() { return true; }

template <class, class>
constexpr bool common() { return false; }
Common{T, U}
constexpr bool common() { return true; }

template <class, class>
constexpr bool assignable() { return false; }
Assignable{T, U}
constexpr bool assignable() { return true; }

template <class>
constexpr bool core_constructible() { return false; }
template <class T>
  requires core::Constructible<T>()
constexpr bool core_constructible() { return false; }
template <class, class>
constexpr bool core_constructible() { return false; }
template <class T, class U>
  requires core::Constructible<T, U>()
constexpr bool core_constructible() { return false; }
template <class, class, class...>
constexpr bool core_constructible() { return false; }
template <class T, class U, class V, class...Args>
  requires core::Constructible<T, U, V, Args...>()
constexpr bool core_constructible() { return false; }

}}}} // namespace stl2::v1::ext::models

#endif // STL2_CONCEPTS_CORE_HPP
