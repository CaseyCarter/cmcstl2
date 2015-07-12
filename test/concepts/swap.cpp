// -*- compile-command: "(cd ~/cmcstl2/build && make swap && ./test/concepts/swap)" -*-

#include <stl2/concepts/object.hpp>
#include <stl2/utility.hpp>

#include <cassert>
#include <cstddef>

#include "../simple_test.hpp"

namespace swappable_test {
using stl2::ext::models::swappable;
using stl2::is_nothrow_swappable_v;
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
static_assert(!swappable<int(&)[3][4][1][2], int(&)[4][4][1][2]>(), "");

static_assert(noexcept(swap(stl2::declval<int&>(),
                            stl2::declval<int&>())), "");
static_assert(is_nothrow_swappable_v<int&, int&>, "");
static_assert(is_nothrow_swappable_v<int(&)[42], int(&)[42]>, "");
static_assert(is_nothrow_swappable_v<int(&)[6][7], int(&)[6][7]>, "");

struct A {
  A() = default;
  A(A&&) = delete;
  A& operator=(A&&) = delete;
  friend void swap(A&, A&) noexcept {}
};

static_assert(swappable<A&>(), "");
static_assert(noexcept(swap(stl2::declval<A&>(), stl2::declval<A&>())), "");
static_assert(is_nothrow_swappable_v<A&, A&>, "");

struct B {
  friend void swap(B&, B&) {}
};

static_assert(swappable<B&>(), "");
static_assert(!noexcept(swap(stl2::declval<B&>(), stl2::declval<B&>())), "");
static_assert(!is_nothrow_swappable_v<B&, B&>, "");

} // namespace swappable_test

namespace adl_swap_detail {
using stl2::swap;

stl2::Swappable{T, U}
constexpr void adl_swap(T&& t, U&& u)
  noexcept(noexcept(swap(stl2::forward<T>(t), stl2::forward<U>(u)))) {
  swap(stl2::forward<T>(t), stl2::forward<U>(u));
}
} // namespace adl_swap_detail

using adl_swap_detail::adl_swap;

#if 0 // No longer functional

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
  requires stl2::Swappable<T&, U&>()
void swap(array<T, N>& a, array<U, N>& b)
  noexcept(noexcept(adl_swap(a.elements_, b.elements_))) {
  adl_swap(a.elements_, b.elements_);
}

template <class T, class U, std::size_t N>
  requires stl2::Swappable<T&, U&>()
void swap(array<T, N>& a, U (&b)[N])
  noexcept(noexcept(adl_swap(a.elements_, b))) {
  adl_swap(a.elements_, b);
}

template <class T, class U, std::size_t N>
  requires stl2::Swappable<T&, U&>()
void swap(T (&b)[N], array<U, N>& a)
  noexcept(noexcept(adl_swap(a.elements_, b))) {
  adl_swap(a.elements_, b);
}

#endif

int main() {
  using stl2::ext::models::swappable;

  {
    int a[2][2] = {{0, 1}, {2, 3}};
    int b[2][2] = {{4, 5}, {6, 7}};

    static_assert(swappable<decltype((a)),decltype((b))>(), "");
    adl_swap(a, b);
    static_assert(noexcept(adl_swap(a, b)), "");

    CHECK(a[0][0] == 4);
    CHECK(a[0][1] == 5);
    CHECK(a[1][0] == 6);
    CHECK(a[1][1] == 7);

    CHECK(b[0][0] == 0);
    CHECK(b[0][1] == 1);
    CHECK(b[1][0] == 2);
    CHECK(b[1][1] == 3);
  }

#if 0
  {
    array<int, 4> a = {0,1,2,3};
    int b[4] = {4,5,6,7};

    static_assert(swappable<decltype(a[0]),decltype(b[0])>(), "");
    adl_swap(a[0], b[0]);
    static_assert(noexcept(adl_swap(a[0], b[0])), "");

    static_assert(swappable<decltype((a)),decltype((b))>(), "");
    adl_swap(a, b);
    static_assert(noexcept(adl_swap(a, b)), "");

    CHECK(a[0] == 0);
    CHECK(a[1] == 5);
    CHECK(a[2] == 6);
    CHECK(a[3] == 7);

    CHECK(b[0] == 4);
    CHECK(b[1] == 1);
    CHECK(b[2] == 2);
    CHECK(b[3] == 3);
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

    CHECK(a[0][0] == 0);
    CHECK(a[0][1] == 1);
    CHECK(a[1][0] == 8);
    CHECK(a[1][1] == 9);
    CHECK(a[2][0] == 10);
    CHECK(a[2][1] == 11);

    CHECK(b[0][0] == 6);
    CHECK(b[0][1] == 7);
    CHECK(b[1][0] == 2);
    CHECK(b[1][1] == 3);
    CHECK(b[2][0] == 4);
    CHECK(b[2][1] == 5);
  }
#endif
}
