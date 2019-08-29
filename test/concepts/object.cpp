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
#include <stl2/detail/concepts/object.hpp>

namespace ranges = std::experimental::ranges;

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

static_assert(ranges::destructible<int>);
static_assert(ranges::destructible<const int>);
static_assert(!ranges::destructible<void>);
static_assert(ranges::destructible<int&>);
static_assert(!ranges::destructible<void()>);
static_assert(ranges::destructible<void(*)()>);
static_assert(ranges::destructible<void(&)()>);
static_assert(!ranges::destructible<int[]>);
static_assert(ranges::destructible<int[2]>);
static_assert(ranges::destructible<int(*)[2]>);
static_assert(ranges::destructible<int(&)[2]>);
static_assert(ranges::destructible<moveonly>);
static_assert(ranges::destructible<nonmovable>);
static_assert(!ranges::destructible<indestructible>);
static_assert(!ranges::destructible<throwing_destructor>);

static_assert(ranges::constructible_from<int>);
static_assert(ranges::constructible_from<int const>);
static_assert(!ranges::constructible_from<int const&>);
static_assert(!ranges::constructible_from<int()>);
static_assert(!ranges::constructible_from<int(&)()>);
static_assert(!ranges::constructible_from<int[]>);
static_assert(ranges::constructible_from<int[5]>);
static_assert(!ranges::constructible_from<nondefaultconstructible>);
static_assert(ranges::constructible_from<int const(&)[5], int(&)[5]>);
static_assert(!ranges::constructible_from<int, int(&)[3]>);

static_assert(ranges::constructible_from<int, int>);
static_assert(ranges::constructible_from<int, int&>);
static_assert(ranges::constructible_from<int, int&&>);
static_assert(ranges::constructible_from<int, const int>);
static_assert(ranges::constructible_from<int, const int&>);
static_assert(ranges::constructible_from<int, const int&&>);

static_assert(ranges::constructible_from<copyable, copyable>);
static_assert(ranges::constructible_from<copyable, copyable&>);
static_assert(ranges::constructible_from<copyable, copyable&&>);
static_assert(ranges::constructible_from<copyable, const copyable>);
static_assert(ranges::constructible_from<copyable, const copyable&>);
static_assert(ranges::constructible_from<copyable, const copyable&&>);

static_assert(!ranges::constructible_from<int&, int>);
static_assert(ranges::constructible_from<int&, int&>);
static_assert(!ranges::constructible_from<int&, int&&>);
static_assert(!ranges::constructible_from<int&, const int>);
static_assert(!ranges::constructible_from<int&, const int&>);
static_assert(!ranges::constructible_from<int&, const int&&>);

static_assert(ranges::constructible_from<const int&, int>);
static_assert(ranges::constructible_from<const int&, int&>);
static_assert(ranges::constructible_from<const int&, int&&>);
static_assert(ranges::constructible_from<const int&, const int>);
static_assert(ranges::constructible_from<const int&, const int&>);
static_assert(ranges::constructible_from<const int&, const int&&>);

static_assert(ranges::constructible_from<int&&, int>);
static_assert(!ranges::constructible_from<int&&, int&>);
static_assert(ranges::constructible_from<int&&, int&&>);
static_assert(!ranges::constructible_from<int&&, const int>);
static_assert(!ranges::constructible_from<int&&, const int&>);
static_assert(!ranges::constructible_from<int&&, const int&&>);

static_assert(ranges::constructible_from<const int&&, int>);
static_assert(!ranges::constructible_from<const int&&, int&>);
static_assert(ranges::constructible_from<const int&&, int&&>);
static_assert(ranges::constructible_from<const int&&, const int>);
static_assert(!ranges::constructible_from<const int&&, const int&>);
static_assert(ranges::constructible_from<const int&&, const int&&>);

static_assert(ranges::constructible_from<XXX, int>);

static_assert(ranges::default_initializable<int>);
static_assert(ranges::default_initializable<int const>);
static_assert(!ranges::default_initializable<int&>);
static_assert(!ranges::default_initializable<int const&>);
static_assert(!ranges::default_initializable<int()>);
static_assert(!ranges::default_initializable<int(&)()>);
static_assert(ranges::default_initializable<double>);
static_assert(!ranges::default_initializable<void>);
static_assert(!ranges::default_initializable<int[]>);
static_assert(ranges::default_initializable<int[2]>);
static_assert(!ranges::default_initializable<nondefaultconstructible>);

namespace pathological_explicit_default_constructor {
	struct S0 { explicit S0() = default; };
	struct S1 { S0 x; };
#if STL2_WORKAROUND_GCC_UNKNOWN0 || STL2_WORKAROUND_MSVC_830372
	static_assert(ranges::default_initializable<S1>);
#else // ^^^ workaround / no workaround vvv
	static_assert(!ranges::default_initializable<S1>);
#endif // STL2_WORKAROUND_GCC_UNKNOWN0
}

static_assert(ranges::default_initializable<explicit_move>);
static_assert(ranges::default_initializable<explicit_copy>);
static_assert(!ranges::default_initializable<deleted_default>);

