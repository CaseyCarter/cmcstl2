#ifndef STL2_COMMON_TYPE_HPP
#define STL2_COMMON_TYPE_HPP

#include <utility>
#include <type_traits>

#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/core.hpp>

#include <meta/meta.hpp>

namespace stl2 { inline namespace v1 {

template <class T>
struct __unary {
  template <class U>
  using apply = meta::apply<T, U>;
};

template <class T, class X = std::remove_reference_t<T>>
using __cref = std::add_lvalue_reference_t<std::add_const_t<X>>;
template <class T>
using __uncvref = std::remove_cv_t<std::remove_reference_t<T>>;

template <class T, class U>
using __cond = decltype(true ? declval<T>() : declval<U>());

template <class From, class To>
struct __copy_cv_ : meta::id<To> { };
template <class From, class To>
struct __copy_cv_<From const, To> : std::add_const<To> { };
template <class From, class To>
struct __copy_cv_<From volatile, To> : std::add_volatile<To> { };
template <class From, class To>
struct __copy_cv_<From const volatile, To> : std::add_cv<To> { };
template <class From, class To>
using __copy_cv = meta::_t<__copy_cv_<From, To>>;

template <class T, class U>
struct __builtin_common { };
template <class T, class U>
using __builtin_common_t = meta::_t<__builtin_common<T, U>>;
template <class T, class U>
  requires requires { typename __cond<__cref<T>, __cref<U>>; }
struct __builtin_common<T, U> :
  std::decay<__cond<__cref<T>, __cref<U>>> { };
template <class T, class U, class R = __builtin_common_t<T &, U &>>
using __rref_res = std::conditional_t<meta::_v<std::is_reference<R>>,
  std::remove_reference_t<R> &&, R>;
template <class T, class U>
  requires requires { typename __rref_res<T, U>; }
    && meta::_v<std::is_convertible<T &&, __rref_res<T, U>>>
    && meta::_v<std::is_convertible<U &&, __rref_res<T, U>>>
struct __builtin_common<T &&, U &&> : meta::id<__rref_res<T, U>> { };
template <class T, class U>
using __lref_res = __cond<__copy_cv<T, U> &, __copy_cv<U, T> &>;
template <class T, class U>
struct __builtin_common<T &, U &> : meta::defer<__lref_res, T, U> { };
template <class T, class U>
  requires requires { typename __builtin_common_t<T &, U const &>; }
    && meta::_v<std::is_convertible<U &&, __builtin_common_t<T &, U const &>>>
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
struct common_type<T> : std::decay<T> { };

template <class T, class U>
struct common_type<T, U>
  : common_type<std::decay_t<T>, std::decay_t<U>> { };

template <class T>
concept bool _Decayed = meta::_v<std::is_same<std::decay_t<T>, T>>;

template <_Decayed T, _Decayed U>
struct common_type<T, U> : __builtin_common<T, U> { };

template <class T, class U, class V, class... W>
  requires requires { typename common_type_t<T, U>; }
struct common_type<T, U, V, W...>
  : common_type<common_type_t<T, U>, V, W...> { };

namespace __qual {
  using __rref = meta::quote<std::add_rvalue_reference_t>;
  using __lref = meta::quote<std::add_lvalue_reference_t>;
  template <class>
  struct __xref : meta::id<meta::compose<meta::quote<meta::_t>, meta::quote<meta::id>>> { };
  template <class T>
  struct __xref<T&> : meta::id<meta::compose<__lref, meta::_t<__xref<T>>>> { };
  template <class T>
  struct __xref<T&&> : meta::id<meta::compose<__rref, meta::_t<__xref<T>>>> { };
  template <class T>
  struct __xref<const T> : meta::id<meta::quote<std::add_const_t>> { };
  template <class T>
  struct __xref<volatile T> : meta::id<meta::quote<std::add_volatile_t>> { };
  template <class T>
  struct __xref<const volatile T> : meta::id<meta::quote<std::add_cv_t>> { };
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
struct common_reference<T, U>
  : meta::if_<meta::has_type<__basic_common_reference<T, U>>,
      __basic_common_reference<T, U>, common_type<T, U>> { };

template <class T, class U>
  requires requires {typename __builtin_common_t<T, U>;} &&
    meta::_v<std::is_reference<__builtin_common_t<T, U>>>
struct common_reference<T, U> : __builtin_common<T, U> { };

template <class T, class U, class V, class... W>
  requires requires { typename common_reference_t<T, U>; }
struct common_reference<T, U, V, W...>
  : common_reference<common_reference_t<T, U>, V, W...> { };

template <class T, class U>
using CommonReferenceType =
  stl2::common_reference_t<T, U>;

template <class T, class U>
concept bool CommonReference() {
  return
    requires (T&& t, U&& u) {
      typename CommonReferenceType<T, U>;
      typename CommonReferenceType<U, T>;
      requires Same<CommonReferenceType<T, U>,
                    CommonReferenceType<U, T>>();
      CommonReferenceType<T, U>(stl2::forward<T>(t));
      CommonReferenceType<T, U>(stl2::forward<U>(u));
    };
}

template <class T, class U>
using CommonType = common_type_t<T, U>;

// Casey strongly suspects that we want Same to subsume Common
// (See https://github.com/ericniebler/stl2/issues/50).
template <class T, class U>
concept bool Common() {
  return CommonReference<const T&, const U&>() &&
    requires (T&& t, U&& u) {
      typename CommonType<T, U>;
      typename CommonType<U, T>;
      requires Same<CommonType<T, U>,
                    CommonType<U, T>>();
      CommonType<T, U>(std::forward<T>(t));
      CommonType<T, U>(std::forward<U>(u));
      requires CommonReference<CommonType<T, U>&,
                               CommonReferenceType<const T&, const U&>>();
    };
}

namespace ext { namespace models {
template <class, class>
constexpr bool common() { return false; }
Common{T, U}
constexpr bool common() { return true; }
}}}}

#endif
