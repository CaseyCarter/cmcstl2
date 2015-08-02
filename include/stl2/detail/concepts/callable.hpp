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

template <class T>
  requires _Is<std::decay<T>, std::is_member_pointer>
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
  using value_type = std::decay_t<ResultType<FunctionType<Proj>, ValueType<I>>>;
  ResultType<FunctionType<Proj>, ReferenceType<I>> operator*() const;
};

template <WeaklyIncrementable I, class Proj>
struct difference_type<Projected<I, Proj>> :
  meta::id<DifferenceType<I>> {};

///////////////////////////////////////////////////////////////////////////
// Indirect callables [indirectfunc.indirectcallables]
//
template <class F, class...Is>
using IndirectCallableResultType =
  ResultType<F, ValueType<Is>...>;

namespace detail {
template <class...Ts>
struct all_readable : std::false_type {};

template <>
struct all_readable<> : std::true_type {};

template <Readable First, class...Rest>
struct all_readable<First, Rest...> :
  all_readable<Rest...> {};
}

template <class F, class...Is>
concept bool IndirectCallable() {
  return detail::all_readable<Is...>::value &&
    Function<FunctionType<F>, ValueType<Is>...>();
}

template <class F, class...Is>
concept bool IndirectRegularCallable() {
  return detail::all_readable<Is...>::value &&
    RegularFunction<FunctionType<F>, ValueType<Is>...>();
}

template <class F, class...Is>
concept bool IndirectCallablePredicate() {
  return detail::all_readable<Is...>::value &&
    Predicate<FunctionType<F>, ValueType<Is>...>();
}

template <class F, class I1, class I2 = I1>
concept bool IndirectCallableRelation() {
  return Readable<I1>() && Readable<I2>() &&
    Relation<FunctionType<F>, ValueType<I1>, ValueType<I2>>();
}

template <class F, class I1, class I2 = I1>
concept bool IndirectCallableStrictWeakOrder() {
  return Readable<I1>() && Readable<I2>() &&
    StrictWeakOrder<FunctionType<F>, ValueType<I1>, ValueType<I2>>();
}
}}

#endif
