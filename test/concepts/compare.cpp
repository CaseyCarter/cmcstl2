#include "validate.hpp"

#if VALIDATE_RANGES
namespace models {
template <class T>
using boolean = std::is_same<T, bool>; // Obviously many tests will fail ;)

template <class T, class U = T>
using equality_comparable = ranges::EqualityComparable<T, U>;

template <class T, class U = T>
using totally_ordered = ranges::TotallyOrdered<T, U>;
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
CONCEPT_ASSERT(models::boolean<bool>());
CONCEPT_ASSERT(models::boolean<std::true_type>());
CONCEPT_ASSERT(models::boolean<std::bitset<42>::reference>());

CONCEPT_ASSERT(models::boolean<int>());
CONCEPT_ASSERT(models::boolean<void*>());

struct A {};
struct B { operator bool() const; };

CONCEPT_ASSERT(!models::boolean<A>());
CONCEPT_ASSERT(models::boolean<B>());
}

namespace equality_comparable_test {
struct A {
  friend bool operator==(const A&, const A&);
  friend bool operator!=(const A&, const A&);
};

CONCEPT_ASSERT(models::equality_comparable<int>());
CONCEPT_ASSERT(models::equality_comparable<A>());
CONCEPT_ASSERT(!models::equality_comparable<void>());

CONCEPT_ASSERT(models::equality_comparable<int, int>());
CONCEPT_ASSERT(models::equality_comparable<A, A>());
CONCEPT_ASSERT(!models::equality_comparable<void, void>());
} // namespace equality_comparable_test

CONCEPT_ASSERT(models::totally_ordered<int>());
CONCEPT_ASSERT(models::totally_ordered<float>());
CONCEPT_ASSERT(models::totally_ordered<std::nullptr_t>());
CONCEPT_ASSERT(!models::totally_ordered<void>());

CONCEPT_ASSERT(models::totally_ordered<int, int>());
CONCEPT_ASSERT(models::totally_ordered<int, double>());
CONCEPT_ASSERT(!models::totally_ordered<int, void>());

int main() {
  return ::test_result();
}
