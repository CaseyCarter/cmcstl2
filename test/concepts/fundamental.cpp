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

#if VALIDATE_STL2
#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/detail/concepts/object.hpp>
#endif

#include <cstddef>
#include "../simple_test.hpp"

CONCEPT_ASSERT(ranges::integral<int>);
CONCEPT_ASSERT(!ranges::integral<double>);
CONCEPT_ASSERT(ranges::integral<unsigned>);
CONCEPT_ASSERT(!ranges::integral<void>);
CONCEPT_ASSERT(ranges::integral<std::ptrdiff_t>);
CONCEPT_ASSERT(ranges::integral<std::size_t>);

CONCEPT_ASSERT(ranges::signed_integral<int>);
CONCEPT_ASSERT(!ranges::signed_integral<double>);
CONCEPT_ASSERT(!ranges::signed_integral<unsigned>);
CONCEPT_ASSERT(!ranges::signed_integral<void>);
CONCEPT_ASSERT(ranges::signed_integral<std::ptrdiff_t>);
CONCEPT_ASSERT(!ranges::signed_integral<std::size_t>);

CONCEPT_ASSERT(!ranges::unsigned_integral<int>);
CONCEPT_ASSERT(!ranges::unsigned_integral<double>);
CONCEPT_ASSERT(ranges::unsigned_integral<unsigned>);
CONCEPT_ASSERT(!ranges::unsigned_integral<void>);
CONCEPT_ASSERT(!ranges::unsigned_integral<std::ptrdiff_t>);
CONCEPT_ASSERT(ranges::unsigned_integral<std::size_t>);

#if VALIDATE_STL2
namespace scalar_types {
enum class t {
	regular, scalar, arithmetic, floating_point,
	integral, signed_integral, unsigned_integral, ull
};

template<__stl2::regular T>
constexpr t f(T) { return t::regular; }
template<__stl2::ext::Scalar T>
constexpr t f(T) { return t::scalar; }
template<__stl2::ext::Arithmetic T>
constexpr t f(T) { return t::arithmetic; }
template<__stl2::floating_point T>
constexpr t f(T) { return t::floating_point; }
template<__stl2::integral T>
constexpr t f(T) { return t::integral; }
template<__stl2::signed_integral T>
constexpr t f(T) { return t::signed_integral; }
template<__stl2::unsigned_integral T>
constexpr t f(T) { return t::unsigned_integral; }
constexpr t f(unsigned long long) { return t::ull; }

void test() {
	CHECK(f(0.0f) == t::floating_point);
	CHECK(f(0.0) == t::floating_point);
	CHECK(f(0) == t::signed_integral);
	CHECK(f(0u) == t::unsigned_integral);
	CHECK(f(nullptr) == t::scalar);
	CHECK(f(0ull) == t::ull);
	CHECK((f('a') == t::signed_integral || f('a') == t::unsigned_integral));
	{
		int i;
		CHECK(f(&i) == t::scalar);
	}
	{
		struct A { void foo() {} };
		CHECK(f(&A::foo) == t::scalar);
	}
}
}
#endif

int main() {
#if VALIDATE_STL2
	scalar_types::test();
#endif
	return ::test_result();
}
