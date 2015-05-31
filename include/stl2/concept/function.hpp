#ifndef STL2_CONCEPT_FUNCTION_HPP
#define STL2_CONCEPT_FUNCTION_HPP

#include <stl2/detail/config.hpp>
#include <stl2/concept/foundational.hpp>

namespace stl2 { namespace v1 {

////////////////////
// Function Concepts
//
template <class F, class...Args>
using ResultType =
  std::result_of_t<F(Args...)>;

template <class F, class...Args>
concept bool Function =
  Destructible<F> &&
  CopyConstructible<F> &&
  requires(F& f, Args&&...args) {
    typename ResultType<F, Args...>;
    f((Args&&)(args)...);
    requires Same<ResultType<F, Args...>, decltype(f((Args&&)(args)...))>;
    &f; requires Same<F*,decltype(&f)>;
    { f.~F() } noexcept;
    new F; requires Same<F*,decltype(new F)>;
    delete new F;
  };

template <class F, class...Args>
concept bool RegularFunction =
  Function<F, Args...>;

template <class F, class...Args>
concept bool Predicate =
  RegularFunction<F, Args...> &&
  requires(F& f, Args&&...args) {
    { f((Args&&)(args)...) } -> Boolean;
  };

template <class R, class T, class U = T>
concept bool WeakRelation =
  Predicate<R, T, T> &&
  (Same<T, U> ||
    (Predicate<R, T, U> &&
     Predicate<R, U, T> &&
     Predicate<R, U, U>));

template <class R, class T, class U = T>
concept bool Relation =
  WeakRelation<R, T, U> &&
  (Same<T, U> ||
    (Common<T, U> &&
     Predicate<R, CommonType<T, U>,
                  CommonType<T, U>>));

}} // namespace stl2::v1

#endif // STL2_CONCEPT_FUNCTION_HPP
