// -*- compile-command: "(cd ~/cmcstl2/build && make core_concepts && ./test/core_concepts)" -*-

#include <stl2/concepts/core.hpp>
#include <stl2/utility>

#include <type_traits>
#include <iostream>

#include "copymove.hpp"
#include "simple_test.hpp"

namespace same_test {
using stl2::concepts::models::same;

static_assert(same<int, int>(), "");
static_assert(same<double, double>(), "");
static_assert(!same<double, int>(), "");
static_assert(!same<int, double>(), "");
}

namespace publicly_derived_test {
using stl2::concepts::models::publicly_derived;

struct A {};
struct B : A {};
struct C : A {};
struct D : B, C {};

static_assert(publicly_derived<int,int>(), "");
static_assert(publicly_derived<A,A>(), "");
static_assert(publicly_derived<B,B>(), "");
static_assert(publicly_derived<C,C>(), "");
static_assert(publicly_derived<D,D>(), "");
static_assert(publicly_derived<B,A>(), "");
static_assert(publicly_derived<C,A>(), "");
static_assert(!publicly_derived<A,B>(), "");
static_assert(!publicly_derived<A,C>(), "");
static_assert(!publicly_derived<A,D>(), "");
static_assert(!publicly_derived<D,A>(), "");
static_assert(!publicly_derived<int,void>(), "");
}

namespace convertible_test {
using stl2::concepts::models::convertible;

struct A {};
struct B : A {};

static_assert(convertible<A, A>(), "");
static_assert(convertible<B, B>(), "");
static_assert(!convertible<A, B>(), "");
static_assert(convertible<B, A>(), "");
static_assert(convertible<int, double>(), "");
static_assert(convertible<double, int>(), "");
}

namespace common_test {
using stl2::concepts::CommonType;
using stl2::concepts::models::same;
using stl2::concepts::models::common;

struct A {};
}

namespace std {
template <>
struct common_type<::common_test::A, ::common_test::A> {
  using type = void;
};
}

namespace common_test {
static_assert(same<CommonType<int, int>, int>(), "");
static_assert(same<CommonType<A, A>, A>(), "");

static_assert(common<int, int>(), "");
static_assert(common<int, double>(), "");
static_assert(common<double, int>(), "");
static_assert(common<double, double>(), "");
static_assert(!common<void, int>(), "");
static_assert(!common<int*, int>(), "");
static_assert(common<void*, int*>(), "");
static_assert(common<double,long long>(), "");

struct B {};
struct C { C() = default; C(B) {} };
static_assert(common<B,C>(), "");
}

namespace constructible_test {
using stl2::concepts::Convertible;
using stl2::concepts::Constructible;

template <class T, class U>
  requires Constructible<T, U>
constexpr bool f() { return false; }
template <class T, Convertible<T> >
constexpr bool f() { return true; }

static_assert(f<int,int>(), "");
static_assert(f<int,long>(), "");

struct A {
  A(int);
};
struct B {
  explicit B(int);
};
static_assert(f<A, int>(), "");
static_assert(!f<B, int>(), "");
}

namespace {
struct tag {};
struct A { A() = default; A(int) {} explicit A(tag) {} };
struct B : A {};
struct C : B {};

enum class result {
  exact, publicly_derived, convertible,
  explicitly_convertible, unrelated
};

#if 1
// Concepts version
result f(A) {
  std::cout << "exactly A\n";
  return result::exact;
}

result f(stl2::PubliclyDerived<A>) {
  std::cout << "Publicly derived from A\n";
  return result::publicly_derived;
}

result f(stl2::Convertible<A>) {
  std::cout << "Implicitly convertible to A\n";
  return result::convertible;
}

result f(stl2::ExplicitlyConvertible<A>) {
  std::cout << "Explicitly convertible to A\n";
  return result::explicitly_convertible;
}

result f(auto) {
  std::cout << "Nothing to do with A\n";
  return result::unrelated;
}

#else
// C++14 version
result f(A) {
  std::cout << "exactly A\n";
  return result::exact;
}

template <class T>
std::enable_if_t<std::is_base_of<A, T>::value &&
                 std::is_convertible<T,A>::value &&
                 !(std::is_same<A,T>::value), result>
f(T) {
  std::cout << "Publicly derived from A\n";
  return result::publicly_derived;
}

template <class T>
std::enable_if_t<
  std::is_convertible<T,A>::value &&
  !(std::is_base_of<A,T>::value ||
    std::is_same<A,T>::value), result>
f(T) {
  std::cout << "Implicitly convertible to A\n";
  return result::convertible;
}

template <class T>
std::enable_if_t<
  std::is_constructible<A,T>::value &&
  !(std::is_convertible<T,A>::value ||
    std::is_base_of<A,T>::value ||
    std::is_same<A,T>::value), result>
f(T) {
  std::cout << "Explicitly convertible to A\n";
  return result::explicitly_convertible;
}

template <class T>
std::enable_if_t<
  !(std::is_constructible<A,T>::value ||
    std::is_convertible<T,A>::value ||
    std::is_base_of<A,T>::value ||
    std::is_same<A,T>::value), result>
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
