// -*- compile-command: "(cd ~/cmcstl2/build && make foundational && ./test/foundational)" -*-

#include <stl2/concepts/foundational.hpp>
#include <stl2/utility.hpp>

#include "copymove.hpp"
#include "simple_test.hpp"

namespace destructible_test {
using stl2::concepts::models::destructible;

static_assert(!destructible<void>(), "");
static_assert(destructible<int>(), "");
static_assert(!destructible<int&>(), "");
static_assert(!destructible<int[4]>(), "");
static_assert(!destructible<int()>(), "");
}

namespace copy_move_test {
using stl2::concepts::models::copy_constructible;

static_assert(!copy_constructible<void>(), "");
static_assert(copy_constructible<int>(), "");
static_assert(!copy_constructible<int[4]>(), "");
static_assert(!copy_constructible<int&>(), "");
static_assert(!copy_constructible<void()>(), "");

static_assert(copy_constructible<copyable_t>(), "");
static_assert(!copy_constructible<moveonly_t>(), "");
static_assert(!copy_constructible<nonmovable_t>(), "");
static_assert(!copy_constructible<copyonly_t>(), "");
} // namespace copy_move_test

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

static_assert(equality_comparable<int>(), "");
static_assert(equality_comparable<A>(), "");
static_assert(!equality_comparable<void>(), "");

static_assert(equality_comparable<int, int>(), "");
static_assert(equality_comparable<A, A>(), "");
static_assert(!equality_comparable<void, void>(), "");
} // namespace equality_comparable_test

namespace regular_test {
using stl2::concepts::models::regular;

struct A {
  friend constexpr bool operator==(const A&, const A&) {
    return true;
  }
  friend constexpr bool operator!=(const A&, const A&) {
    return false;
  }
};

static_assert(regular<int>(), "");
static_assert(regular<A>(), "");
static_assert(!regular<void>(), "");
} // namespace regular_test

namespace totally_ordered_test {
using stl2::concepts::models::totally_ordered;

static_assert(totally_ordered<int>(), "");
static_assert(totally_ordered<float>(), "");
static_assert(totally_ordered<std::nullptr_t>(), "");
static_assert(!totally_ordered<void>(), "");

static_assert(totally_ordered<int, int>(), "");
static_assert(totally_ordered<int, double>(), "");
static_assert(!totally_ordered<int, void>(), "");
} // namespace totally_ordered_test


namespace detail {
struct destroy_fn {
  template <stl2::Destructible T>
  void operator()(T& o) const noexcept {
    o.~T();
  }

  template <class T, std::size_t N>
    requires stl2::Destructible<std::remove_all_extents<T>>()
  void operator()(T (&a)[N]) const noexcept {
    for (auto& i : a) {
      (*this)(i);
    }
  }
};
} // namespace detail
namespace {
  constexpr const auto& destroy = detail::destroy_fn{};
} // unnamed namespace

int main() {}
