#include "validate.hpp"

#if VALIDATE_RANGES
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

CONCEPT_ASSERT(models::destructible<int>());
CONCEPT_ASSERT(models::destructible<const int>());
CONCEPT_ASSERT(!models::destructible<void>());
CONCEPT_ASSERT(!models::destructible<int&>());
CONCEPT_ASSERT(!models::destructible<void()>());
CONCEPT_ASSERT(models::destructible<void(*)()>());
CONCEPT_ASSERT(!models::destructible<void(&)()>());
CONCEPT_ASSERT(!models::destructible<int[]>());
CONCEPT_ASSERT(!models::destructible<int[2]>());
CONCEPT_ASSERT(models::destructible<int(*)[2]>());
CONCEPT_ASSERT(!models::destructible<int(&)[2]>());
CONCEPT_ASSERT(models::destructible<moveonly>());
CONCEPT_ASSERT(models::destructible<nonmovable>());
CONCEPT_ASSERT(!models::destructible<indestructible>());
CONCEPT_ASSERT(!models::destructible<incomplete>());

CONCEPT_ASSERT(models::destructible<partial_overloaded_address>());
CONCEPT_ASSERT(models::destructible<overloaded_address>());
CONCEPT_ASSERT(!models::destructible<bad_overloaded_address>());
CONCEPT_ASSERT(!models::destructible<bad_overloaded_const_address>());

CONCEPT_ASSERT(models::constructible<int>());
CONCEPT_ASSERT(models::constructible<int const>());
CONCEPT_ASSERT(!models::constructible<int const&>());
CONCEPT_ASSERT(!models::constructible<int ()>());
CONCEPT_ASSERT(!models::constructible<int(&)()>());
CONCEPT_ASSERT(!models::constructible<int[]>());
CONCEPT_ASSERT(!models::constructible<int[5]>());
CONCEPT_ASSERT(!models::constructible<nondefaultconstructible>());
CONCEPT_ASSERT(models::constructible<int const(&)[5], int(&)[5]>());
CONCEPT_ASSERT(!models::constructible<int, int(&)[3]>());

CONCEPT_ASSERT(models::constructible_object<int, int>());
CONCEPT_ASSERT(models::constructible_object<int, int&>());
CONCEPT_ASSERT(models::constructible_object<int, int&&>());
CONCEPT_ASSERT(models::constructible_object<int, const int>());
CONCEPT_ASSERT(models::constructible_object<int, const int&>());
CONCEPT_ASSERT(models::constructible_object<int, const int&&>());
CONCEPT_ASSERT(models::constructible<int, int>());
CONCEPT_ASSERT(models::constructible<int, int&>());
CONCEPT_ASSERT(models::constructible<int, int&&>());
CONCEPT_ASSERT(models::constructible<int, const int>());
CONCEPT_ASSERT(models::constructible<int, const int&>());
CONCEPT_ASSERT(models::constructible<int, const int&&>());

CONCEPT_ASSERT(models::constructible_object<copyable, copyable>());
CONCEPT_ASSERT(models::constructible_object<copyable, copyable&>());
CONCEPT_ASSERT(models::constructible_object<copyable, copyable&&>());
CONCEPT_ASSERT(models::constructible_object<copyable, const copyable>());
CONCEPT_ASSERT(models::constructible_object<copyable, const copyable&>());
CONCEPT_ASSERT(models::constructible_object<copyable, const copyable&&>());
CONCEPT_ASSERT(models::constructible<copyable, copyable>());
CONCEPT_ASSERT(models::constructible<copyable, copyable&>());
CONCEPT_ASSERT(models::constructible<copyable, copyable&&>());
CONCEPT_ASSERT(models::constructible<copyable, const copyable>());
CONCEPT_ASSERT(models::constructible<copyable, const copyable&>());
CONCEPT_ASSERT(models::constructible<copyable, const copyable&&>());

CONCEPT_ASSERT(!models::bindable_reference<int&, int>());
CONCEPT_ASSERT(models::bindable_reference<int&, int&>());
CONCEPT_ASSERT(!models::bindable_reference<int&, int&&>());
CONCEPT_ASSERT(!models::bindable_reference<int&, const int>());
CONCEPT_ASSERT(!models::bindable_reference<int&, const int&>());
CONCEPT_ASSERT(!models::bindable_reference<int&, const int&&>());
CONCEPT_ASSERT(!models::constructible<int&, int>());
CONCEPT_ASSERT(models::constructible<int&, int&>());
CONCEPT_ASSERT(!models::constructible<int&, int&&>());
CONCEPT_ASSERT(!models::constructible<int&, const int>());
CONCEPT_ASSERT(!models::constructible<int&, const int&>());
CONCEPT_ASSERT(!models::constructible<int&, const int&&>());

