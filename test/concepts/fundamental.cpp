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
#include <cstddef>
#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/detail/concepts/object.hpp>
#include "../simple_test.hpp"

namespace ranges = std::experimental::ranges;

static_assert(ranges::integral<int>);
static_assert(!ranges::integral<double>);
static_assert(ranges::integral<unsigned>);
static_assert(!ranges::integral<void>);
static_assert(ranges::integral<std::ptrdiff_t>);
static_assert(ranges::integral<std::size_t>);

static_assert(ranges::signed_integral<int>);
static_assert(!ranges::signed_integral<double>);
static_assert(!ranges::signed_integral<unsigned>);
static_assert(!ranges::signed_integral<void>);
static_assert(ranges::signed_integral<std::ptrdiff_t>);
static_assert(!ranges::signed_integral<std::size_t>);

static_assert(!ranges::unsigned_integral<int>);
static_assert(!ranges::unsigned_integral<double>);
static_assert(ranges::unsigned_integral<unsigned>);
static_assert(!ranges::unsigned_integral<void>);
static_assert(!ranges::unsigned_integral<std::ptrdiff_t>);
static_assert(ranges::unsigned_integral<std::size_t>);

namespace scalar_types {
	enum class t {
		regular, scalar, arithmetic, floating_point,
		integral, signed_integral, unsigned_integral, ull
	};

	template<ranges::regular T>
	constexpr t f(T) { return t::regular; }
	template<ranges::ext::Scalar T>
	constexpr t f(T) { return t::scalar; }
	template<ranges::ext::Arithmetic T>
	constexpr t f(T) { return t::arithmetic; }
	template<ranges::floating_point T>
	constexpr t f(T) { return t::floating_point; }
	template<ranges::integral T>
	constexpr t f(T) { return t::integral; }
	template<ranges::signed_integral T>
	constexpr t f(T) { return t::signed_integral; }
	template<ranges::unsigned_integral T>
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
} // namespace scalar_types

int main() {
	scalar_types::test();
	return ::test_result();
}
