#if VALIDATE_RANGES == VALIDATE_STL2
#error You must define exactly one of VALIDATE_RANGES or VALIDATE_STL2.
#endif

#if VALIDATE_RANGES
#include <range/v3/utility/concepts.hpp>

namespace models {
template <class T>
using destructible = ranges::Destructible<T>;

template<class T, class...Args>
using constructible_object =
  ranges::concepts::models<ranges::concepts::ConstructibleObject, T, Args...>;

template<class T, class...Args>
using bindable_reference =
  ranges::concepts::models<ranges::concepts::BindableReference, T, Args...>;

template <class T, class...Args>
using constructible = ranges::Constructible<T, Args...>;

template <class T>
using default_constructible = ranges::DefaultConstructible<T>;

template <class T>
using move_constructible = ranges::MoveConstructible<T>;

template <class T>
using copy_constructible = ranges::CopyConstructible<T>;

template <class T>
using movable = ranges::Movable<T>;

template <class T>
using copyable = ranges::Copyable<T>;

template <class T>
using semiregular = ranges::SemiRegular<T>;

template <class T>
using regular = ranges::Regular<T>;
} // namespace models

#elif VALIDATE_STL2

#include <stl2/concepts/object.hpp>

namespace models = stl2::ext::models;
#endif

struct copyable {};

struct moveonly {
  moveonly() = default;
  moveonly(moveonly&&) = default;
  moveonly& operator=(moveonly&&) = default;
};

struct copyonly {
  copyonly() = default;
  copyonly(const copyonly&) = default;
  copyonly& operator=(const copyonly&) = default;
  copyonly(copyonly&&) = delete;
  copyonly& operator=(copyonly&&) = delete;
};

struct nonmovable {
  nonmovable() = default;
  nonmovable(nonmovable&&) = delete;
};

struct nondefaultconstructible {
  nondefaultconstructible(int) {}
};

struct indestructible {
  ~indestructible() = delete;
};

struct explicit_default {
  explicit explicit_default() {}
};

struct explicit_move {
  explicit_move() = default;
  explicit explicit_move(explicit_move&&) = default;
};

