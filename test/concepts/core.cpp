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
#include <iostream>
#include <type_traits>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/utility.hpp>
#include "../simple_test.hpp"

namespace ranges = std::experimental::ranges;

static_assert(ranges::same_as<int, int>);
static_assert(ranges::same_as<double, double>);
static_assert(!ranges::same_as<double, int>);
static_assert(!ranges::same_as<int, double>);

// Test that `same_as<A, B> && X` subsumes `same_as<B, A>` (with reversed args).
template<class A, class B>
	requires ranges::same_as<B, A>
constexpr bool test_same() {
	return false;
}

template<class A, class B>
	requires ranges::same_as<A, B> && ranges::integral<A>
constexpr bool test_same() {
	return true;
}

static_assert(!test_same<int*, int*>());
static_assert(test_same<int, int>());

namespace convertible_to_test {
	struct A {};
	struct B : A {};

	static_assert(ranges::convertible_to<A, A>);
	static_assert(ranges::convertible_to<B, B>);
	static_assert(!ranges::convertible_to<A, B>);
	static_assert(ranges::convertible_to<B, A>);
	static_assert(ranges::convertible_to<int, double>);
	static_assert(ranges::convertible_to<double, int>);
	static_assert(ranges::convertible_to<void, void>);
}

namespace common_test {
	static_assert(ranges::same_as<ranges::common_type_t<int, int>, int>);
	static_assert(ranges::same_as<ranges::common_type_t<int, float, double>, double>);

	static_assert(ranges::common_with<int, int>);
	static_assert(ranges::common_with<int, double>);
	static_assert(ranges::common_with<double, int>);
	static_assert(ranges::common_with<double, double>);
	static_assert(!ranges::common_with<void, int>);
	static_assert(!ranges::common_with<int*, int>);
	static_assert(ranges::common_with<void*, int*>);
	static_assert(ranges::common_with<double, long long>);
	static_assert(ranges::common_with<void, void>);

	struct B {};
	struct C { C() = default; C(B) {} C(int) {} };
	static_assert(ranges::common_with<B, C>);

	struct incomplete;
	static_assert(ranges::common_with<void*, incomplete*>);
}

namespace {
	struct A {
		A() = default;
		A(int) {}
	};

	enum class result {
		exact, convertible, unrelated
	};

	// Concepts version
	result f(A) {
		std::cout << "exactly A\n";
		return result::exact;
	}

	template<ranges::convertible_to<A> T>
	result f(T) {
		std::cout << "Convertible to A\n";
		return result::convertible;
	}

	template<class T>
	result f(T) {
		std::cout << "Nothing to do with A\n";
		return result::unrelated;
	}
} // unnamed namespace

int main() {
	CHECK(f(A{}) == result::exact);
	{
		const A a{};
		CHECK(f(a) == result::exact);
	}
	CHECK(f(42) == result::convertible);
	CHECK(f("foo") == result::unrelated);

	return ::test_result();
}
