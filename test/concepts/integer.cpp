#include "validate.hpp"

#if VALIDATE_RANGES
namespace models {
template <class T>
using integral = ranges::Integral<T>;

template <class T>
using signed_integral = ranges::SignedIntegral<T>;

template <class T>
using unsigned_integral = ranges::UnsignedIntegral<T>;
} // namespace models

#elif VALIDATE_STL2
#include <stl2/detail/concepts/object.hpp>
#endif

#include <cstddef>

CONCEPT_ASSERT(models::integral<int>());
CONCEPT_ASSERT(!models::integral<double>());
CONCEPT_ASSERT(models::integral<unsigned>());
CONCEPT_ASSERT(!models::integral<void>());
CONCEPT_ASSERT(models::integral<std::ptrdiff_t>());
CONCEPT_ASSERT(models::integral<std::size_t>());

CONCEPT_ASSERT(models::signed_integral<int>());
CONCEPT_ASSERT(!models::signed_integral<double>());
CONCEPT_ASSERT(!models::signed_integral<unsigned>());
CONCEPT_ASSERT(!models::signed_integral<void>());
CONCEPT_ASSERT(models::signed_integral<std::ptrdiff_t>());
CONCEPT_ASSERT(!models::signed_integral<std::size_t>());

CONCEPT_ASSERT(!models::unsigned_integral<int>());
CONCEPT_ASSERT(!models::unsigned_integral<double>());
CONCEPT_ASSERT(models::unsigned_integral<unsigned>());
CONCEPT_ASSERT(!models::unsigned_integral<void>());
CONCEPT_ASSERT(!models::unsigned_integral<std::ptrdiff_t>());
CONCEPT_ASSERT(models::unsigned_integral<std::size_t>());

int main() {}