struct explicit_copy {
  explicit_copy() = default;
  explicit_copy(explicit_copy&&) = default;
  explicit explicit_copy(const explicit_copy&) = default;
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

struct incomplete;

struct semiregular {};

struct regular {
  friend constexpr bool operator==(const regular&, const regular&) {
    return true;
  }
  friend constexpr bool operator!=(const regular&, const regular&) {
    return false;
  }
};

struct XXX
{
    XXX() = default;
    XXX(XXX&&) = delete;
    explicit XXX(int) {}
};

static_assert(models::destructible<int>(), "");
static_assert(models::destructible<const int>(), "");
static_assert(!models::destructible<void>(), "");
static_assert(!models::destructible<int&>(), "");
static_assert(!models::destructible<void()>(), "");
static_assert(models::destructible<void(*)()>(), "");
static_assert(!models::destructible<void(&)()>(), "");
static_assert(!models::destructible<int[]>(), "");
static_assert(!models::destructible<int[2]>(), "");
static_assert(models::destructible<int(*)[2]>(), "");
static_assert(!models::destructible<int(&)[2]>(), "");
static_assert(models::destructible<moveonly>(), "");
static_assert(models::destructible<nonmovable>(), "");
static_assert(!models::destructible<indestructible>(), "");
static_assert(!models::destructible<incomplete>(), "");

static_assert(models::destructible<partial_overloaded_address>(), "");
static_assert(models::destructible<overloaded_address>(), "");
static_assert(!models::destructible<bad_overloaded_address>(), "");
static_assert(!models::destructible<bad_overloaded_const_address>(), "");

static_assert(models::constructible<int>(), "");
static_assert(models::constructible<int const>(), "");
static_assert(!models::constructible<int const&>(), "");
static_assert(!models::constructible<int ()>(), "");
static_assert(!models::constructible<int(&)()>(), "");
static_assert(!models::constructible<int[]>(), "");
static_assert(!models::constructible<int[5]>(), "");
static_assert(!models::constructible<nondefaultconstructible>(), "");
static_assert(models::constructible<int const(&)[5], int(&)[5]>(), "");
static_assert(!models::constructible<int, int(&)[3]>(), "");

static_assert(models::constructible_object<int, int>(), "");
static_assert(models::constructible_object<int, int&>(), "");
static_assert(models::constructible_object<int, int&&>(), "");
static_assert(models::constructible_object<int, const int>(), "");
static_assert(models::constructible_object<int, const int&>(), "");
static_assert(models::constructible_object<int, const int&&>(), "");
static_assert(models::constructible<int, int>(), "");
static_assert(models::constructible<int, int&>(), "");
static_assert(models::constructible<int, int&&>(), "");
static_assert(models::constructible<int, const int>(), "");
static_assert(models::constructible<int, const int&>(), "");
static_assert(models::constructible<int, const int&&>(), "");

static_assert(models::constructible_object<copyable, copyable>(), "");
static_assert(models::constructible_object<copyable, copyable&>(), "");
static_assert(models::constructible_object<copyable, copyable&&>(), "");
static_assert(models::constructible_object<copyable, const copyable>(), "");
static_assert(models::constructible_object<copyable, const copyable&>(), "");
static_assert(models::constructible_object<copyable, const copyable&&>(), "");
static_assert(models::constructible<copyable, copyable>(), "");
static_assert(models::constructible<copyable, copyable&>(), "");
static_assert(models::constructible<copyable, copyable&&>(), "");
static_assert(models::constructible<copyable, const copyable>(), "");
static_assert(models::constructible<copyable, const copyable&>(), "");
static_assert(models::constructible<copyable, const copyable&&>(), "");

static_assert(!models::bindable_reference<int&, int>(), "");
static_assert(models::bindable_reference<int&, int&>(), "");
static_assert(!models::bindable_reference<int&, int&&>(), "");
static_assert(!models::bindable_reference<int&, const int>(), "");
static_assert(!models::bindable_reference<int&, const int&>(), "");
static_assert(!models::bindable_reference<int&, const int&&>(), "");
static_assert(!models::constructible<int&, int>(), "");
static_assert(models::constructible<int&, int&>(), "");
static_assert(!models::constructible<int&, int&&>(), "");
static_assert(!models::constructible<int&, const int>(), "");
static_assert(!models::constructible<int&, const int&>(), "");
static_assert(!models::constructible<int&, const int&&>(), "");

static_assert(models::bindable_reference<const int&, int>(), "");
static_assert(models::bindable_reference<const int&, int&>(), "");
static_assert(models::bindable_reference<const int&, int&&>(), "");
static_assert(models::bindable_reference<const int&, const int>(), "");
static_assert(models::bindable_reference<const int&, const int&>(), "");
static_assert(models::bindable_reference<const int&, const int&&>(), "");
static_assert(models::constructible<const int&, int>(), "");
static_assert(models::constructible<const int&, int&>(), "");
static_assert(models::constructible<const int&, int&&>(), "");
static_assert(models::constructible<const int&, const int>(), "");
static_assert(models::constructible<const int&, const int&>(), "");
static_assert(models::constructible<const int&, const int&&>(), "");

static_assert(models::bindable_reference<int&&, int>(), "");
static_assert(!models::bindable_reference<int&&, int&>(), "");
static_assert(models::bindable_reference<int&&, int&&>(), "");
static_assert(!models::bindable_reference<int&&, const int>(), "");
static_assert(!models::bindable_reference<int&&, const int&>(), "");
static_assert(!models::bindable_reference<int&&, const int&&>(), "");
static_assert(models::constructible<int&&, int>(), "");
static_assert(!models::constructible<int&&, int&>(), "");
static_assert(models::constructible<int&&, int&&>(), "");
static_assert(!models::constructible<int&&, const int>(), "");
static_assert(!models::constructible<int&&, const int&>(), "");
static_assert(!models::constructible<int&&, const int&&>(), "");

static_assert(models::bindable_reference<const int&&, int>(), "");
static_assert(!models::bindable_reference<const int&&, int&>(), "");
static_assert(models::bindable_reference<const int&&, int&&>(), "");
static_assert(models::bindable_reference<const int&&, const int>(), "");
static_assert(!models::bindable_reference<const int&&, const int&>(), "");
static_assert(models::bindable_reference<const int&&, const int&&>(), "");
static_assert(models::constructible<const int&&, int>(), "");
static_assert(!models::constructible<const int&&, int&>(), "");
static_assert(models::constructible<const int&&, int&&>(), "");
static_assert(models::constructible<const int&&, const int>(), "");
static_assert(!models::constructible<const int&&, const int&>(), "");
static_assert(models::constructible<const int&&, const int&&>(), "");

static_assert(models::constructible<XXX, int>(), "");

static_assert(models::default_constructible<int>(), "");
static_assert(models::default_constructible<int const>(), "");
static_assert(!models::default_constructible<int&>(), "");
static_assert(!models::default_constructible<int const&>(), "");
static_assert(!models::default_constructible<int ()>(), "");
static_assert(!models::default_constructible<int(&)()>(), "");
static_assert(models::default_constructible<double>(), "");
static_assert(!models::default_constructible<void>(), "");
static_assert(!models::default_constructible<int[]>(), "");
static_assert(!models::default_constructible<int[2]>(), "");
static_assert(!models::default_constructible<nondefaultconstructible>(), "");

// It's hard to catch explicit default constructors, see
// http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1518.
// static_assert(!models::default_constructible<explicit_default>(), "");
static_assert(models::default_constructible<explicit_move>(), "");
static_assert(models::default_constructible<explicit_copy>(), "");

static_assert(!models::move_constructible<void>(), "");
static_assert(models::move_constructible<int>(), "");
static_assert(models::move_constructible<const int>(), "");
static_assert(!models::move_constructible<int[4]>(), "");
static_assert(!models::move_constructible<void()>(), "");
static_assert(models::move_constructible<int &>(), "");
static_assert(models::move_constructible<int &&>(), "");
static_assert(models::move_constructible<const int &>(), "");
static_assert(models::move_constructible<const int &&>(), "");

static_assert(models::constructible_object<moveonly, moveonly>(), "");
static_assert(models::constructible<moveonly, moveonly>(), "");
static_assert(models::move_constructible<copyable>(), "");
static_assert(models::move_constructible<moveonly>(), "");
static_assert(!models::move_constructible<nonmovable>(), "");
static_assert(!models::move_constructible<copyonly>(), "");
static_assert(!models::move_constructible<explicit_move>(), "");
static_assert(models::move_constructible<explicit_copy>(), "");

static_assert(models::move_constructible<nonmovable &>(), "");
static_assert(models::move_constructible<nonmovable &&>(), "");
static_assert(models::move_constructible<const nonmovable &>(), "");
static_assert(models::move_constructible<const nonmovable &&>(), "");

static_assert(!models::copy_constructible<void>(), "");
static_assert(models::copy_constructible<int>(), "");
static_assert(models::copy_constructible<const int>(), "");
static_assert(models::copy_constructible<int&>(), "");
static_assert(!models::copy_constructible<int&&>(), "");
static_assert(models::copy_constructible<const int&>(), "");
static_assert(!models::copy_constructible<const int&&>(), "");
static_assert(!models::copy_constructible<int[4]>(), "");
static_assert(!models::copy_constructible<void()>(), "");

static_assert(models::copy_constructible<copyable>(), "");
static_assert(!models::copy_constructible<moveonly>(), "");
static_assert(!models::copy_constructible<nonmovable>(), "");
static_assert(!models::copy_constructible<copyonly>(), "");
static_assert(!models::copy_constructible<explicit_move>(), "");
static_assert(!models::copy_constructible<explicit_copy>(), "");
static_assert(models::copy_constructible<nonmovable &>(), "");
static_assert(!models::copy_constructible<nonmovable &&>(), "");
static_assert(models::copy_constructible<const nonmovable &>(), "");
static_assert(!models::copy_constructible<const nonmovable &&>(), "");

static_assert(models::movable<int>(), "");
static_assert(!models::movable<const int>(), "");
static_assert(models::movable<double>(), "");
static_assert(!models::movable<void>(), "");
static_assert(models::movable<copyable>(), "");
static_assert(models::movable<moveonly>(), "");
static_assert(!models::movable<nonmovable>(), "");
static_assert(!models::movable<copyonly>(), "");

static_assert(models::copyable<int>(), "");
static_assert(!models::copyable<const int>(), "");
static_assert(models::copyable<double>(), "");
static_assert(!models::copyable<void>(), "");
static_assert(models::copyable<copyable>(), "");
static_assert(!models::copyable<moveonly>(), "");
static_assert(!models::copyable<nonmovable>(), "");
static_assert(!models::copyable<copyonly>(), "");

static_assert(models::semiregular<int>(), "");
static_assert(models::semiregular<double>(), "");
static_assert(!models::semiregular<void>(), "");
static_assert(!models::semiregular<int&>(), "");
static_assert(models::semiregular<semiregular>(), "");
static_assert(models::semiregular<regular>(), "");
static_assert(models::semiregular<copyable>(), "");
static_assert(!models::semiregular<moveonly>(), "");
static_assert(!models::semiregular<nonmovable>(), "");
static_assert(!models::semiregular<copyonly>(), "");
static_assert(!models::semiregular<explicit_move>(), "");
static_assert(!models::semiregular<explicit_copy>(), "");

static_assert(models::regular<int>(), "");
static_assert(models::regular<double>(), "");
static_assert(!models::regular<void>(), "");
static_assert(!models::regular<int&>(), "");
static_assert(!models::regular<semiregular>(), "");
static_assert(models::regular<regular>(), "");
static_assert(!models::regular<copyable>(), "");
static_assert(!models::regular<moveonly>(), "");
static_assert(!models::regular<nonmovable>(), "");
static_assert(!models::regular<copyonly>(), "");
static_assert(!models::regular<explicit_move>(), "");
static_assert(!models::regular<explicit_copy>(), "");

int main() {}
