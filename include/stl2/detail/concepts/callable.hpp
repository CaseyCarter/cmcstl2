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

STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // Indirect callables [indirectfunc.indirectcallables]
  //
  template <class...T> struct __common_reference : meta::bool_<sizeof...(T)==1> {};
  template <class T, class U, class...Rest> requires CommonReference<T,U>()
  struct __common_reference<T,U,Rest...> : __common_reference<CommonReferenceType<T,U>,Rest...> {};

  template <Readable...Is>
  using __iter_args_lists =
    meta::push_back<
      meta::cartesian_product<
        meta::list<meta::list<value_type_t<Is>, reference_t<Is>>...>>,
      meta::list<iter_common_reference_t<Is>...>>;

  template <typename MapFn, typename ReduceFn>
  using __iter_map_reduce_fn =
    meta::compose<
      meta::uncurry<meta::on<ReduceFn, meta::uncurry<MapFn>>>,
      meta::quote<__iter_args_lists>>;

  Callable{F, ...Args}
  using __callable_result_t =
    result_of_t<F&(Args...)>;

  template <class F, class...Is>
  concept bool IndirectCallable() {
    return
      (Readable<Is>() &&...&& true) &&
      // The following 3 are checked redundantly, but are called out
      // specifically for better error messages on concept check failure.
      Callable<F, value_type_t<Is>...>() &&
      Callable<F, reference_t<Is>...>() &&
      Callable<F, iter_common_reference_t<Is>...>() &&
      // redundantly checks the above 3 requirements
      meta::_v<meta::apply<
        __iter_map_reduce_fn<
          meta::bind_front<meta::quote<__callable_result_t>, F>,
          meta::quote<__common_reference>>,
        Is...>>;
  }

  namespace models {
    template <class, class...>
    constexpr bool IndirectCallable = false;
    __stl2::IndirectCallable{F, ...Is}
    constexpr bool IndirectCallable<F, Is...> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // indirect_result_of [indirectcallables.indirectfunc]
  //
  template <class>
  struct indirect_result_of {};

  // Not to spec: The function type must be decayed before being constrained.
  template <class F, class...Is>
    requires models::IndirectCallable<__f<F>, Is...>
  struct indirect_result_of<F(Is...)> :
    result_of<F(value_type_t<Is>...)> {};

  template <class T>
  using indirect_result_of_t =
    meta::_t<indirect_result_of<T>>;

  template <class F, class...Is>
  concept bool IndirectRegularCallable() {
    return IndirectCallable<F, Is...>();
  }

  namespace models {
    template <class, class...>
    constexpr bool IndirectRegularCallable = false;
    __stl2::IndirectRegularCallable{F, ...Is}
    constexpr bool IndirectRegularCallable<F, Is...> = true;
  }

  template <class, class...> struct __predicate : false_type {};
  Predicate{F, ...Args} struct __predicate<F, Args...> : true_type {};

  template <class F, class...Is>
  concept bool IndirectCallablePredicate() {
    return
      (Readable<Is>() &&...&& true) &&
      // The following 3 are checked redundantly, but are called out
      // specifically for better error messages on concept check failure.
      Predicate<F, value_type_t<Is>...>() &&
      Predicate<F, reference_t<Is>...>() &&
      Predicate<F, iter_common_reference_t<Is>...>() &&
      // redundantly checks the above 3 requirements
      meta::_v<meta::apply<
        __iter_map_reduce_fn<
          meta::bind_front<meta::quote<__predicate>, F>,
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
      Relation<F, value_type_t<I1>, value_type_t<I2>>() &&
      Relation<F, value_type_t<I1>, reference_t<I2>>() &&
      Relation<F, reference_t<I1>, value_type_t<I2>>() &&
      Relation<F, reference_t<I1>, reference_t<I2>>() &&
      Relation<F, iter_common_reference_t<I1>, iter_common_reference_t<I2>>();
  }

  namespace models {
    template <class F, class I1, class I2 = I1>
    constexpr bool IndirectCallableRelation = false;
    __stl2::IndirectCallableRelation{F, I1, I2}
    constexpr bool IndirectCallableRelation<F, I1, I2> = true;
  }

  template <class F, class I1, class I2 = I1>
  concept bool IndirectCallableStrictWeakOrder() {
    return IndirectCallableRelation<F, I1, I2>();
  }

  namespace models {
    template <class F, class I1, class I2 = I1>
    constexpr bool IndirectCallableStrictWeakOrder = false;
    __stl2::IndirectCallableStrictWeakOrder{F, I1, I2}
    constexpr bool IndirectCallableStrictWeakOrder<F, I1, I2> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // projected [projected.indirectcallables]
  //
  template <Readable I, IndirectRegularCallable<I> Proj>
  struct projected {
    using value_type = decay_t<indirect_result_of_t<Proj&(I)>>;
    auto operator*() const ->
      result_of_t<Proj&(reference_t<I>)>;
  };

  template <WeaklyIncrementable I, class Proj>
  struct difference_type<projected<I, Proj>> :
    difference_type<I> {};
} STL2_CLOSE_NAMESPACE

#endif
