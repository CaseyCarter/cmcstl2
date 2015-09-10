// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_TYPE_TRAITS
#define STL2_TYPE_TRAITS

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // common_type
  //
  template <class T>
  struct __unary {
    template <class U>
    using apply = meta::apply<T, U>;
  };

  template <class T, class X = remove_reference_t<T>>
  using __cref = add_lvalue_reference_t<add_const_t<X>>;

  template <class T, class U>
  using __cond = decltype(true ? declval<T>() : declval<U>());

  template <class From, class To>
  struct __copy_cv_ : meta::id<To> { };
  template <class From, class To>
  struct __copy_cv_<From const, To> : add_const<To> { };
  template <class From, class To>
  struct __copy_cv_<From volatile, To> : add_volatile<To> { };
  template <class From, class To>
  struct __copy_cv_<From const volatile, To> : add_cv<To> { };
  template <class From, class To>
  using __copy_cv = meta::_t<__copy_cv_<From, To>>;

  template <class T, class U>
  struct __builtin_common { };
  template <class T, class U>
  using __builtin_common_t = meta::_t<__builtin_common<T, U>>;
  template <class T, class U>
    requires _Valid<__cond, __cref<T>, __cref<U>>
  struct __builtin_common<T, U> :
    decay<__cond<__cref<T>, __cref<U>>> { };
  template <class T, class U, class R = __builtin_common_t<T &, U &>>
  using __rref_res = conditional_t<meta::_v<is_reference<R>>,
    remove_reference_t<R> &&, R>;
  template <class T, class U>
    requires _Valid<__builtin_common_t, T &, U &>
      && ConvertibleTo<T &&, __rref_res<T, U>>()
      && ConvertibleTo<U &&, __rref_res<T, U>>()
  struct __builtin_common<T &&, U &&> : meta::id<__rref_res<T, U>> { };
  template <class T, class U>
  using __lref_res = __cond<__copy_cv<T, U> &, __copy_cv<U, T> &>;
  template <class T, class U>
  struct __builtin_common<T &, U &> : meta::defer<__lref_res, T, U> { };
  template <class T, class U>
    requires _Valid<__builtin_common_t, T &, U const &>
      && ConvertibleTo<U &&, __builtin_common_t<T &, U const &>>()
  struct __builtin_common<T &, U &&> :
    __builtin_common<T &, U const &> { };
  template <class T, class U>
  struct __builtin_common<T &&, U &> : __builtin_common<U &, T &&> { };

  // common_type
  template <class ...Ts>
  struct common_type { };

  template <class... T>
  using common_type_t = meta::_t<common_type<T...>>;

  template <class T>
  struct common_type<T> : decay<T> { };

  template <class T, class U>
  struct __common_type2
    : common_type<decay_t<T>, decay_t<U>> { };

  template <_Decayed T, _Decayed U>
  struct __common_type2<T, U> : __builtin_common<T, U> { };

  template <class T, class U>
  struct common_type<T, U>
    : __common_type2<T, U> { };

  template <class T, class U, class V, class... W>
    requires _Valid<common_type_t, T, U>
  struct common_type<T, U, V, W...>
    : common_type<common_type_t<T, U>, V, W...> { };

  ///////////////////////////////////////////////////////////////////////////
  // common_reference machinery
  //
  namespace __qual {
    using __rref = meta::quote<add_rvalue_reference_t>;
    using __lref = meta::quote<add_lvalue_reference_t>;
    template <class>
    struct __xref : meta::id<meta::compose<meta::quote<meta::_t>, meta::quote<meta::id>>> { };
    template <class T>
    struct __xref<T&> : meta::id<meta::compose<__lref, meta::_t<__xref<T>>>> { };
    template <class T>
    struct __xref<T&&> : meta::id<meta::compose<__rref, meta::_t<__xref<T>>>> { };
    template <class T>
    struct __xref<const T> : meta::id<meta::quote<add_const_t>> { };
    template <class T>
    struct __xref<volatile T> : meta::id<meta::quote<add_volatile_t>> { };
    template <class T>
    struct __xref<const volatile T> : meta::id<meta::quote<add_cv_t>> { };
  }

  template <class T, class U, template <class> class TQual,
    template <class> class UQual>
  struct basic_common_reference { };

  template <class T, class U>
  using __basic_common_reference =
    basic_common_reference<__uncvref<T>, __uncvref<U>,
      __unary<meta::_t<__qual::__xref<T>>>::template apply,
      __unary<meta::_t<__qual::__xref<U>>>::template apply>;

  // common_reference
  template <class... T>
  struct common_reference { };

  template <class... T>
  using common_reference_t = meta::_t<common_reference<T...>>;

  template <class T>
  struct common_reference<T> : meta::id<T> { };

  template <class T, class U>
  struct __common_reference2
    : meta::if_<meta::has_type<__basic_common_reference<T, U>>,
        __basic_common_reference<T, U>, common_type<T, U>> { };

  template <class T, class U>
    requires _Valid<__builtin_common_t, T, U>
      && _Is<__builtin_common_t<T, U>, is_reference>
  struct __common_reference2<T, U> : __builtin_common<T, U> { };

  template <class T, class U>
  struct common_reference<T, U> : __common_reference2<T, U> { };

  template <class T, class U, class V, class... W>
    requires _Valid<common_reference_t, T, U>
  struct common_reference<T, U, V, W...>
    : common_reference<common_reference_t<T, U>, V, W...> { };

  template <class T, class U>
  concept bool CommonReference() {
    return requires (T (&t)(), U (&u)()) {
      typename common_reference_t<T, U>;
      typename common_reference_t<U, T>;
      requires Same<common_reference_t<T, U>,
                    common_reference_t<U, T>>();
      common_reference_t<T, U>(t());
      common_reference_t<T, U>(u());
    };
  }

  namespace models {
    template <class, class>
    constexpr bool CommonReference = false;
    __stl2::CommonReference{T, U}
    constexpr bool CommonReference<T, U> = true;
  }

  CommonReference{T, U}
  using CommonReferenceType =
    common_reference_t<T, U>;

  // Casey strongly suspects that we want Same to subsume Common
  // (See https://github.com/ericniebler/stl2/issues/50).
  template <class T, class U>
  concept bool Common() {
    return requires (T (&t)(), U (&u)()) {
      typename common_type_t<T, U>;
      typename common_type_t<U, T>;
      requires Same<common_type_t<T, U>,
                    common_type_t<U, T>>();
      common_type_t<T, U>(t());
      common_type_t<T, U>(u());
      requires CommonReference<add_lvalue_reference_t<common_type_t<T, U>>,
                               CommonReferenceType<add_lvalue_reference_t<const T>,
                                                   add_lvalue_reference_t<const U>>>();
    };
  }

  namespace models {
    template <class, class>
    constexpr bool Common = false;
    __stl2::Common{T, U}
    constexpr bool Common<T, U> = true;
  }

  Common{T, U}
  using CommonType = common_type_t<T, U>;
} STL2_CLOSE_NAMESPACE

#endif
