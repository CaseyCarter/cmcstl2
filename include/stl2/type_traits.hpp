#ifndef STL2_TYPE_TRAITS
#define STL2_TYPE_TRAITS

#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>

namespace stl2 { inline namespace v1 {
///////////////////////////////////////////////////////////////////////////
// <type_traits> imports
//
using std::integral_constant;
using std::bool_constant;
using std::true_type;
using std::false_type;

using std::is_void;
using std::is_null_pointer;
using std::is_integral;
using std::is_floating_point;
using std::is_array;
using std::is_pointer;
using std::is_lvalue_reference;
using std::is_rvalue_reference;
using std::is_member_object_pointer;
using std::is_member_function_pointer;
using std::is_enum;
using std::is_union;
using std::is_class;
using std::is_function;

using std::is_reference;
using std::is_arithmetic;
using std::is_fundamental;
using std::is_object;
using std::is_scalar;
using std::is_compound;
using std::is_member_pointer;

using std::is_const;
using std::is_volatile;
using std::is_trivial;
using std::is_trivially_copyable;
using std::is_standard_layout;
using std::is_pod;
using std::is_literal_type;
using std::is_empty;
using std::is_polymorphic;
using std::is_abstract;
using std::is_final;

using std::is_signed;
using std::is_unsigned;

using std::is_destructible;
using std::is_trivially_destructible;
using std::is_nothrow_destructible;

using std::is_constructible;
using std::is_trivially_constructible;
using std::is_nothrow_constructible;

using std::is_default_constructible;
using std::is_trivially_default_constructible;
using std::is_nothrow_default_constructible;

using std::is_copy_constructible;
using std::is_trivially_copy_constructible;
using std::is_nothrow_copy_constructible;

using std::is_move_constructible;
using std::is_trivially_move_constructible;
using std::is_nothrow_move_constructible;

using std::is_assignable;
using std::is_trivially_assignable;
using std::is_nothrow_assignable;

using std::is_copy_assignable;
using std::is_trivially_copy_assignable;
using std::is_nothrow_copy_assignable;

using std::is_move_assignable;
using std::is_trivially_move_assignable;
using std::is_nothrow_move_assignable;

using std::has_virtual_destructor;

using std::alignment_of;
using std::rank;
using std::extent;

using std::is_same;
using std::is_base_of;
using std::is_convertible;

using std::remove_const;
using std::remove_const_t;
using std::remove_volatile;
using std::remove_volatile_t;
using std::remove_cv;
using std::remove_cv_t;
using std::add_const;
using std::add_const_t;
using std::add_volatile;
using std::add_volatile_t;
using std::add_cv;
using std::add_cv_t;

using std::remove_reference;
using std::remove_reference_t;
using std::add_lvalue_reference;
using std::add_lvalue_reference_t;
using std::add_rvalue_reference;
using std::add_rvalue_reference_t;

using std::make_signed;
using std::make_signed_t;
using std::make_unsigned;
using std::make_unsigned_t;

using std::remove_extent;
using std::remove_extent_t;
using std::remove_all_extents;
using std::remove_all_extents_t;

using std::remove_pointer;
using std::remove_pointer_t;
using std::add_pointer;
using std::add_pointer_t;

using std::aligned_storage;
using std::aligned_storage_t;
using std::aligned_union;
using std::aligned_union_t;
using std::decay;
using std::decay_t;
using std::enable_if;
using std::enable_if_t;
using std::conditional;
using std::conditional_t;
using std::underlying_type;
using std::underlying_type_t;
using std::result_of;
using std::result_of_t;
using std::void_t;

///////////////////////////////////////////////////////////////////////////
// common_type
//
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
  requires _Valid<__cond, __cref<T>, __cref<U>>
struct __builtin_common<T, U> :
  std::decay<__cond<__cref<T>, __cref<U>>> { };
template <class T, class U, class R = __builtin_common_t<T &, U &>>
using __rref_res = std::conditional_t<meta::_v<std::is_reference<R>>,
  std::remove_reference_t<R> &&, R>;
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
struct common_type<T> : std::decay<T> { };

template <class T, class U>
struct __common_type2
  : common_type<std::decay_t<T>, std::decay_t<U>> { };

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
struct __common_reference2
  : meta::if_<meta::has_type<__basic_common_reference<T, U>>,
      __basic_common_reference<T, U>, common_type<T, U>> { };

template <class T, class U>
  requires _Valid<__builtin_common_t, T, U>
    && meta::_v<std::is_reference<__builtin_common_t<T, U>>>
struct __common_reference2<T, U> : __builtin_common<T, U> { };

template <class T, class U>
struct common_reference<T, U> : __common_reference2<T, U> { };

template <class T, class U, class V, class... W>
  requires _Valid<common_reference_t, T, U>
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

template <class, class>
constexpr bool common_reference() { return false; }
CommonReference{T, U}
constexpr bool common_reference() { return true; }
}}}} // namespace stl2::v1::ext::models

#endif
