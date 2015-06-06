// -*- compile-command: "(cd ~/cmcstl2/build && make foundational && ./test/foundational)" -*-

#include <stl2/concepts/foundational.hpp>
#include <stl2/utility>

#include "copymove.hpp"
#include "simple_test.hpp"

namespace boolean_test {
using stl2::concepts::models::boolean;

static_assert(boolean<bool>(), "");
static_assert(boolean<int>(), "");
static_assert(boolean<void*>(), "");

struct A {};
struct B { operator bool() const { return true; } };

static_assert(!boolean<A>(), "");
static_assert(boolean<B>(), "");
}

namespace integral_test {
using stl2::concepts::models::integral;

static_assert(integral<int>(), "");
static_assert(!integral<double>(), "");
static_assert(integral<unsigned>(), "");
static_assert(!integral<void>(), "");
}

namespace copy_move_test {
using stl2::concepts::models::movable;
using stl2::concepts::models::copyable;

static_assert(movable<int>(), "");
static_assert(movable<double>(), "");
static_assert(!movable<void>(), "");
static_assert(movable<copyable_t>(), "");
static_assert(movable<moveonly_t>(), "");
static_assert(!movable<nonmovable_t>(), "");
static_assert(!movable<copyonly_t>(), "");

static_assert(copyable<int>(), "");
static_assert(copyable<double>(), "");
static_assert(!copyable<void>(), "");
static_assert(copyable<copyable_t>(), "");
static_assert(!copyable<moveonly_t>(), "");
static_assert(!copyable<nonmovable_t>(), "");
static_assert(!copyable<copyonly_t>(), "");
} // namespace copy_move_test

namespace semiregular {
using stl2::concepts::models::semiregular;

struct A {};

static_assert(semiregular<int>(), "");
static_assert(semiregular<double>(), "");
static_assert(!semiregular<void>(), "");
static_assert(!semiregular<int&>(), "");
static_assert(semiregular<A>(), "");
}

namespace equality_comparable_test {
using stl2::concepts::models::equality_comparable;

struct A {
  friend constexpr bool operator==(const A&, const A&) {
    return true;
  }
  friend constexpr bool operator!=(const A&, const A&) {
    return false;
  }
};

#if 0 // FIXME: ICE
static_assert(equality_comparable<int>(), "");
static_assert(equality_comparable<A>(), "");
#endif
} // namespace equality_comparable_test

#if 0 // FIXME: ICE
namespace regular_test {
using stl2::concepts::models::semiregular;
using stl2::concepts::models::regular;

static_assert(semiregular<int>(), "");
static_assert(regular<int>(), "");
static_assert(regular<A>(), "");
static_assert(!regular<void>(), "");
} // namespac regular_test
#endif

int main() {}
