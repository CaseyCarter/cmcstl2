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
namespace ranges {
template<class T>
constexpr bool boolean =
	std::is_same<T, bool>(); // Obviously many tests will fail ;)
}
#elif VALIDATE_STL2
#include <stl2/detail/concepts/compare.hpp>
#endif

#include <type_traits>
#include <bitset>
#include "../simple_test.hpp"

namespace boolean_test {
// Better have at least these three, since we use them as
// examples in the TS draft.
CONCEPT_ASSERT(ranges::boolean<bool>);
CONCEPT_ASSERT(ranges::boolean<std::true_type>);
CONCEPT_ASSERT(ranges::boolean<std::bitset<42>::reference>);

CONCEPT_ASSERT(ranges::boolean<int>);
CONCEPT_ASSERT(!ranges::boolean<void*>);

struct A {};
struct B { operator bool() const; };

CONCEPT_ASSERT(!ranges::boolean<A>);
CONCEPT_ASSERT(ranges::boolean<B>);
}

namespace equality_comparable_test {
struct A {
	friend bool operator==(const A&, const A&);
	friend bool operator!=(const A&, const A&);
};

CONCEPT_ASSERT(ranges::equality_comparable<int>);
CONCEPT_ASSERT(ranges::equality_comparable<A>);
CONCEPT_ASSERT(!ranges::equality_comparable<void>);
CONCEPT_ASSERT(ranges::equality_comparable<int&>);
CONCEPT_ASSERT(ranges::equality_comparable<std::nullptr_t>);

CONCEPT_ASSERT(ranges::equality_comparable_with<int, int>);
CONCEPT_ASSERT(ranges::equality_comparable_with<A, A>);
CONCEPT_ASSERT(!ranges::equality_comparable_with<void, void>);
CONCEPT_ASSERT(ranges::equality_comparable_with<int&, int>);
} // namespace equality_comparable_test

CONCEPT_ASSERT(ranges::totally_ordered<int>);
CONCEPT_ASSERT(ranges::totally_ordered<float>);
CONCEPT_ASSERT(!ranges::totally_ordered<void>);
CONCEPT_ASSERT(ranges::totally_ordered<int&>);

CONCEPT_ASSERT(ranges::totally_ordered_with<int, int>);
CONCEPT_ASSERT(ranges::totally_ordered_with<int, double>);
CONCEPT_ASSERT(!ranges::totally_ordered_with<int, void>);
CONCEPT_ASSERT(ranges::totally_ordered_with<int&, int>);

int main() {
	return ::test_result();
}
