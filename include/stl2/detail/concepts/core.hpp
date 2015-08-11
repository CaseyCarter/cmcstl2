#ifndef STL2_DETAIL_CONCEPTS_CORE_HPP
#define STL2_DETAIL_CONCEPTS_CORE_HPP

#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>

////////////////////////////////////////
// Core Concepts [concepts.lib.corelang]
//

#if defined(__GNUC__)
#define STL2_IS_SAME_AS(T, U) __is_same_as(T, U)
#define STL2_IS_BASE_OF(T, U) __is_base_of(T, U)
#else
#define STL2_IS_SAME_AS(T, U) _Is<T, std::is_same, U>
#define STL2_IS_BASE_OF(T, U) _Is<T, std::is_base_of, U>
#endif

namespace stl2 { inline namespace v1 {

template <template <class...> class T, class... U>
concept bool _Valid = requires { typename T<U...>; };

template <class U, template <class...> class T, class... V>
concept bool _Is = _Valid<T, U, V...> && T<U, V...>::value;

template <class U, template <class...> class T, class... V>
concept bool _IsNot = !_Is<U, T, V...>;

namespace detail {
template <class...>
struct all_same : std::true_type {};

template <class T, class...Rest>
struct all_same<T, Rest...> :
  // meta::bool_<STL2_IS_SAME_AS(T, Rest) && ... && true>
  meta::and_c<STL2_IS_SAME_AS(T, Rest)...> {};
}

// 20150714: Conforming extension: variadic.
template <class...Ts>
concept bool Same() {
  return detail::all_same<Ts...>::value;
}

template <class T>
concept bool _Decayed = Same<T, std::decay_t<T>>();

template <class T, class U>
concept bool DerivedFrom() {
  return STL2_IS_BASE_OF(U, T);
}

namespace ext {

template <class T, class U>
concept bool ExplicitlyConvertibleTo() {
  return requires (T&& t) { static_cast<U>((T&&)t); };
}

template <class T, class U>
concept bool ImplicitlyConvertibleTo() {
  // Q: Why not { t } -> U ?
  // A: They do not have equivalent results as of 20150724,
  //    which I think is a bug.
  return _Is<T, std::is_convertible, U>;
}

} // namespace ext

// 20150715: not to spec. The draft specifies that ConvertibleTo has
// the same value as is_convertible, but Casey strongly suspects
// that we (a) want Same to subsume ConvertibleTo, and (b) want
// ConvertibleTo to require both implicit and explicit conversion
// with equal results.
template <class T, class U>
concept bool ConvertibleTo() {
  return ext::ExplicitlyConvertibleTo<T, U>() &&
    ext::ImplicitlyConvertibleTo<T, U>();
}

namespace ext {

// "PubliclyAndUnambiguouslyDerivedFrom" would be a very long name.
template <class T, class U>
concept bool PubliclyDerivedFrom() {
  return ConvertibleTo<T, U>() && (Same<T, U>() || DerivedFrom<T, U>());
}

} // namespace ext

template <class T, class U>
concept bool Assignable() {
  return requires (T&& t, U&& u) {
    STL2_EXACT_TYPE_CONSTRAINT((T&&)t = (U&&)u, T&);
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
  return ExplicitlyConvertibleTo<U, T>();
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

template <class...>
constexpr bool same() { return false; }
Same{...Ts}
constexpr bool same() { return true; }

template <class, class>
constexpr bool convertible_to() { return false; }
ConvertibleTo{T, U}
constexpr bool convertible_to() { return true; }

template <class, class>
constexpr bool derived_from() { return false; }
DerivedFrom{T, U}
constexpr bool derived_from() { return true; }

template <class, class>
constexpr bool publicly_derived_from() { return false; }
PubliclyDerivedFrom{T, U}
constexpr bool publicly_derived_from() { return true; }

template <class, class>
constexpr bool assignable() { return false; }
Assignable{T, U}
constexpr bool assignable() { return true; }

template <class>
constexpr bool core_constructible() { return false; }
template <core::Constructible T>
constexpr bool core_constructible() { return true; }
template <class, class>
constexpr bool core_constructible() { return false; }
template <class T, class U>
  requires core::Constructible<T, U>()
constexpr bool core_constructible() { return true; }
template <class, class, class, class...>
constexpr bool core_constructible() { return false; }
template <class T, class U, class V, class...Args>
  requires core::Constructible<T, U, V, Args...>()
constexpr bool core_constructible() { return true; }

}}}} // namespace stl2::v1::ext::models

#endif
