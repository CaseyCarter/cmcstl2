#ifndef STL2_DETAIL_CONCEPTS_CORE_HPP
#define STL2_DETAIL_CONCEPTS_CORE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>

////////////////////////////////////////
// Core Concepts [concepts.lib.corelang]
//

namespace stl2 { inline namespace v1 {

template <template <class...> class T, class... U>
concept bool _Valid = requires { typename T<U...>; };

template <class U, template <class...> class T, class... V>
concept bool _Is = _Valid<T, U, V...> && T<U, V...>::value;

template <class U, template <class...> class T, class... V>
concept bool _IsNot = !_Is<U, T, V...>;

template <template <class...> class Trait, class...Ts>
concept bool _AllAre =
  meta::_v<meta::all_of<meta::list<Ts...>, meta::quote_trait<Trait>>>;

///////////////////////////////////////////////////////////////////////////
// Same [concepts.lib.corelang.same]
// Extension: variadic.
//
namespace detail {
template <class T, class U>
concept bool _Same =
#if defined(__GNUC__)
  __is_same_as(T, U);
#else
  _Is<T, is_same, U>;
#endif

template <class...>
constexpr bool AllSame = false;
template <>
constexpr bool AllSame<> = true;
template <class T, _Same<T>...Rest>
constexpr bool AllSame<T, Rest...> = true;
}

template <class...Ts>
concept bool Same() {
  return detail::AllSame<Ts...>;
}

template <class T>
concept bool _Decayed = Same<T, decay_t<T>>();

template <class T>
concept bool _Unqual = Same<T, __uncvref<T>>();

///////////////////////////////////////////////////////////////////////////
// DerivedFrom [concepts.lib.corelang.derived]
//
template <class T, class U>
concept bool DerivedFrom() {
#if defined(__GNUC__)
  return __is_base_of(U, T);
#else
  return _Is<U, is_base_of, T>;
#endif
}

///////////////////////////////////////////////////////////////////////////
// ConvertibleTo [concepts.lib.corelang.convertibleto]
// Not to spec: Requires both implicit and explicit conversion with
//              equal results.
//
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
  return _Is<T, is_convertible, U>;
}
} // namespace ext

template <class T, class U>
concept bool ConvertibleTo() {
  return ext::ExplicitlyConvertibleTo<T, U>() &&
    ext::ImplicitlyConvertibleTo<T, U>();
  // Axiom: implicit and explicit conversion have equal results.
}

///////////////////////////////////////////////////////////////////////////
// PubliclyDerivedFrom [Extension]
//
namespace ext {
template <class T, class U>
concept bool PubliclyDerivedFrom() {
  return ConvertibleTo<T, U>() && (Same<T, U>() || DerivedFrom<T, U>());
}
} // namespace ext

///////////////////////////////////////////////////////////////////////////
// Assignable [concepts.lib.corelang.assignable]
//
template <class T, class U>
concept bool Assignable() {
  return requires (T&& t, U&& u) {
    STL2_EXACT_TYPE_CONSTRAINT((T&&)t = (U&&)u, T&);
  };
}

namespace ext { namespace models {
template <class...>
constexpr bool same() { return false; }
Same{...Ts}
constexpr bool same() { return true; }

template <class, class>
constexpr bool derived_from() { return false; }
DerivedFrom{T, U}
constexpr bool derived_from() { return true; }

template <class, class>
constexpr bool convertible_to() { return false; }
ConvertibleTo{T, U}
constexpr bool convertible_to() { return true; }

template <class, class>
constexpr bool assignable() { return false; }
Assignable{T, U}
constexpr bool assignable() { return true; }

template <class, class>
constexpr bool publicly_derived_from() { return false; }
PubliclyDerivedFrom{T, U}
constexpr bool publicly_derived_from() { return true; }
}}}} // namespace stl2::v1::ext::models

#endif
