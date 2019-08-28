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
#include <bitset>
#include <type_traits>
#include <stl2/detail/concepts/compare.hpp>
#include "../simple_test.hpp"

namespace ranges = std::experimental::ranges;

namespace boolean_test {
// Better have at least these three, since we use them as
// examples in the TS draft.
static_assert(ranges::boolean<bool>);
static_assert(ranges::boolean<std::true_type>);
static_assert(ranges::boolean<std::bitset<42>::reference>);

static_assert(ranges::boolean<int>);
static_assert(!ranges::boolean<void*>);

struct A {};
struct B { operator bool() const; };

static_assert(!ranges::boolean<A>);
static_assert(ranges::boolean<B>);
}

namespace equality_comparable_test {
struct A {
	friend bool operator==(const A&, const A&);
	friend bool operator!=(const A&, const A&);
};

static_assert(ranges::equality_comparable<int>);
static_assert(ranges::equality_comparable<A>);
static_assert(!ranges::equality_comparable<void>);
static_assert(ranges::equality_comparable<int&>);
static_assert(ranges::equality_comparable<std::nullptr_t>);

static_assert(ranges::equality_comparable_with<int, int>);
static_assert(ranges::equality_comparable_with<A, A>);
static_assert(!ranges::equality_comparable_with<void, void>);
static_assert(ranges::equality_comparable_with<int&, int>);
} // namespace equality_comparable_test

static_assert(ranges::totally_ordered<int>);
static_assert(ranges::totally_ordered<float>);
static_assert(!ranges::totally_ordered<void>);
static_assert(ranges::totally_ordered<int&>);

static_assert(ranges::totally_ordered_with<int, int>);
static_assert(ranges::totally_ordered_with<int, double>);
static_assert(!ranges::totally_ordered_with<int, void>);
static_assert(ranges::totally_ordered_with<int&, int>);

int main() {
	return ::test_result();
}
