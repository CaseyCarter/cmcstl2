#ifndef STL2_DETAIL_CONCEPTS_CORE_HPP
#define STL2_DETAIL_CONCEPTS_CORE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>

////////////////////////////////////////
// Core Concepts [concepts.lib.corelang]
//

STL2_OPEN_NAMESPACE {
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
  }

  namespace models {
    template <class...>
    constexpr bool Same = false;
    template <>
    constexpr bool Same<> = true;
    template <class T, detail::_Same<T>...Rest>
    constexpr bool Same<T, Rest...> = true;
  }

  template <class...Ts>
  concept bool Same() {
    return models::Same<Ts...>;
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

  namespace models {
    template <class, class>
    constexpr bool DerivedFrom = false;
    __stl2::DerivedFrom{T, U}
    constexpr bool DerivedFrom<T, U> = true;
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
  }

  namespace models {
    template <class, class>
    constexpr bool ImplicitlyConvertibleTo = false;
    __stl2::ext::ImplicitlyConvertibleTo{T, U}
    constexpr bool ImplicitlyConvertibleTo<T, U> = true;

    template <class, class>
    constexpr bool ExplicitlyConvertibleTo = false;
    __stl2::ext::ExplicitlyConvertibleTo{T, U}
    constexpr bool ExplicitlyConvertibleTo<T, U> = true;
  }

  template <class T, class U>
  concept bool ConvertibleTo() {
    return ext::ExplicitlyConvertibleTo<T, U>() &&
      ext::ImplicitlyConvertibleTo<T, U>();
    // Axiom: implicit and explicit conversion have equal results.
  }

  namespace models {
    template <class, class>
    constexpr bool ConvertibleTo = false;
    __stl2::ConvertibleTo{T, U}
    constexpr bool ConvertibleTo<T, U> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // PubliclyDerivedFrom [Extension]
  //
  namespace ext {
    template <class T, class U>
    concept bool PubliclyDerivedFrom() {
      return ConvertibleTo<T, U>() && (Same<T, U>() || DerivedFrom<T, U>());
    }
  }

  namespace models {
    template <class, class>
    constexpr bool PubliclyDerivedFrom = false;
    __stl2::ext::PubliclyDerivedFrom{T, U}
    constexpr bool PubliclyDerivedFrom<T, U> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Assignable [concepts.lib.corelang.assignable]
  //
  template <class T, class U>
    concept bool Assignable() {
    return requires (T&& t, U&& u) {
      STL2_EXACT_TYPE_CONSTRAINT((T&&)t = (U&&)u, T&);
    };
  }

  namespace models {
    template <class, class>
    constexpr bool Assignable = false;
    __stl2::Assignable{T, U}
    constexpr bool Assignable<T, U> = true;
  }
} STL2_CLOSE_NAMESPACE

#endif
