// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include "validate.hpp"

#if VALIDATE_RANGES
namespace ns {
template<class T, class U>
using common_type_t = ranges::common_type_t<T, U>;
}

#elif VALIDATE_STL2
#include <stl2/detail/concepts/core.hpp>
#include <stl2/utility.hpp>

namespace ns {
using __stl2::common_type_t;
}
#endif

#include <iostream>
#include <type_traits>

#include "../simple_test.hpp"

CONCEPT_ASSERT(ranges::same_as<int, int>);
CONCEPT_ASSERT(ranges::same_as<double, double>);
CONCEPT_ASSERT(!ranges::same_as<double, int>);
CONCEPT_ASSERT(!ranges::same_as<int, double>);

#if VALIDATE_STL2
// Test that `same_as<A,B> && X` subsumes `same_as<B,A>` (with reversed args).
template<class A, class B>
  requires __stl2::same_as<B, A>
constexpr bool test_same() {
  return false;
}

template<class A, class B>
  requires __stl2::same_as<A, B> && __stl2::integral<A>
constexpr bool test_same() {
  return true;
}

static_assert(!test_same<int*, int*>());
static_assert(test_same<int, int>());
#endif

namespace convertible_to_test {
struct A {};
struct B : A {};

CONCEPT_ASSERT(ranges::convertible_to<A, A>);
CONCEPT_ASSERT(ranges::convertible_to<B, B>);
CONCEPT_ASSERT(!ranges::convertible_to<A, B>);
CONCEPT_ASSERT(ranges::convertible_to<B, A>);
CONCEPT_ASSERT(ranges::convertible_to<int, double>);
CONCEPT_ASSERT(ranges::convertible_to<double, int>);
CONCEPT_ASSERT(ranges::convertible_to<void, void>);
}

namespace common_test {
CONCEPT_ASSERT(ranges::same_as<ns::common_type_t<int, int>, int>);
//CONCEPT_ASSERT(ranges::same_as<ns::common_type_t<int, float, double>, double>);

CONCEPT_ASSERT(ranges::common_with<int, int>);
CONCEPT_ASSERT(ranges::common_with<int, double>);
CONCEPT_ASSERT(ranges::common_with<double, int>);
CONCEPT_ASSERT(ranges::common_with<double, double>);
CONCEPT_ASSERT(!ranges::common_with<void, int>);
CONCEPT_ASSERT(!ranges::common_with<int*, int>);
CONCEPT_ASSERT(ranges::common_with<void*, int*>);
CONCEPT_ASSERT(ranges::common_with<double,long long>);
CONCEPT_ASSERT(ranges::common_with<void, void>);
//CONCEPT_ASSERT(ranges::common_with<int, float, double>);
//CONCEPT_ASSERT(!ranges::common_with<int, float, double, void>);

struct B {};
struct C { C() = default; C(B) {} C(int) {} };
CONCEPT_ASSERT(ranges::common_with<B,C>);
//CONCEPT_ASSERT(ranges::common_with<int, C, B>);

struct incomplete;
CONCEPT_ASSERT(ranges::common_with<void*, incomplete*>);
}

namespace {
struct A { A() = default; A(int) {} };

enum class result {
	exact, convertible, unrelated
};

#if VALIDATE_STL2
// Concepts version
result f(A) {
	std::cout << "exactly A\n";
	return result::exact;
}

template<__stl2::convertible_to<A> T>
result f(T) {
	std::cout << "Convertible to A\n";
	return result::convertible;
}

template<class T>
result f(T) {
	std::cout << "Nothing to do with A\n";
	return result::unrelated;
}

#else
// C++11 version
result f(A) {
	std::cout << "exactly A\n";
	return result::exact;
}

template<class, class, class = void>
constexpr bool convertible_to = false;
template<class T, class U>
constexpr bool convertible_to<T, U, std::enable_if_t<
	std::is_convertible<T, U>::value,
	decltype(static_cast<U>(std::declval<T>()))>> = true;

template<class T>
meta::if_c<
	convertible_to<T,A> &&
		!std::is_same<A,T>::value,
	result>
f(T) {
	std::cout << "Convertible to A\n";
	return result::convertible;
}

template<class T>
meta::if_c<
	!(convertible_to<T, A> ||
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
	CHECK(f(42) == result::convertible);
	CHECK(f("foo") == result::unrelated);

	return ::test_result();
}
