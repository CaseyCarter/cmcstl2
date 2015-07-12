// -*- compile-command: "(cd ~/cmcstl2/build && make object && ./test/object)" -*-

#include <stl2/concepts/object.hpp>
#include <stl2/utility.hpp>

#include "copymove.hpp"
#include "../simple_test.hpp"

namespace destructible_test {
using stl2::ext::models::destructible;

static_assert(!destructible<void>(), "");
static_assert(destructible<int>(), "");
static_assert(!destructible<int&>(), "");
static_assert(!destructible<int[4]>(), "");
static_assert(!destructible<int()>(), "");
}

namespace copy_move_test {
using stl2::ext::models::copy_constructible;

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

namespace integral_test {
using stl2::ext::models::integral;

static_assert(integral<int>(), "");
static_assert(!integral<double>(), "");
static_assert(integral<unsigned>(), "");
static_assert(!integral<void>(), "");
}

namespace copy_move_test {
using stl2::ext::models::movable;
using stl2::ext::models::copyable;

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
using stl2::ext::models::semiregular;

struct A {};

static_assert(semiregular<int>(), "");
static_assert(semiregular<double>(), "");
static_assert(!semiregular<void>(), "");
static_assert(!semiregular<int&>(), "");
static_assert(semiregular<A>(), "");
}

namespace regular_test {
using stl2::ext::models::regular;

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
