#if VALIDATE_RANGES == VALIDATE_STL2
#error You must define exactly one of VALIDATE_RANGES or VALIDATE_STL2.
#endif

#if VALIDATE_RANGES
#include <range/v3/utility/concepts.hpp>

namespace models {
template <class T>
using integral = ranges::Integral<T>;

template <class T>
using signed_integral = ranges::SignedIntegral<T>;

template <class T>
using unsigned_integral = ranges::UnsignedIntegral<T>;
} // namespace models

#elif VALIDATE_STL2

#include <stl2/concepts/object.hpp>

namespace models = stl2::ext::models;
#endif

static_assert(integral<int>(), "");
static_assert(!integral<double>(), "");
static_assert(integral<unsigned>(), "");
static_assert(!integral<void>(), "");

int main() {}
