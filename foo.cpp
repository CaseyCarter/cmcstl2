// -*- compile-command: "(cd ~/cmcstl2/build && make foo && ./foo)" -*-

#include <stl2/concepts/all.hpp>
#include <stl2/utility>

#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <iostream>

////////////
// Test code
//
namespace boolean_test {
using stl2::concepts::models::boolean;

static_assert(boolean<bool>(), "");
static_assert(boolean<int>(), "");
static_assert(boolean<void*>(), "");

struct A {};
static_assert(!boolean<A>(), "");

struct B { operator bool() const { return true; } };
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

struct copyable {};
struct moveonly {
  moveonly() = default;
  moveonly(moveonly&&) = default;
  moveonly& operator=(moveonly&&) = default;
};
struct nonmovable {
  nonmovable() = default;
  nonmovable(nonmovable&&) = delete;
};
struct copyonly {
  copyonly() = default;
  copyonly(const copyonly&) = default;
  copyonly& operator=(const copyonly&) = default;
  copyonly(copyonly&&) = delete;
  copyonly& operator=(copyonly&&) = delete;
};

static_assert(movable<int>(), "");
static_assert(movable<double>(), "");
static_assert(!movable<void>(), "");
static_assert(movable<copyable>(), "");
static_assert(movable<moveonly>(), "");
static_assert(!movable<nonmovable>(), "");
static_assert(!movable<copyonly>(), "");

static_assert(copyable<int>(), "");
static_assert(copyable<double>(), "");
static_assert(!copyable<void>(), "");
static_assert(copyable<copyable>(), "");
static_assert(!copyable<moveonly>(), "");
static_assert(!copyable<nonmovable>(), "");
static_assert(!copyable<copyonly>(), "");
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

#if 0
namespace regular {
using stl2::concepts::models::equality_comparable;
using stl2::concepts::models::semiregular;
using stl2::concepts::models::regular;

struct A {
  friend constexpr bool operator==(const A&, const A&) {
    return true;
  }
  friend constexpr bool operator!=(const A&, const A&) {
    return false;
  }
};

static_assert(semiregular<int>(), "");
static_assert(equality_comparable<int>(), "");
static_assert(regular<int>(), "");
static_assert(regular<A>(), "");
static_assert(!regular<void>(), "");
}
#endif

namespace associated_type_test {
using stl2::concepts::models::same;
using stl2::ReferenceType;
using stl2::ValueType;
using stl2::DifferenceType;
using stl2::DistanceType;

struct A { int& operator*(); };
struct B : A { using value_type = double; };
struct C : A { using element_type = double; };
struct D : A {
  using value_type = double;
  using element_type = char;
};

static_assert(same<int&, ReferenceType<int*>>(), "");
static_assert(same<int&, ReferenceType<int[]>>(), "");
static_assert(same<int&, ReferenceType<int[4]>>(), "");
static_assert(same<int&, ReferenceType<A>>(), "");
static_assert(same<int&, ReferenceType<B>>(), "");
static_assert(same<int&, ReferenceType<C>>(), "");
static_assert(same<int&, ReferenceType<D>>(), "");
static_assert(same<const int&, ReferenceType<const int*>>(), "");

static_assert(same<int, ValueType<int*>>(), "");
static_assert(same<int, ValueType<int[]>>(), "");
static_assert(same<int, ValueType<int[4]>>(), "");
static_assert(same<int, ValueType<A>>(), "");
static_assert(same<double, ValueType<B>>(), "");
static_assert(same<double, ValueType<C>>(), "");
static_assert(same<double, ValueType<D>>(), "");
static_assert(same<int, ValueType<const int*>>(), "");

static_assert(same<std::ptrdiff_t, DifferenceType<int*>>(), "");
static_assert(same<std::ptrdiff_t, DifferenceType<int[]>>(), "");
static_assert(same<std::ptrdiff_t, DifferenceType<int[4]>>(), "");
static_assert(same<std::ptrdiff_t, DifferenceType<std::nullptr_t>>(), "");
static_assert(same<std::make_unsigned_t<std::ptrdiff_t>, DistanceType<int*>>(), "");

static_assert(same<int, DifferenceType<int>>(), "");
static_assert(same<unsigned, DistanceType<int>>(), "");
} // namespace associated_type_test

namespace readable_test {
using stl2::concepts::models::readable;

static_assert(!readable<void>(), "");
static_assert(!readable<void*>(), "");
static_assert(readable<int*>(), "");
static_assert(readable<const int*>(), "");
}

namespace weakly_incrementable_test {
using stl2::concepts::models::weakly_incrementable;

static_assert(weakly_incrementable<int>(), "");
static_assert(weakly_incrementable<unsigned int>(), "");
static_assert(!weakly_incrementable<void>(), "");
static_assert(weakly_incrementable<int*>(), "");
static_assert(weakly_incrementable<const int*>(), "");
}

#if 0 // FIXME: These cause the compiler to ICE
namespace incrementable_test {
using stl2::concepts::models::incrementable;

static_assert(incrementable<int>(), "");
static_assert(incrementable<unsigned int>(), "");
static_assert(!incrementable<void>(), "");
static_assert(incrementable<int*>(), "");
static_assert(incrementable<const int*>(), "");
}
#endif

int main() {}
