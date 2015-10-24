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
#ifndef STL2_DETAIL_CONCEPTS_CALLABLE_HPP
#define STL2_DETAIL_CONCEPTS_CALLABLE_HPP

#include <functional>

#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/function.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/functional/invoke.hpp>

///////////////////////////////////////////////////////////////////////////
// Indirect callable requirements [indirectcallables]
//
STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // FunctionType [functiontype.indirectcallables]
  // Not to spec: Expects the adapted function to be an lvalue, since that's
  //              how the algorithms use it.
  //
  template <class F>
    requires CopyConstructible<decay_t<F>>() &&
      Constructible<decay_t<F>, F&&>()
  using FunctionType = decltype(ext::make_callable_wrapper(declval<F>()));

  ///////////////////////////////////////////////////////////////////////////
  // Callable [Extension]
  //
  namespace ext {
    template <class F, class...Args>
    concept bool Callable =
      Function<FunctionType<F>, Args...>();

    Callable{F, ...Args}
    using CallableResultType = result_t<FunctionType<F>, Args...>;
  }

  namespace models {
    template <class F, class...Args>
    constexpr bool Callable = false;
    __stl2::ext::Callable{F, ...Args}
    constexpr bool Callable<F, Args...> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // RegularCallable [Extension]
  //
  namespace ext {
    template <class F, class...Args>
    concept bool RegularCallable =
      RegularFunction<FunctionType<F>, Args...>();

    RegularCallable{F, ...Args}
    using RegularCallableResultType = CallableResultType<F, Args...>;
  }

  namespace models {
    template <class F, class...Args>
    constexpr bool RegularCallable = false;
    __stl2::ext::RegularCallable{F, ...Args}
    constexpr bool RegularCallable<F, Args...> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // CallablePredicate [Extension]
  //
  namespace ext {
    template <class F, class...Args>
    concept bool CallablePredicate =
      Predicate<FunctionType<F>, Args...>();
  }

  namespace models {
    template <class F, class...Args>
    constexpr bool CallablePredicate = false;
    __stl2::ext::CallablePredicate{F, ...Args}
    constexpr bool CallablePredicate<F, Args...> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // CallableRelation [Extension]
  //
  namespace ext {
    template <class F, class T>
    concept bool CallableRelation() {
      return Relation<FunctionType<F>, T>();
    }

    template <class F, class T, class U>
    concept bool CallableRelation() {
      return Relation<FunctionType<F>, T, U>();
    }
  }

  namespace models {
    template <class F, class T, class U = T>
    constexpr bool CallableRelation = false;
    __stl2::ext::CallableRelation{F, T}
    constexpr bool CallableRelation<F, T, T> = true;
    __stl2::ext::CallableRelation{F, T, U}
    constexpr bool CallableRelation<F, T, U> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // CallableStrictWeakOrder [Extension]
  //
  namespace ext {
    template <class F, class T>
    concept bool CallableStrictWeakOrder() {
      return StrictWeakOrder<FunctionType<F>, T>();
    }

    template <class F, class T, class U>
    concept bool CallableStrictWeakOrder() {
      return StrictWeakOrder<FunctionType<F>, T, U>();
    }
  }

  namespace models {
    template <class F, class T, class U = T>
    constexpr bool CallableStrictWeakOrder = false;
    __stl2::ext::CallableStrictWeakOrder{F, T}
    constexpr bool CallableStrictWeakOrder<F, T, T> = true;
    __stl2::ext::CallableStrictWeakOrder{F, T, U}
    constexpr bool CallableStrictWeakOrder<F, T, U> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Indirect callables [indirectfunc.indirectcallables]
  //
  template <class, class...> struct __predicate : false_type {};
  Predicate{F, ...Args} struct __predicate<F, Args...> : true_type {};

  template <class...T> struct __common_reference : meta::bool_<sizeof...(T)==1> {};
  template <class T, class U, class...Rest> requires CommonReference<T,U>()
  struct __common_reference<T,U,Rest...> : __common_reference<CommonReferenceType<T,U>,Rest...> {};

  template <Readable...Is>
  using __iter_args_lists =
    meta::push_back<
      meta::cartesian_product<
        meta::list<meta::list<ValueType<Is>, ReferenceType<Is>>...>>,
      meta::list<iter_common_reference_t<Is>...>>;

  template <typename MapFn, typename ReduceFn>
  using __iter_map_reduce_fn =
    meta::compose<
      meta::uncurry<meta::on<ReduceFn, meta::uncurry<MapFn>>>,
      meta::quote<__iter_args_lists>>;

  template <class F, class...Is>
  concept bool IndirectCallable() {
    return
      //(Readable<Is>() &&...) &&
      models::AllReadable<Is...> &&
      // The following 3 are checked redundantly, but are called out
      // specifically for better error messages on concept check failure.
      ext::Callable<F, ValueType<Is>...> &&
      ext::Callable<F, ReferenceType<Is>...> &&
      ext::Callable<F, iter_common_reference_t<Is>...> &&
      // redundantly checks the above 3 requirements
      meta::_v<meta::apply<
        __iter_map_reduce_fn<
          meta::bind_front<meta::quote<ext::CallableResultType>, F>,
          meta::quote<__common_reference>>,
        Is...>>;
  }

  namespace models {
    template <class, class...>
    constexpr bool IndirectCallable = false;
    __stl2::IndirectCallable{F, ...Is}
    constexpr bool IndirectCallable<F, Is...> = true;
  }

  IndirectCallable{F, ...Is}
  using IndirectCallableResultType =
    ext::CallableResultType<F, ValueType<Is>...>;

  template <class F, class...Is>
  concept bool IndirectRegularCallable() {
    return
      //(Readable<Is>() &&...) &&
      models::AllReadable<Is...> &&
      // The following 3 are checked redundantly, but are called out
      // specifically for better error messages on concept check failure.
      ext::RegularCallable<F, ValueType<Is>...> &&
      ext::RegularCallable<F, ReferenceType<Is>...> &&
      ext::RegularCallable<F, iter_common_reference_t<Is>...> &&
      // redundantly checks the above 3 requirements
      meta::_v<meta::apply<
        __iter_map_reduce_fn<
          meta::bind_front<meta::quote<ext::RegularCallableResultType>, F>,
          meta::quote<__common_reference>>,
        Is...>>;
  }

  namespace models {
    template <class, class...>
    constexpr bool IndirectRegularCallable = false;
    __stl2::IndirectRegularCallable{F, ...Is}
    constexpr bool IndirectRegularCallable<F, Is...> = true;
  }

  template <class F, class...Is>
  concept bool IndirectCallablePredicate() {
    return
      //(Readable<Is>() &&...) &&
      models::AllReadable<Is...> &&
      // The following 3 are checked redundantly, but are called out
      // specifically for better error messages on concept check failure.
      ext::CallablePredicate<F, ValueType<Is>...> &&
      ext::CallablePredicate<F, ReferenceType<Is>...> &&
      ext::CallablePredicate<F, iter_common_reference_t<Is>...> &&
      // redundantly checks the above 3 requirements
      meta::_v<meta::apply<
        __iter_map_reduce_fn<
          meta::bind_front<meta::quote<__predicate>, FunctionType<F>>,
          meta::quote<meta::fast_and>>,
        Is...>>;
  }

  namespace models {
    template <class, class...>
    constexpr bool IndirectCallablePredicate = false;
    __stl2::IndirectCallablePredicate{F, ...Is}
    constexpr bool IndirectCallablePredicate<F, Is...> = true;
  }

  template <class F, class I1, class I2 = I1>
  concept bool IndirectCallableRelation() {
    return
      Readable<I1>() && Readable<I2>() &&
      ext::CallableRelation<F, ValueType<I1>, ValueType<I2>>() &&
      ext::CallableRelation<F, ValueType<I1>, ReferenceType<I2>>() &&
      ext::CallableRelation<F, ReferenceType<I1>, ValueType<I2>>() &&
      ext::CallableRelation<F, ReferenceType<I1>, ReferenceType<I2>>() &&
      ext::CallableRelation<F, iter_common_reference_t<I1>, iter_common_reference_t<I2>>();
  }

  namespace models {
    template <class F, class I1, class I2 = I1>
    constexpr bool IndirectCallableRelation = false;
    __stl2::IndirectCallableRelation{F, I1, I2}
    constexpr bool IndirectCallableRelation<F, I1, I2> = true;
  }

  template <class F, class I1, class I2 = I1>
  concept bool IndirectCallableStrictWeakOrder() {
    return
      Readable<I1>() && Readable<I2>() &&
      ext::CallableStrictWeakOrder<F, ValueType<I1>, ValueType<I2>>() &&
      ext::CallableStrictWeakOrder<F, ValueType<I1>, ReferenceType<I2>>() &&
      ext::CallableStrictWeakOrder<F, ReferenceType<I1>, ValueType<I2>>() &&
      ext::CallableStrictWeakOrder<F, ReferenceType<I1>, ReferenceType<I2>>() &&
      ext::CallableStrictWeakOrder<F, iter_common_reference_t<I1>, iter_common_reference_t<I2>>();
  }

  namespace models {
    template <class F, class I1, class I2 = I1>
    constexpr bool IndirectCallableStrictWeakOrder = false;
    __stl2::IndirectCallableStrictWeakOrder{F, I1, I2}
    constexpr bool IndirectCallableStrictWeakOrder<F, I1, I2> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Projected [projected.indirectcallables]
  //
  template <Readable I, IndirectRegularCallable<I> Proj>
  struct Projected {
    using value_type = decay_t<IndirectCallableResultType<Proj, I>>;
    auto operator*() const ->
      ext::CallableResultType<Proj, ReferenceType<I>>;
  };

  template <WeaklyIncrementable I, class Proj>
  struct difference_type<Projected<I, Proj>> :
    difference_type<I> {};
} STL2_CLOSE_NAMESPACE

#endif
