#ifndef STL2_CONCEPTS_CORE_HPP
#define STL2_CONCEPTS_CORE_HPP

#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/common_type.hpp>

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

namespace stl2 { inline namespace v1 {

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
concept bool ExplicitlyConvertible() { return
  Same<T, U> ||
  requires(T&& t) {
    static_cast<U>(stl2::forward<T>(t));
  };
}

template <class T, class U>
concept bool Convertible =
  Same<T, U> ||
  (ExplicitlyConvertible<T, U>() &&
    std::is_convertible<T, U>::value);

template <class T, class U>
concept bool PubliclyDerived =
  Same<T, U> ||
  (Derived<T, U> && Convertible<T, U>);

template <class T, class U>
using CommonType =
  meta::let<
    meta::if_c<STL2_IS_SAME_AS(T, U), T,
      meta::defer_trait<stl2::common_type, T, U>>>;

template <class T, class U>
concept bool Common =
  Same<T, U> ||
  requires {
    typename CommonType<T, U>;
    typename CommonType<U, T>;
    requires Same<CommonType<T, U>, CommonType<U, T>>;
    requires ExplicitlyConvertible<T, CommonType<T, U>>();
    requires ExplicitlyConvertible<U, CommonType<T, U>>();
  };

// Same<T, U> subsumes Convertible<T, U> and PubliclyDerived<T, U> and Common<T, U>
// PubliclyDerived<T, U> subsumes Convertible<T, U>
// Convertible<T, U> (and transitively Same<T, U> and PubliclyDerived<T, U>) subsumes ExplicitlyConvertible<T, U>

// ExplictlyConvertible<T, U> (and transitively Convertible<T, U>, PubliclyDerived<T, U>,
// and Same<T, U>) subsumes Constructible<U, T>

template <class T, class U>
concept bool Assignable =
  requires(T&& t, U&& u) {
    stl2::forward<T>(t) = stl2::forward<U>(u);
    requires Same<T&, decltype(stl2::forward<T>(t) = stl2::forward<U>(u))>;
  };

template <class T, class U>
concept bool AssignableTo =
  Assignable<U, T>;

namespace core {

template <class T>
concept bool Constructible() { return
  requires {
    T{ };
  };
}

template <class T, class U>
concept bool Constructible() { return
  ExplicitlyConvertible<U, T>() ||
  requires (U&& u) {
    T{ (U&&)u };
  };
}

template <class T, class U, class V, class...Args>
concept bool Constructible() { return
  requires (U&& u, V&& v, Args&&...args) {
    T{ (U&&)u, (V&&)v, (Args&&)args... };
  };
}

} // namespace core

#undef STL2_IS_SAME_AS
#undef STL2_IS_BASE_OF

namespace models {

template <class, class>
constexpr bool same() { return false; }

Same{T, U}
constexpr bool same() { return true; }

template <class, class>
constexpr bool assignable() { return false; }

Assignable{T, U}
constexpr bool assignable() { return true; }

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

//template <class, class...>
//constexpr bool core_constructible() { return false; }
//
//template <class T, class...Args>
//requires core::Constructible<T, Args...>()
//constexpr bool core_constructible() { return false; }

}}} // namespace stl2::v1::models

#endif // STL2_CONCEPTS_CORE_HPP
