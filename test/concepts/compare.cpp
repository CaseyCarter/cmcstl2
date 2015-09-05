#include "validate.hpp"

#if VALIDATE_RANGES
namespace models {
template <class T>
constexpr bool Boolean =
  std::is_same<T, bool>(); // Obviously many tests will fail ;)

template <class T, class U = T>
constexpr bool EqualityComparable =
  ranges::EqualityComparable<T, U>();

template <class T, class U = T>
constexpr bool TotallyOrdered =
  ranges::TotallyOrdered<T, U>();
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
CONCEPT_ASSERT(models::Boolean<bool>);
CONCEPT_ASSERT(models::Boolean<std::true_type>);
CONCEPT_ASSERT(models::Boolean<std::bitset<42>::reference>);

CONCEPT_ASSERT(models::Boolean<int>);
CONCEPT_ASSERT(!models::Boolean<void*>);

struct A {};
struct B { operator bool() const; };

CONCEPT_ASSERT(!models::Boolean<A>);
CONCEPT_ASSERT(models::Boolean<B>);
}

namespace equality_comparable_test {
struct A {
  friend bool operator==(const A&, const A&);
  friend bool operator!=(const A&, const A&);
};

CONCEPT_ASSERT(models::EqualityComparable<int>);
CONCEPT_ASSERT(models::EqualityComparable<A>);
CONCEPT_ASSERT(!models::EqualityComparable<void>);

CONCEPT_ASSERT(models::EqualityComparable<int, int>);
CONCEPT_ASSERT(models::EqualityComparable<A, A>);
CONCEPT_ASSERT(!models::EqualityComparable<void, void>);
} // namespace equality_comparable_test

CONCEPT_ASSERT(models::TotallyOrdered<int>);
CONCEPT_ASSERT(models::TotallyOrdered<float>);
CONCEPT_ASSERT(models::TotallyOrdered<std::nullptr_t>);
CONCEPT_ASSERT(!models::TotallyOrdered<void>);

CONCEPT_ASSERT(models::TotallyOrdered<int, int>);
CONCEPT_ASSERT(models::TotallyOrdered<int, double>);
CONCEPT_ASSERT(!models::TotallyOrdered<int, void>);

int main() {
  return ::test_result();
}