CONCEPT_ASSERT(models::bindable_reference<const int&, int>());
CONCEPT_ASSERT(models::bindable_reference<const int&, int&>());
CONCEPT_ASSERT(models::bindable_reference<const int&, int&&>());
CONCEPT_ASSERT(models::bindable_reference<const int&, const int>());
CONCEPT_ASSERT(models::bindable_reference<const int&, const int&>());
CONCEPT_ASSERT(models::bindable_reference<const int&, const int&&>());
CONCEPT_ASSERT(models::constructible<const int&, int>());
CONCEPT_ASSERT(models::constructible<const int&, int&>());
CONCEPT_ASSERT(models::constructible<const int&, int&&>());
CONCEPT_ASSERT(models::constructible<const int&, const int>());
CONCEPT_ASSERT(models::constructible<const int&, const int&>());
CONCEPT_ASSERT(models::constructible<const int&, const int&&>());

CONCEPT_ASSERT(models::bindable_reference<int&&, int>());
CONCEPT_ASSERT(!models::bindable_reference<int&&, int&>());
CONCEPT_ASSERT(models::bindable_reference<int&&, int&&>());
CONCEPT_ASSERT(!models::bindable_reference<int&&, const int>());
CONCEPT_ASSERT(!models::bindable_reference<int&&, const int&>());
CONCEPT_ASSERT(!models::bindable_reference<int&&, const int&&>());
CONCEPT_ASSERT(models::constructible<int&&, int>());
CONCEPT_ASSERT(!models::constructible<int&&, int&>());
CONCEPT_ASSERT(models::constructible<int&&, int&&>());
CONCEPT_ASSERT(!models::constructible<int&&, const int>());
CONCEPT_ASSERT(!models::constructible<int&&, const int&>());
CONCEPT_ASSERT(!models::constructible<int&&, const int&&>());

CONCEPT_ASSERT(models::bindable_reference<const int&&, int>());
CONCEPT_ASSERT(!models::bindable_reference<const int&&, int&>());
CONCEPT_ASSERT(models::bindable_reference<const int&&, int&&>());
CONCEPT_ASSERT(models::bindable_reference<const int&&, const int>());
CONCEPT_ASSERT(!models::bindable_reference<const int&&, const int&>());
CONCEPT_ASSERT(models::bindable_reference<const int&&, const int&&>());
CONCEPT_ASSERT(models::constructible<const int&&, int>());
CONCEPT_ASSERT(!models::constructible<const int&&, int&>());
CONCEPT_ASSERT(models::constructible<const int&&, int&&>());
CONCEPT_ASSERT(models::constructible<const int&&, const int>());
CONCEPT_ASSERT(!models::constructible<const int&&, const int&>());
CONCEPT_ASSERT(models::constructible<const int&&, const int&&>());

CONCEPT_ASSERT(models::constructible<XXX, int>());

CONCEPT_ASSERT(models::default_constructible<int>());
CONCEPT_ASSERT(models::default_constructible<int const>());
CONCEPT_ASSERT(!models::default_constructible<int&>());
CONCEPT_ASSERT(!models::default_constructible<int const&>());
CONCEPT_ASSERT(!models::default_constructible<int ()>());
CONCEPT_ASSERT(!models::default_constructible<int(&)()>());
CONCEPT_ASSERT(models::default_constructible<double>());
CONCEPT_ASSERT(!models::default_constructible<void>());
CONCEPT_ASSERT(!models::default_constructible<int[]>());
CONCEPT_ASSERT(!models::default_constructible<int[2]>());
CONCEPT_ASSERT(!models::default_constructible<nondefaultconstructible>());

// It's hard to catch explicit default constructors, see
// http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1518.
// CONCEPT_ASSERT(!models::default_constructible<explicit_default>());
CONCEPT_ASSERT(models::default_constructible<explicit_move>());
CONCEPT_ASSERT(models::default_constructible<explicit_copy>());

CONCEPT_ASSERT(!models::move_constructible<void>());
CONCEPT_ASSERT(models::move_constructible<int>());
CONCEPT_ASSERT(models::move_constructible<const int>());
CONCEPT_ASSERT(!models::move_constructible<int[4]>());
CONCEPT_ASSERT(!models::move_constructible<void()>());
CONCEPT_ASSERT(models::move_constructible<int &>());
CONCEPT_ASSERT(models::move_constructible<int &&>());
CONCEPT_ASSERT(models::move_constructible<const int &>());
CONCEPT_ASSERT(models::move_constructible<const int &&>());

CONCEPT_ASSERT(models::constructible_object<moveonly, moveonly>());
CONCEPT_ASSERT(models::constructible<moveonly, moveonly>());
CONCEPT_ASSERT(models::move_constructible<copyable>());
CONCEPT_ASSERT(models::move_constructible<moveonly>());
CONCEPT_ASSERT(!models::move_constructible<nonmovable>());
CONCEPT_ASSERT(!models::move_constructible<copyonly>());
CONCEPT_ASSERT(!models::move_constructible<explicit_move>());
CONCEPT_ASSERT(models::move_constructible<explicit_copy>());

