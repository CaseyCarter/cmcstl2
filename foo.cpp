// -*- compile-command: "(cd ~/cmcstl2/build && make && ./foo)" -*-

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

namespace swappable_test {
using stl2::concepts::models::swappable;
using stl2::swap;

static_assert(swappable<int&>(), "");
static_assert(swappable<int(&)[4]>(), "");
static_assert(swappable<int(&)[3][4], int(&)[3][4]>(), "");
static_assert(swappable<int(&)[3][4][1][2], int(&)[3][4][1][2]>(), "");
static_assert(!swappable<int, int>(), "");
static_assert(!swappable<int&, double&>(), "");
static_assert(!swappable<int(&)[4], bool(&)[4]>(), "");
static_assert(!swappable<int(&)[], int(&)[]>(), "");
static_assert(!swappable<int(&)[][4], int(&)[][4]>(), "");

static_assert(noexcept(swap(stl2::declval<int&>(),
                            stl2::declval<int&>())), "");

struct A {
  A() = default;
  A(A&&) = delete;
  A& operator=(A&&) = delete;
  friend void swap(A&, A&) noexcept {}
};
// A is Swappable despite non-Movable
static_assert(swappable<A&>(), "");
static_assert(noexcept(swap(stl2::declval<A&>(), stl2::declval<A&>())), "");

struct B {
  friend void swap(A&, B&) noexcept {}
  friend void swap(B&, A&) noexcept {}
};
static_assert(swappable<B&>(), "");
static_assert(swappable<A&, B&>(), "");
static_assert(swappable<B(&)[1], A(&)[1]>(), "");
static_assert(swappable<B(&)[1][3], A(&)[1][3]>(), "");
static_assert(!swappable<B(&)[3][1], A(&)[1][3]>(), "");

#ifdef STL2_SWAPPABLE_POINTERS
static_assert(swappable<int*,int*>(), "");
static_assert(swappable<int*,int&>(), "");
static_assert(swappable<int&,int*>(), "");
static_assert(noexcept(swap(stl2::declval<int*>(), stl2::declval<int&>())), "");
static_assert(!swappable<int*&,int&>(), "");
static_assert(swappable<int(*)[3][2][1], int(&)[3][2][1]>(), "");

#if 0 // FIXME: These cause the compiler to bomb memory
static_assert(swappable<A*,B*>(), "");
static_assert(swappable<A(*)[4], B(&)[4]>(), "");
#endif

static_assert(noexcept(swap(stl2::declval<A(*)[4]>(), stl2::declval<B(&)[4]>())), "");
#endif
} // namespace swappable_test

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

namespace adl_swap_detail {
using stl2::swap;

stl2::Swappable{T, U}
constexpr void adl_swap(T&& t, U&& u)
  noexcept(noexcept(swap(stl2::forward<T>(t), stl2::forward<U>(u)))) {
  swap(stl2::forward<T>(t), stl2::forward<U>(u));
}
} // namespace adl_swap_detail

using adl_swap_detail::adl_swap;

template <class T, std::size_t N>
struct array {
  T elements_[N];

  constexpr T& operator[](std::size_t i) noexcept {
    assert(i < N);
    return elements_[i];
  }

  constexpr const T& operator[](std::size_t i) const noexcept {
    assert(i < N);
    return elements_[i];
  }
};

template <class T, class U, std::size_t N>
  requires stl2::Swappable<T&, U&>
void swap(array<T, N>& a, array<U, N>& b)
  noexcept(noexcept(adl_swap(a.elements_, b.elements_))) {
  adl_swap(a.elements_, b.elements_);
}

template <class T, class U, std::size_t N>
  requires stl2::Swappable<T&, U&>
void swap(array<T, N>& a, U (&b)[N])
  noexcept(noexcept(adl_swap(a.elements_, b))) {
  adl_swap(a.elements_, b);
}

template <class T, class U, std::size_t N>
  requires stl2::Swappable<T&, U&>
void swap(T (&b)[N], array<U, N>& a)
  noexcept(noexcept(adl_swap(a.elements_, b))) {
  adl_swap(a.elements_, b);
}

void test_swap() {
  using stl2::concepts::models::swappable;

  {
    int a[2][2] = {{0, 1}, {2, 3}};
    int b[2][2] = {{4, 5}, {6, 7}};

    static_assert(swappable<decltype((a)),decltype((b))>(), "");
    adl_swap(a, b);
    static_assert(noexcept(adl_swap(a, b)), "");

    assert(a[0][0] == 4);
    assert(a[0][1] == 5);
    assert(a[1][0] == 6);
    assert(a[1][1] == 7);

    assert(b[0][0] == 0);
    assert(b[0][1] == 1);
    assert(b[1][0] == 2);
    assert(b[1][1] == 3);
  }

  {
    array<int, 4> a = {0,1,2,3};
    int b[4] = {4,5,6,7};

    static_assert(swappable<decltype(a[0]),decltype(b[0])>(), "");
    adl_swap(a[0], b[0]);
    static_assert(noexcept(adl_swap(a[0], b[0])), "");

    static_assert(swappable<decltype((a)),decltype((b))>(), "");
    adl_swap(a, b);
    static_assert(noexcept(adl_swap(a, b)), "");

    assert(a[0] == 0);
    assert(a[1] == 5);
    assert(a[2] == 6);
    assert(a[3] == 7);

    assert(b[0] == 4);
    assert(b[1] == 1);
    assert(b[2] == 2);
    assert(b[3] == 3);
  }

  {
    array<array<int, 2>, 3> a = {{{{0, 1}}, {{2, 3}}, {{4, 5}}}};
    int b[3][2] = {{6, 7}, {8, 9}, {10, 11}};

    static_assert(swappable<decltype(a[0]),decltype(b[0])>(), "");
    adl_swap(a[0], b[0]);
    static_assert(noexcept(adl_swap(a[0], b[0])), "");

    static_assert(swappable<decltype((a)),decltype((b))>(), "");
    adl_swap(a, b);
    static_assert(noexcept(adl_swap(a, b)), "");
  }

#ifdef STL2_SWAPPABLE_POINTERS
  {
    struct A {
      void foo(A& other) {
        static_assert(swappable<A*,A&>(), "");
        // adl_swap(this, other); // ICE
        stl2::swap(this, other);
      }
    };

    A a, b;
    a.foo(b);
  }
#endif
}
} // unnamed namespace

int main() {
  test_swap();
}
