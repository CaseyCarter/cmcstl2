// -*- compile-command: "(cd ~/cmcstl2/build && make test/concepts/compare && ./test/concepts/compare)" -*-

#ifdef VALIDATE_RANGES
#error No range-v3 version of this test yet.
#endif

#include <stl2/concepts/compare.hpp>

#include <type_traits>
#include <bitset>

#include "../simple_test.hpp"

namespace boolean_test {
using stl2::ext::models::boolean;

// Better have at least these three, since we use them as
// examples in the TS draft.
static_assert(boolean<bool>(), "");
static_assert(boolean<std::true_type>(), "");
static_assert(boolean<std::bitset<42>::reference>(), "");

static_assert(boolean<int>(), "");
static_assert(boolean<void*>(), "");

struct A {};
struct B { operator bool() const; };

static_assert(!boolean<A>(), "");
static_assert(boolean<B>(), "");
}

namespace equality_comparable_test {
using stl2::ext::models::equality_comparable;

struct A {
  friend bool operator==(const A&, const A&);
  friend bool operator!=(const A&, const A&);
};

static_assert(equality_comparable<int>(), "");
static_assert(equality_comparable<A>(), "");
static_assert(!equality_comparable<void>(), "");

static_assert(equality_comparable<int, int>(), "");
static_assert(equality_comparable<A, A>(), "");
static_assert(!equality_comparable<void, void>(), "");
} // namespace equality_comparable_test

namespace totally_ordered_test {
using stl2::ext::models::totally_ordered;

static_assert(totally_ordered<int>(), "");
static_assert(totally_ordered<float>(), "");
static_assert(totally_ordered<std::nullptr_t>(), "");
static_assert(!totally_ordered<void>(), "");

static_assert(totally_ordered<int, int>(), "");
static_assert(totally_ordered<int, double>(), "");
static_assert(!totally_ordered<int, void>(), "");
} // namespace totally_ordered_test

int main() {
  return ::test_result();
}