CONCEPT_ASSERT(models::move_constructible<nonmovable &>());
CONCEPT_ASSERT(models::move_constructible<nonmovable &&>());
CONCEPT_ASSERT(models::move_constructible<const nonmovable &>());
CONCEPT_ASSERT(models::move_constructible<const nonmovable &&>());

CONCEPT_ASSERT(!models::copy_constructible<void>());
CONCEPT_ASSERT(models::copy_constructible<int>());
CONCEPT_ASSERT(models::copy_constructible<const int>());
CONCEPT_ASSERT(models::copy_constructible<int&>());
CONCEPT_ASSERT(!models::copy_constructible<int&&>());
CONCEPT_ASSERT(models::copy_constructible<const int&>());
CONCEPT_ASSERT(!models::copy_constructible<const int&&>());
CONCEPT_ASSERT(!models::copy_constructible<int[4]>());
CONCEPT_ASSERT(!models::copy_constructible<void()>());

CONCEPT_ASSERT(models::copy_constructible<copyable>());
CONCEPT_ASSERT(!models::copy_constructible<moveonly>());
CONCEPT_ASSERT(!models::copy_constructible<nonmovable>());
CONCEPT_ASSERT(!models::copy_constructible<copyonly>());
CONCEPT_ASSERT(!models::copy_constructible<explicit_move>());
CONCEPT_ASSERT(!models::copy_constructible<explicit_copy>());
CONCEPT_ASSERT(models::copy_constructible<nonmovable &>());
CONCEPT_ASSERT(!models::copy_constructible<nonmovable &&>());
CONCEPT_ASSERT(models::copy_constructible<const nonmovable &>());
CONCEPT_ASSERT(!models::copy_constructible<const nonmovable &&>());

CONCEPT_ASSERT(models::movable<int>());
CONCEPT_ASSERT(!models::movable<const int>());
CONCEPT_ASSERT(models::movable<double>());
CONCEPT_ASSERT(!models::movable<void>());
CONCEPT_ASSERT(models::movable<copyable>());
CONCEPT_ASSERT(models::movable<moveonly>());
CONCEPT_ASSERT(!models::movable<nonmovable>());
CONCEPT_ASSERT(!models::movable<copyonly>());

CONCEPT_ASSERT(models::copyable<int>());
CONCEPT_ASSERT(!models::copyable<const int>());
CONCEPT_ASSERT(models::copyable<double>());
CONCEPT_ASSERT(!models::copyable<void>());
CONCEPT_ASSERT(models::copyable<copyable>());
CONCEPT_ASSERT(!models::copyable<moveonly>());
CONCEPT_ASSERT(!models::copyable<nonmovable>());
CONCEPT_ASSERT(!models::copyable<copyonly>());

CONCEPT_ASSERT(models::semiregular<int>());
CONCEPT_ASSERT(models::semiregular<double>());
CONCEPT_ASSERT(!models::semiregular<void>());
CONCEPT_ASSERT(!models::semiregular<int&>());
CONCEPT_ASSERT(models::semiregular<semiregular>());
CONCEPT_ASSERT(models::semiregular<regular>());
CONCEPT_ASSERT(models::semiregular<copyable>());
CONCEPT_ASSERT(!models::semiregular<moveonly>());
CONCEPT_ASSERT(!models::semiregular<nonmovable>());
CONCEPT_ASSERT(!models::semiregular<copyonly>());
CONCEPT_ASSERT(!models::semiregular<explicit_move>());
CONCEPT_ASSERT(!models::semiregular<explicit_copy>());

CONCEPT_ASSERT(models::regular<int>());
CONCEPT_ASSERT(models::regular<double>());
CONCEPT_ASSERT(!models::regular<void>());
CONCEPT_ASSERT(!models::regular<int&>());
CONCEPT_ASSERT(!models::regular<semiregular>());
CONCEPT_ASSERT(models::regular<regular>());
CONCEPT_ASSERT(!models::regular<copyable>());
CONCEPT_ASSERT(!models::regular<moveonly>());
CONCEPT_ASSERT(!models::regular<nonmovable>());
CONCEPT_ASSERT(!models::regular<copyonly>());
CONCEPT_ASSERT(!models::regular<explicit_move>());
CONCEPT_ASSERT(!models::regular<explicit_copy>());

CONCEPT_ASSERT(models::constructible_object<std::initializer_list<int>>());
CONCEPT_ASSERT(models::constructible<std::initializer_list<int>>());
CONCEPT_ASSERT(models::default_constructible<std::initializer_list<int>>());

CONCEPT_ASSERT(models::constructible_object<int*>());
CONCEPT_ASSERT(models::constructible<int*>());
CONCEPT_ASSERT(models::default_constructible<int*>());

int main() {}
