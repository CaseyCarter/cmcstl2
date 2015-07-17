// -*- compile-command: "(cd ~/cmcstl2/build && make concepts.core && ./test/concepts/concepts.core)" -*-

#include <iostream>
#include <type_traits>

#include <stl2/concepts/core.hpp>
#include <stl2/utility.hpp>

#include "../simple_test.hpp"

namespace same_test {
using stl2::ext::models::same;

static_assert(stl2::detail::all_same<>(), "");
static_assert(stl2::detail::all_same<int, int>(), "");
static_assert(stl2::detail::all_same<double, double>(), "");
static_assert(stl2::detail::all_same<double>(), "");
static_assert(!stl2::detail::all_same<double, int>(), "");
static_assert(!stl2::detail::all_same<int, double>(), "");

static_assert(same<>(), "");
static_assert(same<int, int>(), "");
static_assert(same<double, double>(), "");
static_assert(same<double>(), "");
static_assert(!same<double, int>(), "");
static_assert(!same<int, double>(), "");

static_assert(same<int, int, int, int>(), "");
static_assert(!same<int, int, double, int>(), "");
}

namespace publicly_derived_from_test {
using stl2::ext::models::publicly_derived_from;

struct A {};
struct B : A {};
struct C : A {};
struct D : B, C {};

static_assert(publicly_derived_from<int,int>(), "");
static_assert(publicly_derived_from<A,A>(), "");
static_assert(publicly_derived_from<B,B>(), "");
static_assert(publicly_derived_from<C,C>(), "");
static_assert(publicly_derived_from<D,D>(), "");
static_assert(publicly_derived_from<B,A>(), "");
static_assert(publicly_derived_from<C,A>(), "");
static_assert(!publicly_derived_from<A,B>(), "");
static_assert(!publicly_derived_from<A,C>(), "");
static_assert(!publicly_derived_from<A,D>(), "");
static_assert(!publicly_derived_from<D,A>(), "");
static_assert(!publicly_derived_from<int,void>(), "");
}

namespace convertible_to_test {
using stl2::ext::models::convertible_to;

struct A {};
struct B : A {};

static_assert(convertible_to<A, A>(), "");
static_assert(convertible_to<B, B>(), "");
static_assert(!convertible_to<A, B>(), "");
static_assert(convertible_to<B, A>(), "");
static_assert(convertible_to<int, double>(), "");
static_assert(convertible_to<double, int>(), "");
}

namespace common_test {
using stl2::CommonType;
using stl2::ext::models::same;
using stl2::ext::models::common;

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
static_assert(same<CommonType<int, float, double>, double>(), "");

static_assert(common<int, int>(), "");
static_assert(common<int, double>(), "");
static_assert(common<double, int>(), "");
static_assert(common<double, double>(), "");
static_assert(!common<void, int>(), "");
static_assert(!common<int*, int>(), "");
static_assert(common<void*, int*>(), "");
static_assert(common<double,long long>(), "");
static_assert(common<int, float, double>(), "");
static_assert(!common<int, float, double, void>(), "");

struct B {};
struct C { C() = default; C(B) {} C(int) {} };
static_assert(common<B,C>(), "");
static_assert(common<int, C, B>(), "");
}

namespace core_constructible_test {

template <class T, class U>
  requires stl2::ext::core::Constructible<T, U>()
constexpr bool f() { return false; }

template <class T, stl2::ConvertibleTo<T> >
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
