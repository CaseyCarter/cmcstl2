// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include "validate.hpp"

#if VALIDATE_STL2
#include <stl2/detail/concepts/object.hpp>
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

struct throwing_destructor {
	~throwing_destructor() noexcept(false);
};

struct explicit_default {
	explicit explicit_default() {}
};

struct deleted_default {
	deleted_default() = delete;
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

CONCEPT_ASSERT(ranges::destructible<int>);
CONCEPT_ASSERT(ranges::destructible<const int>);
CONCEPT_ASSERT(!ranges::destructible<void>);
CONCEPT_ASSERT(ranges::destructible<int&>);
CONCEPT_ASSERT(!ranges::destructible<void()>);
CONCEPT_ASSERT(ranges::destructible<void(*)()>);
CONCEPT_ASSERT(ranges::destructible<void(&)()>);
CONCEPT_ASSERT(!ranges::destructible<int[]>);
CONCEPT_ASSERT(ranges::destructible<int[2]>);
CONCEPT_ASSERT(ranges::destructible<int(*)[2]>);
CONCEPT_ASSERT(ranges::destructible<int(&)[2]>);
CONCEPT_ASSERT(ranges::destructible<moveonly>);
CONCEPT_ASSERT(ranges::destructible<nonmovable>);
CONCEPT_ASSERT(!ranges::destructible<indestructible>);
CONCEPT_ASSERT(!ranges::destructible<throwing_destructor>);

CONCEPT_ASSERT(ranges::constructible_from<int>);
CONCEPT_ASSERT(ranges::constructible_from<int const>);
CONCEPT_ASSERT(!ranges::constructible_from<int const&>);
CONCEPT_ASSERT(!ranges::constructible_from<int()>);
CONCEPT_ASSERT(!ranges::constructible_from<int(&)()>);
CONCEPT_ASSERT(!ranges::constructible_from<int[]>);
CONCEPT_ASSERT(ranges::constructible_from<int[5]>);
CONCEPT_ASSERT(!ranges::constructible_from<nondefaultconstructible>);
CONCEPT_ASSERT(ranges::constructible_from<int const(&)[5], int(&)[5]>);
CONCEPT_ASSERT(!ranges::constructible_from<int, int(&)[3]>);

CONCEPT_ASSERT(ranges::constructible_from<int, int>);
CONCEPT_ASSERT(ranges::constructible_from<int, int&>);
CONCEPT_ASSERT(ranges::constructible_from<int, int&&>);
CONCEPT_ASSERT(ranges::constructible_from<int, const int>);
CONCEPT_ASSERT(ranges::constructible_from<int, const int&>);
CONCEPT_ASSERT(ranges::constructible_from<int, const int&&>);

CONCEPT_ASSERT(ranges::constructible_from<copyable, copyable>);
CONCEPT_ASSERT(ranges::constructible_from<copyable, copyable&>);
CONCEPT_ASSERT(ranges::constructible_from<copyable, copyable&&>);
CONCEPT_ASSERT(ranges::constructible_from<copyable, const copyable>);
CONCEPT_ASSERT(ranges::constructible_from<copyable, const copyable&>);
CONCEPT_ASSERT(ranges::constructible_from<copyable, const copyable&&>);

CONCEPT_ASSERT(!ranges::constructible_from<int&, int>);
CONCEPT_ASSERT(ranges::constructible_from<int&, int&>);
CONCEPT_ASSERT(!ranges::constructible_from<int&, int&&>);
CONCEPT_ASSERT(!ranges::constructible_from<int&, const int>);
CONCEPT_ASSERT(!ranges::constructible_from<int&, const int&>);
CONCEPT_ASSERT(!ranges::constructible_from<int&, const int&&>);

CONCEPT_ASSERT(ranges::constructible_from<const int&, int>);
CONCEPT_ASSERT(ranges::constructible_from<const int&, int&>);
CONCEPT_ASSERT(ranges::constructible_from<const int&, int&&>);
CONCEPT_ASSERT(ranges::constructible_from<const int&, const int>);
CONCEPT_ASSERT(ranges::constructible_from<const int&, const int&>);
CONCEPT_ASSERT(ranges::constructible_from<const int&, const int&&>);

CONCEPT_ASSERT(ranges::constructible_from<int&&, int>);
CONCEPT_ASSERT(!ranges::constructible_from<int&&, int&>);
CONCEPT_ASSERT(ranges::constructible_from<int&&, int&&>);
CONCEPT_ASSERT(!ranges::constructible_from<int&&, const int>);
CONCEPT_ASSERT(!ranges::constructible_from<int&&, const int&>);
CONCEPT_ASSERT(!ranges::constructible_from<int&&, const int&&>);

CONCEPT_ASSERT(ranges::constructible_from<const int&&, int>);
CONCEPT_ASSERT(!ranges::constructible_from<const int&&, int&>);
CONCEPT_ASSERT(ranges::constructible_from<const int&&, int&&>);
CONCEPT_ASSERT(ranges::constructible_from<const int&&, const int>);
CONCEPT_ASSERT(!ranges::constructible_from<const int&&, const int&>);
CONCEPT_ASSERT(ranges::constructible_from<const int&&, const int&&>);

CONCEPT_ASSERT(ranges::constructible_from<XXX, int>);

CONCEPT_ASSERT(ranges::default_initializable<int>);
CONCEPT_ASSERT(ranges::default_initializable<int const>);
CONCEPT_ASSERT(!ranges::default_initializable<int&>);
CONCEPT_ASSERT(!ranges::default_initializable<int const&>);
CONCEPT_ASSERT(!ranges::default_initializable<int()>);
CONCEPT_ASSERT(!ranges::default_initializable<int(&)()>);
CONCEPT_ASSERT(ranges::default_initializable<double>);
CONCEPT_ASSERT(!ranges::default_initializable<void>);
CONCEPT_ASSERT(!ranges::default_initializable<int[]>);
CONCEPT_ASSERT(ranges::default_initializable<int[2]>);
CONCEPT_ASSERT(!ranges::default_initializable<nondefaultconstructible>);

namespace pathological_explicit_default_constructor {
	struct S0 { explicit S0() = default; };
	struct S1 { S0 x; };
#if STL2_WORKAROUND_GCC_UNKNOWN0
	CONCEPT_ASSERT(ranges::default_initializable<S1>);
#else // ^^^ workaround / no workaround vvv
	CONCEPT_ASSERT(!ranges::default_initializable<S1>);
#endif // STL2_WORKAROUND_GCC_UNKNOWN0
}

CONCEPT_ASSERT(ranges::default_initializable<explicit_move>);
CONCEPT_ASSERT(ranges::default_initializable<explicit_copy>);
CONCEPT_ASSERT(!ranges::default_initializable<deleted_default>);

CONCEPT_ASSERT(!ranges::move_constructible<void>);
CONCEPT_ASSERT(ranges::move_constructible<int>);
CONCEPT_ASSERT(ranges::move_constructible<const int>);
CONCEPT_ASSERT(!ranges::move_constructible<int[4]>);
CONCEPT_ASSERT(!ranges::move_constructible<void()>);
CONCEPT_ASSERT(ranges::move_constructible<int &>);
CONCEPT_ASSERT(ranges::move_constructible<int &&>);
CONCEPT_ASSERT(ranges::move_constructible<const int &>);
CONCEPT_ASSERT(ranges::move_constructible<const int &&>);

CONCEPT_ASSERT(ranges::constructible_from<moveonly, moveonly>);
CONCEPT_ASSERT(ranges::move_constructible<copyable>);
CONCEPT_ASSERT(ranges::move_constructible<moveonly>);
CONCEPT_ASSERT(!ranges::move_constructible<nonmovable>);
CONCEPT_ASSERT(!ranges::move_constructible<copyonly>);
CONCEPT_ASSERT(!ranges::move_constructible<explicit_move>);
CONCEPT_ASSERT(ranges::move_constructible<explicit_copy>);

CONCEPT_ASSERT(ranges::move_constructible<nonmovable &>);
CONCEPT_ASSERT(ranges::move_constructible<nonmovable &&>);
CONCEPT_ASSERT(ranges::move_constructible<const nonmovable &>);
CONCEPT_ASSERT(ranges::move_constructible<const nonmovable &&>);

CONCEPT_ASSERT(!ranges::copy_constructible<void>);
CONCEPT_ASSERT(ranges::copy_constructible<int>);
CONCEPT_ASSERT(ranges::copy_constructible<const int>);
CONCEPT_ASSERT(ranges::copy_constructible<int&>);
CONCEPT_ASSERT(!ranges::copy_constructible<int&&>);
CONCEPT_ASSERT(ranges::copy_constructible<const int&>);
CONCEPT_ASSERT(!ranges::copy_constructible<const int&&>);
CONCEPT_ASSERT(!ranges::copy_constructible<int[4]>);
CONCEPT_ASSERT(!ranges::copy_constructible<void()>);

CONCEPT_ASSERT(ranges::copy_constructible<copyable>);
CONCEPT_ASSERT(!ranges::copy_constructible<moveonly>);
CONCEPT_ASSERT(!ranges::copy_constructible<nonmovable>);
CONCEPT_ASSERT(!ranges::copy_constructible<copyonly>);
CONCEPT_ASSERT(!ranges::copy_constructible<explicit_move>);
CONCEPT_ASSERT(!ranges::copy_constructible<explicit_copy>);
CONCEPT_ASSERT(ranges::copy_constructible<nonmovable &>);
CONCEPT_ASSERT(!ranges::copy_constructible<nonmovable &&>);
CONCEPT_ASSERT(ranges::copy_constructible<const nonmovable &>);
CONCEPT_ASSERT(!ranges::copy_constructible<const nonmovable &&>);

// https://github.com/ericniebler/stl2/issues/301
struct not_mutable_ref {
	not_mutable_ref() = default;
	not_mutable_ref(const not_mutable_ref&) = default;
	not_mutable_ref(not_mutable_ref&&) = default;
	not_mutable_ref(not_mutable_ref&) = delete;
};
struct not_const_ref_ref {
	not_const_ref_ref() = default;
	not_const_ref_ref(const not_const_ref_ref&) = default;
	not_const_ref_ref(not_const_ref_ref&&) = default;
	not_const_ref_ref(const not_const_ref_ref&&) = delete;
};
CONCEPT_ASSERT(!ranges::copy_constructible<not_mutable_ref>);
CONCEPT_ASSERT(!ranges::copy_constructible<not_const_ref_ref>);

CONCEPT_ASSERT(ranges::movable<int>);
CONCEPT_ASSERT(!ranges::movable<const int>);
CONCEPT_ASSERT(ranges::movable<double>);
CONCEPT_ASSERT(!ranges::movable<void>);
CONCEPT_ASSERT(ranges::movable<copyable>);
CONCEPT_ASSERT(ranges::movable<moveonly>);
CONCEPT_ASSERT(!ranges::movable<nonmovable>);
CONCEPT_ASSERT(!ranges::movable<copyonly>);

CONCEPT_ASSERT(ranges::copyable<int>);
CONCEPT_ASSERT(!ranges::copyable<const int>);
CONCEPT_ASSERT(ranges::copyable<double>);
CONCEPT_ASSERT(!ranges::copyable<void>);
CONCEPT_ASSERT(ranges::copyable<copyable>);
CONCEPT_ASSERT(!ranges::copyable<moveonly>);
CONCEPT_ASSERT(!ranges::copyable<nonmovable>);
CONCEPT_ASSERT(!ranges::copyable<copyonly>);

CONCEPT_ASSERT(ranges::semiregular<int>);
CONCEPT_ASSERT(ranges::semiregular<double>);
CONCEPT_ASSERT(!ranges::semiregular<void>);
CONCEPT_ASSERT(!ranges::semiregular<int&>);
CONCEPT_ASSERT(ranges::semiregular<semiregular>);
CONCEPT_ASSERT(ranges::semiregular<regular>);
CONCEPT_ASSERT(ranges::semiregular<copyable>);
CONCEPT_ASSERT(!ranges::semiregular<moveonly>);
CONCEPT_ASSERT(!ranges::semiregular<nonmovable>);
CONCEPT_ASSERT(!ranges::semiregular<copyonly>);
CONCEPT_ASSERT(!ranges::semiregular<explicit_move>);
CONCEPT_ASSERT(!ranges::semiregular<explicit_copy>);

CONCEPT_ASSERT(ranges::regular<int>);
CONCEPT_ASSERT(ranges::regular<double>);
CONCEPT_ASSERT(!ranges::regular<void>);
CONCEPT_ASSERT(!ranges::regular<int&>);
CONCEPT_ASSERT(!ranges::regular<semiregular>);
CONCEPT_ASSERT(ranges::regular<regular>);
CONCEPT_ASSERT(!ranges::regular<copyable>);
CONCEPT_ASSERT(!ranges::regular<moveonly>);
CONCEPT_ASSERT(!ranges::regular<nonmovable>);
CONCEPT_ASSERT(!ranges::regular<copyonly>);
CONCEPT_ASSERT(!ranges::regular<explicit_move>);
CONCEPT_ASSERT(!ranges::regular<explicit_copy>);

CONCEPT_ASSERT(ranges::constructible_from<std::initializer_list<int>>);
CONCEPT_ASSERT(ranges::default_initializable<std::initializer_list<int>>);

CONCEPT_ASSERT(ranges::constructible_from<int*>);
CONCEPT_ASSERT(ranges::default_initializable<int*>);

// https://github.com/ericniebler/stl2/issues/301
CONCEPT_ASSERT(!ranges::constructible_from<int&, long&>);

// https://github.com/ericniebler/stl2/issues/310
CONCEPT_ASSERT(!ranges::movable<int&&>);

int main() {}
