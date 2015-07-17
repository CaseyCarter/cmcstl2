// -*- compile-command: "(cd ~/cmcstl2/build && make concepts.object && ./test/concepts/concepts.object)" -*-

#include <stl2/concepts/object.hpp>
#include <stl2/utility.hpp>

#include "../simple_test.hpp"

using namespace stl2::ext::models;

struct copyable_t {};

struct moveonly_t {
  moveonly_t() = default;
  moveonly_t(moveonly_t&&) = default;
  moveonly_t& operator=(moveonly_t&&) = default;
};

struct nonmovable_t {
  nonmovable_t() = default;
  nonmovable_t(nonmovable_t&&) = delete;
};

struct copyonly_t {
  copyonly_t() = default;
  copyonly_t(const copyonly_t&) = default;
  copyonly_t& operator=(const copyonly_t&) = default;
  copyonly_t(copyonly_t&&) = delete;
  copyonly_t& operator=(copyonly_t&&) = delete;
};

struct explicit_default_t {
  explicit explicit_default_t() {}
};

struct explicit_move_t {
  explicit_move_t() = default;
  explicit explicit_move_t(explicit_move_t&&) = default;
};

struct explicit_copy_t {
  explicit_copy_t() = default;
  explicit_copy_t(explicit_copy_t&&) = default;
  explicit explicit_copy_t(const explicit_copy_t&) = default;
};

struct partial_overloaded_address {
  partial_overloaded_address* operator&();
};
struct overloaded_address {
  overloaded_address* operator&();
  const overloaded_address* operator&() const;
};
struct bad_overloaded_address {
  void operator&() const;
};
struct bad_overloaded_const_address {
  bad_overloaded_const_address* operator&();
  void operator&() const;
};

static_assert(!destructible<void>(), "");
static_assert(destructible<int>(), "");
static_assert(!destructible<int&>(), "");
static_assert(!destructible<int[4]>(), "");
static_assert(!destructible<int()>(), "");

static_assert(destructible<partial_overloaded_address>(), "");
static_assert(destructible<overloaded_address>(), "");
static_assert(!destructible<bad_overloaded_address>(), "");
static_assert(!destructible<bad_overloaded_const_address>(), "");

static_assert(constructible<int>(), "");
static_assert(constructible<int&, int&>(), "");
static_assert(constructible<const int&, int&>(), "");
static_assert(constructible<const int&, const int&>(), "");
static_assert(constructible<const int&, int&&>(), "");
static_assert(constructible<int, int>(), "");
static_assert(constructible<int, const int>(), "");
static_assert(constructible<int, int&>(), "");
static_assert(constructible<int, const int&>(), "");
static_assert(constructible<int, int&&>(), "");
static_assert(constructible<int, const int&&>(), "");

static_assert(default_constructible<int>(), "");
static_assert(default_constructible<double>(), "");
static_assert(!default_constructible<void>(), "");
static_assert(!default_constructible<int[2]>(), "");
static_assert(!default_constructible<int&>(), "");
static_assert(!default_constructible<int(int)>(), "");

// It's hard to catch explicit default constructors, see
// http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1518.
// static_assert(!default_constructible<explicit_default_t>(), "");
static_assert(default_constructible<explicit_move_t>(), "");
static_assert(default_constructible<explicit_copy_t>(), "");

static_assert(!move_constructible<void>(), "");
static_assert(move_constructible<int>(), "");
static_assert(!move_constructible<int[4]>(), "");
static_assert(move_constructible<int&>(), "");
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
static_assert(copy_constructible<int&>(), "");
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
static_assert(!semiregular<explicit_move_t>(), "");
static_assert(!semiregular<explicit_copy_t>(), "");
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

int main() {
  return ::test_result();
}
