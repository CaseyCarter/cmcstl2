#ifndef STL2_DETAIL_CONCEPTS_CALLABLE_HPP
#define STL2_DETAIL_CONCEPTS_CALLABLE_HPP

#include <functional>

#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/function.hpp>

///////////////////////////////////////////////////////////////////////////
// Indirect callable requirements [indirectcallables]
//
STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // FunctionType [functiontype.indirectcallables]
  // Not (exactly) to spec: I think the Constructible requirement is
  //     implicit. Also, as_function must be exported so that users can use
  //     Callables.
  //
  template <class T>
    requires Constructible<T, T&&>()
  T as_function(T&& t) {
    return __stl2::forward<T>(t);
  }

  template <class T, _Is<is_member_pointer> = decay_t<T>>
  auto as_function(T&& t) {
    return std::mem_fn(t);
  }

  template <class T>
  using FunctionType =
    decltype(as_function(declval<T>()));

  ///////////////////////////////////////////////////////////////////////////
  // Indirect callables [indirectfunc.indirectcallables]
  //
  template <class, class...> struct __function : false_type {};
  Function{F, ...Args} struct __function<F, Args...> : true_type {};

  template <class, class...> struct __regular_function : false_type {};
  RegularFunction{F, ...Args} struct __regular_function<F, Args...> : true_type {};

  template <class, class...> struct __predicate : false_type {};
  Predicate{F, ...Args} struct __predicate<F, Args...> : true_type {};

  template <class, class I1, class I2 = I1> struct __relation : false_type {};
  Relation{F, I1, I2} struct __relation<F, I1, I2> : true_type {};

  template <class, class I1, class I2 = I1> struct __strict_weak_order : false_type {};
  StrictWeakOrder{F, I1, I2} struct __strict_weak_order<F, I1, I2> : true_type {};

  template <class...T> struct __common_reference : meta::bool_<sizeof...(T)==1> {};
  template <class T, class U, class...Rest> requires CommonReference<T,U>()
  struct __common_reference<T,U,Rest...> : __common_reference<CommonReferenceType<T,U>,Rest...> {};

  template <class...Is>
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
      Function<FunctionType<F>, ValueType<Is>...>() &&
      Function<FunctionType<F>, ReferenceType<Is>...>() &&
      Function<FunctionType<F>, iter_common_reference_t<Is>...>() &&
      meta::_v<meta::apply<  // redundantly checks the above 3 requirements
        __iter_map_reduce_fn<
          meta::bind_front<meta::quote<__function>, FunctionType<F>>,
          meta::quote<meta::fast_and>>,
        Is...>> &&
      meta::_v<meta::apply<
        __iter_map_reduce_fn<
          meta::bind_front<meta::quote<ResultType>, FunctionType<F>>,
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
    ResultType<FunctionType<F>, ValueType<Is>...>;

  template <class F, class...Is>
  concept bool IndirectRegularCallable() {
    return
      //(Readable<Is>() &&...) &&
      models::AllReadable<Is...> &&
      // The following 3 are checked redundantly, but are called out
      // specifically for better error messages on concept check failure.
      RegularFunction<FunctionType<F>, ValueType<Is>...>() &&
      RegularFunction<FunctionType<F>, ReferenceType<Is>...>() &&
      RegularFunction<FunctionType<F>, iter_common_reference_t<Is>...>() &&
      meta::_v<meta::apply<  // redundantly checks the above 3 requirements
        __iter_map_reduce_fn<
          meta::bind_front<meta::quote<__regular_function>, FunctionType<F>>,
          meta::quote<meta::fast_and>>,
        Is...>> &&
      meta::_v<meta::apply<
        __iter_map_reduce_fn<
          meta::bind_front<meta::quote<ResultType>, FunctionType<F>>,
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
      Predicate<FunctionType<F>, ValueType<Is>...>() &&
      Predicate<FunctionType<F>, ReferenceType<Is>...>() &&
      Predicate<FunctionType<F>, iter_common_reference_t<Is>...>() &&
      meta::_v<meta::apply< // redundantly checks the above 3 requirements
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
      Relation<FunctionType<F>, ValueType<I1>, ValueType<I2>>() &&
      Relation<FunctionType<F>, ValueType<I1>, ReferenceType<I2>>() &&
      Relation<FunctionType<F>, ReferenceType<I1>, ValueType<I2>>() &&
      Relation<FunctionType<F>, ReferenceType<I1>, ReferenceType<I2>>() &&
      Relation<FunctionType<F>, iter_common_reference_t<I1>, iter_common_reference_t<I2>>();
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
      StrictWeakOrder<FunctionType<F>, ValueType<I1>, ValueType<I2>>() &&
      StrictWeakOrder<FunctionType<F>, ValueType<I1>, ReferenceType<I2>>() &&
      StrictWeakOrder<FunctionType<F>, ReferenceType<I1>, ValueType<I2>>() &&
      StrictWeakOrder<FunctionType<F>, ReferenceType<I1>, ReferenceType<I2>>() &&
      StrictWeakOrder<FunctionType<F>, iter_common_reference_t<I1>, iter_common_reference_t<I2>>();
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
    using value_type =
      decay_t<ResultType<FunctionType<Proj>, ValueType<I>>>;
    auto operator*() const ->
      ResultType<FunctionType<Proj>, ReferenceType<I>>;
  };

  template <class I, class Proj>
  struct difference_type<Projected<I, Proj>> :
    difference_type<I> { };
} STL2_CLOSE_NAMESPACE

#endif
