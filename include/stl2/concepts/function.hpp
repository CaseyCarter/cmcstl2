#ifndef STL2_CONCEPTS_FUNCTION_HPP
#define STL2_CONCEPTS_FUNCTION_HPP

#include <stl2/concepts/compare.hpp>
#include <stl2/concepts/core.hpp>
#include <stl2/concepts/object.hpp>

#include <type_traits>

////////////////////////////////////////////
// Function Concepts [concepts.lib.function]
//
namespace stl2 { inline namespace v1 {

template <class F, class...Args>
using ResultType =
  std::result_of_t<F(Args...)>;

template <class F, class...Args>
concept bool Function() {
  return CopyConstructible<F>() &&
    requires (F& f, Args&&...args) {
      // not equality preserving
      STL2_EXACT_TYPE_CONSTRAINT(f(stl2::forward<Args>(args)...),
                                 ResultType<F, Args...>);
    };
}

template <class F, class...Args>
concept bool RegularFunction() {
  return Function<F, Args...>();
}

template <class F, class...Args>
concept bool Predicate() {
  return RegularFunction<F, Args...> &&
    Boolean<ResultType<F, Args...>>();
}

namespace ext {

template <class R, class T>
concept bool WeakRelation() {
  return Predicate<R, T, T>();
}

template <class R, class T, class U>
concept bool WeakRelation() {
  return WeakRelation<R, T>() &&
    (Same<T, U>() ||
      (WeakRelation<R, U>() &&
       Predicate<R, T, U>() &&
       Predicate<R, U, T>()));
}

}

template <class R, class T>
concept bool Relation() {
  return ext::WeakRelation<R, T>();
}

template <class R, class T, class U>
concept bool Relation() {
  return ext::WeakRelation<R, T, U>() &&
    (Same<T, U>() ||
      (Common<T, U>() &&
       Predicate<R, CommonType<T, U>>()));
}

template <class R, class T>
concept bool StrictWeakOrder() {
  return Relation<R, T>();
}

template <class R, class T, class U>
concept bool StrictWeakOrder() {
  return Relation<R, T, U>();
}

}} // namespace stl2::v1

#endif // STL2_CONCEPTS_FUNCTION_HPP
