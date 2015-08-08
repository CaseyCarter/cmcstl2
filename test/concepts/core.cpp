#include "validate.hpp"

#if VALIDATE_RANGES
namespace models {
template <class...Ts>
using same = ranges::Same<Ts...>;

template <class T, class U>
using convertible_to = ranges::ConvertibleTo<T, U>;

template <class First, class Second>
using common = ranges::Common<First, Second>;
}

namespace ns {
template <class T, class U>
using CommonType = ranges::common_type_t<T, U>;
}

#elif VALIDATE_STL2
#include <stl2/detail/concepts/core.hpp>
#include <stl2/utility.hpp>

namespace ns {
using stl2::CommonType;
}
#endif

#include <iostream>
#include <type_traits>

#include "../simple_test.hpp"

//CONCEPT_ASSERT(models::same<>());
CONCEPT_ASSERT(models::same<int, int>());
CONCEPT_ASSERT(models::same<double, double>());
//CONCEPT_ASSERT(models::same<double>());
CONCEPT_ASSERT(!models::same<double, int>());
CONCEPT_ASSERT(!models::same<int, double>());

//CONCEPT_ASSERT(models::same<int, int, int, int>());
//CONCEPT_ASSERT(!models::same<int, int, double, int>());

#if VALIDATE_STL2
namespace publicly_derived_from_test {
using stl2::ext::models::publicly_derived_from;

struct A {};
struct B : A {};
struct C : A {};
struct D : B, C {};

CONCEPT_ASSERT(publicly_derived_from<int,int>());
CONCEPT_ASSERT(publicly_derived_from<A,A>());
CONCEPT_ASSERT(publicly_derived_from<B,B>());
CONCEPT_ASSERT(publicly_derived_from<C,C>());
CONCEPT_ASSERT(publicly_derived_from<D,D>());
CONCEPT_ASSERT(publicly_derived_from<B,A>());
CONCEPT_ASSERT(publicly_derived_from<C,A>());
CONCEPT_ASSERT(!publicly_derived_from<A,B>());
CONCEPT_ASSERT(!publicly_derived_from<A,C>());
CONCEPT_ASSERT(!publicly_derived_from<A,D>());
CONCEPT_ASSERT(!publicly_derived_from<D,A>());
CONCEPT_ASSERT(!publicly_derived_from<int,void>());
}
#endif

namespace convertible_to_test {
struct A {};
struct B : A {};

CONCEPT_ASSERT(models::convertible_to<A, A>());
CONCEPT_ASSERT(models::convertible_to<B, B>());
CONCEPT_ASSERT(!models::convertible_to<A, B>());
CONCEPT_ASSERT(models::convertible_to<B, A>());
CONCEPT_ASSERT(models::convertible_to<int, double>());
CONCEPT_ASSERT(models::convertible_to<double, int>());
}

namespace common_test {
CONCEPT_ASSERT(models::same<ns::CommonType<int, int>, int>());
//CONCEPT_ASSERT(models::same<ns::CommonType<int, float, double>, double>());

CONCEPT_ASSERT(models::common<int, int>());
CONCEPT_ASSERT(models::common<int, double>());
CONCEPT_ASSERT(models::common<double, int>());
CONCEPT_ASSERT(models::common<double, double>());
CONCEPT_ASSERT(!models::common<void, int>());
CONCEPT_ASSERT(!models::common<int*, int>());
CONCEPT_ASSERT(models::common<void*, int*>());
CONCEPT_ASSERT(models::common<double,long long>());
//CONCEPT_ASSERT(models::common<int, float, double>());
//CONCEPT_ASSERT(!models::common<int, float, double, void>());

struct B {};
struct C { C() = default; C(B) {} C(int) {} };
CONCEPT_ASSERT(models::common<B,C>());
//CONCEPT_ASSERT(models::common<int, C, B>());
}

// range-v3 does not have core::Constructible. (STL2 probably should not either) 
#if VALIDATE_STL2
namespace core_constructible_test {

template <class T, class U>
  requires stl2::ext::core::Constructible<T, U>()
constexpr bool f() { return false; }

template <class T, stl2::ConvertibleTo<T> >
constexpr bool f() { return true; }

CONCEPT_ASSERT(f<int,int>());
CONCEPT_ASSERT(f<int,long>());

struct A {
  A(int);
};
struct B {
  explicit B(int);
};
CONCEPT_ASSERT(f<A, int>());
CONCEPT_ASSERT(!f<B, int>());
}
#endif

namespace {
struct tag {};
struct A { A() = default; A(int) {} explicit A(tag) {} };
struct B : A {};
struct C : B {};

enum class result {
  exact, publicly_derived, convertible,
  explicitly_convertible, unrelated
};

#if VALIDATE_STL2
// Concepts version
result f(A) {
  std::cout << "exactly A\n";
  return result::exact;
}

result f(stl2::ext::PubliclyDerivedFrom<A>) {
  std::cout << "Publicly derived from A\n";
  return result::publicly_derived;
}

result f(stl2::ConvertibleTo<A>) {
  std::cout << "Implicitly convertible to A\n";
  return result::convertible;
}

  result f(stl2::ext::ExplicitlyConvertibleTo<A>) {
  std::cout << "Explicitly convertible to A\n";
  return result::explicitly_convertible;
}

result f(auto) {
  std::cout << "Nothing to do with A\n";
  return result::unrelated;
}

#else
// C++11 version
result f(A) {
  std::cout << "exactly A\n";
  return result::exact;
}

template <class T>
meta::if_c<
  std::is_base_of<A, T>::value &&
    std::is_convertible<T,A>::value &&
    !std::is_same<A,T>::value,
   result>
f(T) {
  std::cout << "Publicly derived from A\n";
  return result::publicly_derived;
}

template <class T>
meta::if_c<
  std::is_convertible<T,A>::value &&
    !(std::is_base_of<A,T>::value ||
      std::is_same<A,T>::value),
  result>
f(T) {
  std::cout << "Implicitly convertible to A\n";
  return result::convertible;
}

template <class T>
meta::if_c<
  std::is_constructible<A,T>::value &&
    !(std::is_convertible<T,A>::value ||
      std::is_base_of<A,T>::value ||
      std::is_same<A,T>::value),
  result>
f(T) {
  std::cout << "Explicitly convertible to A\n";
  return result::explicitly_convertible;
}

template <class T>
meta::if_c<
  !(std::is_constructible<A,T>::value ||
    std::is_convertible<T,A>::value ||
    std::is_base_of<A,T>::value ||
    std::is_same<A,T>::value),
  result>
f(T) {
  std::cout << "Nothing to do with A\n";
  return result::unrelated;
}
#endif
} // unnamed namespace

int main() {
  CHECK(f(A{}) == result::exact);
  { const A a{}; CHECK(f(a) == result::exact); }
  CHECK(f(B{}) == result::publicly_derived);
  { const B b{}; CHECK(f(b) == result::publicly_derived); }
  CHECK(f(42) == result::convertible);
  CHECK(f(tag{}) == result::explicitly_convertible);
  CHECK(f("foo") == result::unrelated);

  return ::test_result();
}
