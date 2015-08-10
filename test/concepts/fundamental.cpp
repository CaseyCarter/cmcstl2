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
#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/detail/concepts/object.hpp>
#endif

#include <cstddef>
#include "../simple_test.hpp"

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

#if VALIDATE_STL2
namespace scalar_types {
enum class t {
  regular, scalar, arithmetic, floating_point,
  integral, signed_integral, unsigned_integral, ull
};

constexpr t f(stl2::Regular) { return t::regular; }
constexpr t f(stl2::ext::Scalar) { return t::scalar; }
constexpr t f(stl2::ext::Arithmetic) { return t::arithmetic; }
constexpr t f(stl2::ext::FloatingPoint) { return t::floating_point; }
constexpr t f(stl2::Integral) { return t::integral; }
constexpr t f(stl2::SignedIntegral) { return t::signed_integral; }
constexpr t f(stl2::UnsignedIntegral) { return t::unsigned_integral; }
constexpr t f(unsigned long long) { return t::ull; }

void test() {
  CHECK(f(0.0f) == t::floating_point);
  CHECK(f(0.0d) == t::floating_point);
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
