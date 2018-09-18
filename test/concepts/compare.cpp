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
constexpr bool Boolean =
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
CONCEPT_ASSERT(ranges::Boolean<bool>);
CONCEPT_ASSERT(ranges::Boolean<std::true_type>);
CONCEPT_ASSERT(ranges::Boolean<std::bitset<42>::reference>);

CONCEPT_ASSERT(ranges::Boolean<int>);
CONCEPT_ASSERT(!ranges::Boolean<void*>);

struct A {};
struct B { operator bool() const; };

CONCEPT_ASSERT(!ranges::Boolean<A>);
CONCEPT_ASSERT(ranges::Boolean<B>);
}

namespace equality_comparable_test {
struct A {
	friend bool operator==(const A&, const A&);
	friend bool operator!=(const A&, const A&);
};

CONCEPT_ASSERT(ranges::EqualityComparable<int>);
CONCEPT_ASSERT(ranges::EqualityComparable<A>);
CONCEPT_ASSERT(!ranges::EqualityComparable<void>);
CONCEPT_ASSERT(ranges::EqualityComparable<int&>);
CONCEPT_ASSERT(ranges::EqualityComparable<std::nullptr_t>);

CONCEPT_ASSERT(ranges::EqualityComparableWith<int, int>);
CONCEPT_ASSERT(ranges::EqualityComparableWith<A, A>);
CONCEPT_ASSERT(!ranges::EqualityComparableWith<void, void>);
CONCEPT_ASSERT(ranges::EqualityComparableWith<int&, int>);
} // namespace equality_comparable_test

CONCEPT_ASSERT(ranges::StrictTotallyOrdered<int>);
CONCEPT_ASSERT(ranges::StrictTotallyOrdered<float>);
CONCEPT_ASSERT(!ranges::StrictTotallyOrdered<void>);
CONCEPT_ASSERT(ranges::StrictTotallyOrdered<int&>);

CONCEPT_ASSERT(ranges::StrictTotallyOrderedWith<int, int>);
CONCEPT_ASSERT(ranges::StrictTotallyOrderedWith<int, double>);
CONCEPT_ASSERT(!ranges::StrictTotallyOrderedWith<int, void>);
CONCEPT_ASSERT(ranges::StrictTotallyOrderedWith<int&, int>);

int main() {
	return ::test_result();
}
