// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_CONCEPTS_OBJECT_HPP
#define STL2_DETAIL_CONCEPTS_OBJECT_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>

////////////////////////////////////////
// Object Concepts [concepts.lib.object]
//
STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // Destructible [concepts.lib.object.destructible]
  // Not to spec: is_object and accepting the parameters by reference are
  //              necessary to prevent hard errors in the requires clause
  //              with odd types.
  namespace models {
    template <class>
    constexpr bool Destructible = false;
    template <class T>
      requires _Is<T, is_object> && requires (T& t, const T& ct, T* const p) {
        { t.~T() } noexcept;
        requires Same<decltype(&t),T*>;
        requires Same<decltype(&ct),const T*>;
        delete p;
        delete[] p;
      }
    constexpr bool Destructible<T> = true;
  }

  template <class T>
  concept bool Destructible() {
    return models::Destructible<T>;
  }

  namespace models {
    // Destructible<Ts>() && ...
    template <class...Ts>
    constexpr bool AllDestructible = false;
    template <__stl2::Destructible...Ts>
    constexpr bool AllDestructible<Ts...> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Constructible [concepts.lib.object.constructible]
  // Extension: ConstructibleObject and BindableReference are user-visible.
  //
  namespace ext {
    template <class T, class...Args>
    constexpr bool __constructible_object = false;
    template <class T, class...Args>
      requires requires (Args&&...args) {
        T{ (Args&&)args... };
        new T{ (Args&&)args... };
      }
    constexpr bool __constructible_object<T, Args...> = true;

    template <class T, class...Args>
    concept bool ConstructibleObject =
      Destructible<T>() && __constructible_object<T, Args...>;

    // 20150718: Not to spec: spec is broken.
    // FIXME: Correct wording.
    template <class T, class...Args>
    concept bool BindableReference =
      _Is<T, is_reference> &&
      // requires (Args&&...args) { T{ (Args&&)args... }; };
      // requires (Args&&...args) { T( (Args&&)args... ); };
      _Is<T, is_constructible, Args...>;
  }

  namespace models {
    template <class T, class...Args>
    constexpr bool ConstructibleObject = false;
    __stl2::ext::ConstructibleObject{T, ...Args}
    constexpr bool ConstructibleObject<T, Args...> = true;

    template <class T, class...Args>
    constexpr bool BindableReference = false;
    __stl2::ext::BindableReference{T, ...Args}
    constexpr bool BindableReference<T, Args...> = true;

    template <class T, class...Args>
    constexpr bool Constructible =
      ConstructibleObject<T, Args...> ||
      BindableReference<T, Args...>;
  }

  template <class T, class...Args>
  concept bool Constructible() {
    return models::Constructible<T, Args...>;
  }

  ///////////////////////////////////////////////////////////////////////////
  // DefaultConstructible [concepts.lib.object.defaultconstructible]
  // Note: Due to implementation variance around DR1518, may not correctly
  // enforce the requirement that the default constructor be non-explicit.
  //
  template <class T>
  constexpr bool __default_constructible = false;
  template <class T>
    requires requires (const std::size_t n) {
      new T[n]{}; // not required to be equality preserving
    }
  constexpr bool __default_constructible<T> = true;

  template <class T>
  concept bool DefaultConstructible() {
    return Constructible<T>() && __default_constructible<T>;
  }

  namespace models {
    template <class>
    constexpr bool DefaultConstructible = false;
    __stl2::DefaultConstructible{T}
    constexpr bool DefaultConstructible<T> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // MoveConstructible [concepts.lib.object.moveconstructible]
  //
  template <class T>
  concept bool MoveConstructible() {
    return Constructible<T, remove_cv_t<T>&&>() &&
      ext::ImplicitlyConvertibleTo<remove_cv_t<T>&&, T>();
  }

  namespace models {
    template <class>
    constexpr bool MoveConstructible = false;
    __stl2::MoveConstructible{T}
    constexpr bool MoveConstructible<T> = true;

    // MoveConstructible<Ts>() && ...
    template <class...Ts>
    constexpr bool AllMoveConstructible = false;
    template <__stl2::MoveConstructible...Ts>
    constexpr bool AllMoveConstructible<Ts...> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // CopyConstructible [concepts.lib.object.copyconstructible]
  //
  template <class T>
  concept bool CopyConstructible() {
    return MoveConstructible<T>() &&
      Constructible<T, const remove_cv_t<T>&>() &&
      ext::ImplicitlyConvertibleTo<remove_cv_t<T>&, T>() &&
      ext::ImplicitlyConvertibleTo<const remove_cv_t<T>&, T>() &&
      ext::ImplicitlyConvertibleTo<const remove_cv_t<T>&&, T>();
  }

  namespace models {
    template <class>
    constexpr bool CopyConstructible = false;
    __stl2::CopyConstructible{T}
    constexpr bool CopyConstructible<T> = true;

    // CopyConstructible<Ts>() && ...
    template <class...Ts>
    constexpr bool AllCopyConstructible = false;
    template <__stl2::CopyConstructible...Ts>
    constexpr bool AllCopyConstructible<Ts...> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Movable [concepts.lib.object.movable]
  //
  template <class T>
  concept bool Movable() {
    return MoveConstructible<T>() &&
      Assignable<T&, T&&>();
  }

  namespace models {
    template <class>
    constexpr bool Movable = false;
    __stl2::Movable{T}
    constexpr bool Movable<T> = true;

    // Movable<Ts>() && ...
    template <class...Ts>
    constexpr bool AllMovable = false;
    template <__stl2::Movable...Ts>
    constexpr bool AllMovable<Ts...> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Copyable [concepts.lib.object.copyable]
  //
  template <class T>
  concept bool Copyable() {
    return CopyConstructible<T>() &&
      Movable<T>() &&
      Assignable<T&, const T&>();
  }

  namespace models {
    template <class>
    constexpr bool Copyable = false;
    __stl2::Copyable{T}
    constexpr bool Copyable<T> = true;

    // Copyable<Ts>() && ...
    template <class...Ts>
    constexpr bool AllCopyable = false;
    template <__stl2::Copyable...Ts>
    constexpr bool AllCopyable<Ts...> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Semiregular [concepts.lib.object.semiregular]
  //
  template <class T>
  concept bool Semiregular() {
    return Copyable<T>() &&
      DefaultConstructible<T>();
  }

  namespace models {
    template <class>
    constexpr bool Semiregular = false;
    __stl2::Semiregular{T}
    constexpr bool Semiregular<T> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Regular [concepts.lib.object.regular]
  //
  template <class T>
  concept bool Regular() {
    return Semiregular<T>() &&
      EqualityComparable<T>();
  }

  namespace models {
    template <class>
    constexpr bool Regular = false;
    __stl2::Regular{T}
    constexpr bool Regular<T> = true;
  }

  namespace ext {
    ///////////////////////////////////////////////////////////////////////////
    // TriviallyFoo concepts
    //
    template <class T>
    concept bool TriviallyDestructible() {
      return Destructible<T>() &&
        _Is<T, is_trivially_destructible>;
    }

    template <class T, class...Args>
    concept bool TriviallyConstructible() {
      return Constructible<T, Args...>() &&
        _Is<T, is_trivially_constructible, Args...>;
    }

    template <class T>
    concept bool TriviallyDefaultConstructible() {
      return DefaultConstructible<T>() &&
        _Is<T, is_trivially_default_constructible>;
    }

    template <class T>
    concept bool TriviallyMoveConstructible() {
      return MoveConstructible<T>() &&
        _Is<T, is_trivially_move_constructible>;
    }

    template <class T>
    concept bool TriviallyCopyConstructible() {
      return CopyConstructible<T>() &&
        TriviallyMoveConstructible<T>() &&
        _Is<T, is_trivially_copy_constructible>;
    }

    template <class T>
    concept bool TriviallyMovable() {
      return Movable<T>() &&
        TriviallyMoveConstructible<T>() &&
        _Is<T, is_trivially_move_assignable>;
    }

    template <class T>
    concept bool TriviallyCopyable() {
      return Copyable<T>() &&
        TriviallyMovable<T>() &&
        TriviallyCopyConstructible<T>() &&
        _Is<T, is_trivially_copy_assignable>;
    }
  }
} STL2_CLOSE_NAMESPACE

#endif
