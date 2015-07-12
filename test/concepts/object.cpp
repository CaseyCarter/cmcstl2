// -*- compile-command: "(cd ~/cmcstl2/build && make object && ./test/concepts/object)" -*-

#include <stl2/concepts/object.hpp>
#include <stl2/utility.hpp>

#include "copymove.hpp"
#include "../simple_test.hpp"

using namespace stl2::ext::models;

static_assert(!destructible<void>(), "");
static_assert(destructible<int>(), "");
static_assert(!destructible<int&>(), "");
static_assert(!destructible<int[4]>(), "");
static_assert(!destructible<int()>(), "");

static_assert(default_constructible<int>(), "");
static_assert(default_constructible<double>(), "");
static_assert(!default_constructible<void>(), "");
static_assert(!default_constructible<int[2]>(), "");
static_assert(!default_constructible<int&>(), "");
static_assert(!default_constructible<int(int)>(), "");

// It's hard to catch explicit default constructors, see
// http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1518.
// static_assert(!default_constructible<explicit_default_t>(), "");

static_assert(!move_constructible<void>(), "");
static_assert(move_constructible<int>(), "");
static_assert(!move_constructible<int[4]>(), "");
static_assert(!move_constructible<int&>(), "");
static_assert(!move_constructible<void()>(), "");

static_assert(move_constructible<copyable_t>(), "");
static_assert(move_constructible<moveonly_t>(), "");
static_assert(!move_constructible<nonmovable_t>(), "");
static_assert(!move_constructible<copyonly_t>(), "");
static_assert(!move_constructible<explicit_move_t>(), "");
static_assert(move_constructible<explicit_copy_t>(), "");

static_assert(!copy_constructible<void>(), "");
static_assert(copy_constructible<int>(), "");
static_assert(!copy_constructible<int[4]>(), "");
static_assert(!copy_constructible<int&>(), "");
static_assert(!copy_constructible<void()>(), "");

static_assert(copy_constructible<copyable_t>(), "");
static_assert(!copy_constructible<moveonly_t>(), "");
static_assert(!copy_constructible<nonmovable_t>(), "");
static_assert(!copy_constructible<copyonly_t>(), "");
static_assert(!copy_constructible<explicit_move_t>(), "");
static_assert(!copy_constructible<explicit_copy_t>(), "");

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

namespace semiregular_test {
struct A {};

static_assert(semiregular<int>(), "");
static_assert(semiregular<double>(), "");
static_assert(!semiregular<void>(), "");
static_assert(!semiregular<int&>(), "");
static_assert(semiregular<A>(), "");
}

namespace regular_test {
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
}

namespace integral_test {
static_assert(integral<int>(), "");
static_assert(!integral<double>(), "");
static_assert(integral<unsigned>(), "");
static_assert(!integral<void>(), "");
}


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