static_assert(!ranges::move_constructible<void>);
static_assert(ranges::move_constructible<int>);
static_assert(ranges::move_constructible<const int>);
static_assert(!ranges::move_constructible<int[4]>);
static_assert(!ranges::move_constructible<void()>);
static_assert(ranges::move_constructible<int &>);
static_assert(ranges::move_constructible<int &&>);
static_assert(ranges::move_constructible<const int &>);
static_assert(ranges::move_constructible<const int &&>);

static_assert(ranges::constructible_from<moveonly, moveonly>);
static_assert(ranges::move_constructible<copyable>);
static_assert(ranges::move_constructible<moveonly>);
static_assert(!ranges::move_constructible<nonmovable>);
static_assert(!ranges::move_constructible<copyonly>);
static_assert(!ranges::move_constructible<explicit_move>);
static_assert(ranges::move_constructible<explicit_copy>);

static_assert(ranges::move_constructible<nonmovable &>);
static_assert(ranges::move_constructible<nonmovable &&>);
static_assert(ranges::move_constructible<const nonmovable &>);
static_assert(ranges::move_constructible<const nonmovable &&>);

static_assert(!ranges::copy_constructible<void>);
static_assert(ranges::copy_constructible<int>);
static_assert(ranges::copy_constructible<const int>);
static_assert(ranges::copy_constructible<int&>);
static_assert(!ranges::copy_constructible<int&&>);
static_assert(ranges::copy_constructible<const int&>);
static_assert(!ranges::copy_constructible<const int&&>);
static_assert(!ranges::copy_constructible<int[4]>);
static_assert(!ranges::copy_constructible<void()>);

static_assert(ranges::copy_constructible<copyable>);
static_assert(!ranges::copy_constructible<moveonly>);
static_assert(!ranges::copy_constructible<nonmovable>);
static_assert(!ranges::copy_constructible<copyonly>);
static_assert(!ranges::copy_constructible<explicit_move>);
static_assert(!ranges::copy_constructible<explicit_copy>);
static_assert(ranges::copy_constructible<nonmovable &>);
static_assert(!ranges::copy_constructible<nonmovable &&>);
static_assert(ranges::copy_constructible<const nonmovable &>);
static_assert(!ranges::copy_constructible<const nonmovable &&>);

#if !STL2_WORKAROUND_MSVC_106654
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
static_assert(!ranges::copy_constructible<not_mutable_ref>);
static_assert(!ranges::copy_constructible<not_const_ref_ref>);
#endif // STL2_WORKAROUND_MSVC_106654

static_assert(ranges::movable<int>);
static_assert(!ranges::movable<const int>);
static_assert(ranges::movable<double>);
static_assert(!ranges::movable<void>);
static_assert(ranges::movable<copyable>);
static_assert(ranges::movable<moveonly>);
static_assert(!ranges::movable<nonmovable>);
static_assert(!ranges::movable<copyonly>);

static_assert(ranges::copyable<int>);
static_assert(!ranges::copyable<const int>);
static_assert(ranges::copyable<double>);
static_assert(!ranges::copyable<void>);
static_assert(ranges::copyable<copyable>);
static_assert(!ranges::copyable<moveonly>);
static_assert(!ranges::copyable<nonmovable>);
static_assert(!ranges::copyable<copyonly>);

static_assert(ranges::semiregular<int>);
static_assert(ranges::semiregular<double>);
static_assert(!ranges::semiregular<void>);
static_assert(!ranges::semiregular<int&>);
static_assert(ranges::semiregular<semiregular>);
static_assert(ranges::semiregular<regular>);
static_assert(ranges::semiregular<copyable>);
static_assert(!ranges::semiregular<moveonly>);
static_assert(!ranges::semiregular<nonmovable>);
static_assert(!ranges::semiregular<copyonly>);
static_assert(!ranges::semiregular<explicit_move>);
static_assert(!ranges::semiregular<explicit_copy>);

static_assert(ranges::regular<int>);
static_assert(ranges::regular<double>);
static_assert(!ranges::regular<void>);
static_assert(!ranges::regular<int&>);
static_assert(!ranges::regular<semiregular>);
static_assert(ranges::regular<regular>);
static_assert(!ranges::regular<copyable>);
static_assert(!ranges::regular<moveonly>);
static_assert(!ranges::regular<nonmovable>);
static_assert(!ranges::regular<copyonly>);
static_assert(!ranges::regular<explicit_move>);
static_assert(!ranges::regular<explicit_copy>);

static_assert(ranges::constructible_from<std::initializer_list<int>>);
static_assert(ranges::default_initializable<std::initializer_list<int>>);

static_assert(ranges::constructible_from<int*>);
static_assert(ranges::default_initializable<int*>);

// https://github.com/ericniebler/stl2/issues/301
static_assert(!ranges::constructible_from<int&, long&>);

// https://github.com/ericniebler/stl2/issues/310
static_assert(!ranges::movable<int&&>);

int main() {}
