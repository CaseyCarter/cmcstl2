#ifndef STL2_DETAIL_CONCEPTS_CALLABLE_HPP
#define STL2_DETAIL_CONCEPTS_CALLABLE_HPP

#include <functional>
#include <type_traits>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/function.hpp>
#include <stl2/iterator.hpp>

///////////////////////////////////////////////////////////////////////////
// Indirect callable requirements [indirectcallables]
//
namespace stl2 { inline namespace v1 {
///////////////////////////////////////////////////////////////////////////
// FunctionType
//
// 20150801: Not (exactly) to spec: I think the Constructible requirement
// is implicit. Also, as_function must be exported so that users can use
// Callables.
template <class T>
  requires Constructible<T, T&&>()
T as_function(T&& t) {
  return stl2::forward<T>(t);
}

template <class T, _Is<std::is_member_pointer> = std::decay_t<T>>
auto as_function(T&& t) {
  return std::mem_fn(t);
}

template <class T>
using FunctionType =
  decltype(as_function(declval<T>()));

///////////////////////////////////////////////////////////////////////////
// Projected
//
// 20150801: Not (exactly) to spec: I think the
// RegularFunction<..., ReferenceType<I>> requirement is implicit.
template <Readable I, class Proj>
  requires RegularFunction<FunctionType<Proj>, ValueType<I>>() &&
    RegularFunction<FunctionType<Proj>, ReferenceType<I>>()
struct Projected {
  using value_type =
    std::decay_t<ResultType<FunctionType<Proj>, ValueType<I>>>;
  [[noreturn]] auto operator*() const ->
    ResultType<FunctionType<Proj>, ReferenceType<I>> {
    std::terminate();
  }
  [[noreturn]] friend auto iter_move(Projected) ->
    ResultType<FunctionType<Proj>, RvalueReferenceType<I>> {
    std::terminate();
  }
};

template <WeaklyIncrementable I, class Proj>
struct difference_type<Projected<I, Proj>> :
  meta::id<DifferenceType<I>> {};

///////////////////////////////////////////////////////////////////////////
// Indirect callables [indirectfunc.indirectcallables]
//
template <class> struct __readable : std::false_type {};
Readable{T} struct __readable<T> : std::true_type {};

template <class, class...> struct __function : std::false_type {};
Function{F, ...Args} struct __function<F, Args...> : std::true_type {};

template <class, class...> struct __regular_function : std::false_type {};
RegularFunction{F, ...Args} struct __regular_function<F, Args...> : std::true_type {};

template <class, class...> struct __predicate : std::false_type {};
Predicate{F, ...Args} struct __predicate<F, Args...> : std::true_type {};

template <class, class I1, class I2 = I1> struct __relation : std::false_type {};
Relation{F, I1, I2} struct __relation<F, I1, I2> : std::true_type {};

template <class, class I1, class I2 = I1> struct __strict_weak_order : std::false_type {};
StrictWeakOrder{F, I1, I2} struct __strict_weak_order<F, I1, I2> : std::true_type {};

template <class...T> struct __common_reference : meta::bool_<sizeof...(T)==1> {};
template <class T, class U, class...Rest> requires CommonReference<T,U>()
struct __common_reference<T,U,Rest...> : __common_reference<CommonReferenceType<T,U>,Rest...> {};

template <class...Is>
using __iter_args_lists =
  meta::push_back<
    meta::cartesian_product<
      meta::list<meta::list<ValueType<Is>, ReferenceType<Is>>...>>,
    meta::list<iter_common_reference_t<Is>...>>;

template<typename MapFn, typename ReduceFn>
using __iter_map_reduce_fn =
  meta::compose<
    meta::uncurry<meta::on<ReduceFn, meta::uncurry<MapFn>>>,
    meta::quote<__iter_args_lists>>;

template <class F, class...Is>
concept bool IndirectCallable() {
  return
    //(Readable<Is>() &&...) &&
    meta::_v<meta::and_<__readable<Is>...>> &&
    // The following 3 are checked redundantly, but are called out
    // specially for better error messages on concept check failure.
    Function<FunctionType<F>, ValueType<Is>...>() &&
    Function<FunctionType<F>, ReferenceType<Is>...>() &&
    Function<FunctionType<F>, iter_common_reference_t<Is>...>() &&
    meta::_v<meta::apply<  // redundantly checkes the above 3 requirements
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

IndirectCallable{F, ...Is}
using IndirectCallableResultType =
  ResultType<FunctionType<F>, ValueType<Is>...>;

template <class F, class...Is>
concept bool IndirectRegularCallable() {
  return
    //(Readable<Is>() &&...) &&
    meta::_v<meta::and_<__readable<Is>...>> &&
    // The following 3 are checked redundantly, but are called out
    // specially for better error messages on concept check failure.
    RegularFunction<FunctionType<F>, ValueType<Is>...>() &&
    RegularFunction<FunctionType<F>, ReferenceType<Is>...>() &&
    RegularFunction<FunctionType<F>, iter_common_reference_t<Is>...>() &&
    meta::_v<meta::apply<  // redundantly checkes the above 3 requirements
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

template <class F, class...Is>
concept bool IndirectCallablePredicate() {
  return
    //(Readable<Is>() &&...) &&
    meta::_v<meta::and_<__readable<Is>...>> &&
    // The following 3 are checked redundantly, but are called out
    // specially for better error messages on concept check failure.
    Predicate<FunctionType<F>, ValueType<Is>...>() &&
    Predicate<FunctionType<F>, ReferenceType<Is>...>() &&
    Predicate<FunctionType<F>, iter_common_reference_t<Is>...>() &&
    meta::_v<meta::apply< // redundantly checkes the above 3 requirements
      __iter_map_reduce_fn<
        meta::bind_front<meta::quote<__predicate>, FunctionType<F>>,
        meta::quote<meta::fast_and>>,
      Is...>>;
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


namespace ext { namespace models {

template <class, class...>
constexpr bool indirect_callable() { return false; }
IndirectCallable{F, ...Args}
constexpr bool indirect_callable() { return true; }

template <class, class...>
constexpr bool indirect_predicate() { return false; }
IndirectCallablePredicate{F, ...Args}
constexpr bool indirect_predicate() { return true; }

#if 0
template <class, class, class = void>
constexpr bool indirect_relation() { return false; }
IndirectCallableRelation{F, I1}
constexpr bool indirect_relation() { return true; }
IndirectCallableRelation{F, I1, I2}
constexpr bool indirect_relation() { return true; }
#endif

}}

}}

#endif
