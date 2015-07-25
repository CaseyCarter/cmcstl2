#include "validate.hpp"

#if VALIDATE_RANGES
#include <utility>

namespace models {
template <class T, class U = T>
using swappable = ranges::Swappable<T, U>;
}

namespace ns {
using std::declval;
using std::forward;

using ranges::is_nothrow_swappable;
using ranges::swap;
}

#elif VALIDATE_STL2
#include <stl2/detail/concepts/object.hpp>
#include <stl2/utility.hpp>

namespace ns {
using stl2::declval;
using stl2::forward;

using stl2::swap;
using stl2::is_nothrow_swappable;
}
#endif

#include <cassert>
#include <cstddef>

#include "../simple_test.hpp"

namespace swappable_test {
using ns::swap;

CONCEPT_ASSERT(models::swappable<int&>());
CONCEPT_ASSERT(models::swappable<int&, int&>());
CONCEPT_ASSERT(models::swappable<int(&)[4]>());
CONCEPT_ASSERT(models::swappable<int(&)[4], int(&)[4]>());
CONCEPT_ASSERT(!models::swappable<int>());
CONCEPT_ASSERT(!models::swappable<int, int>());
CONCEPT_ASSERT(!models::swappable<int&, double&>());
CONCEPT_ASSERT(!models::swappable<int(&)[4], bool(&)[4]>());
CONCEPT_ASSERT(!models::swappable<int(&)[]>());
CONCEPT_ASSERT(!models::swappable<int(&)[][4]>());

CONCEPT_ASSERT(noexcept(swap(ns::declval<int&>(),
                             ns::declval<int&>())));
CONCEPT_ASSERT(ns::is_nothrow_swappable<int&, int&>());
CONCEPT_ASSERT(ns::is_nothrow_swappable<int(&)[42], int(&)[42]>());

// range-v3 doesn't support swapping multidimensional arrays
#if VALIDATE_STL2
CONCEPT_ASSERT(models::swappable<int(&)[3][4]>());
CONCEPT_ASSERT(models::swappable<int(&)[3][4], int(&)[3][4]>());
CONCEPT_ASSERT(models::swappable<int(&)[3][4][1][2]>());
CONCEPT_ASSERT(models::swappable<int(&)[3][4][1][2], int(&)[3][4][1][2]>());
CONCEPT_ASSERT(!models::swappable<int(&)[3][4][1][2], int(&)[4][4][1][2]>());
CONCEPT_ASSERT(ns::is_nothrow_swappable<int(&)[6][7], int(&)[6][7]>());
#endif

namespace {
struct A {
  A() = default;
  A(A&&) = delete;
  A& operator=(A&&) = delete;
  friend void swap(A&, A&) noexcept {}
};

CONCEPT_ASSERT(models::swappable<A&>());
CONCEPT_ASSERT(noexcept(swap(ns::declval<A&>(), ns::declval<A&>())));
CONCEPT_ASSERT(ns::is_nothrow_swappable<A&, A&>());
}

namespace {
struct B {
  friend void swap(B&, B&) {}
};

CONCEPT_ASSERT(models::swappable<B&>());
CONCEPT_ASSERT(!noexcept(swap(ns::declval<B&>(), ns::declval<B&>())));
CONCEPT_ASSERT(!ns::is_nothrow_swappable<B&, B&>());
}

} // namespace swappable_test

namespace adl_swap_detail {
using ns::swap;

#if VALIDATE_STL2
template <class T, class U>
  requires stl2::Swappable<T, U>()
#else
template <class T, class U,
  CONCEPT_REQUIRES_(ranges::Swappable<T, U>())>
#endif
constexpr void adl_swap(T&& t, U&& u)
  noexcept(noexcept(swap(ns::forward<T>(t), ns::forward<U>(u)))) {
  swap(ns::forward<T>(t), ns::forward<U>(u));
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
#if VALIDATE_STL2
  {
    int a[2][2] = {{0, 1}, {2, 3}};
    int b[2][2] = {{4, 5}, {6, 7}};

    CONCEPT_ASSERT(models::swappable<decltype((a)),decltype((b))>());
    adl_swap(a, b);
    CONCEPT_ASSERT(noexcept(adl_swap(a, b)));

    CHECK(a[0][0] == 4);
    CHECK(a[0][1] == 5);
    CHECK(a[1][0] == 6);
    CHECK(a[1][1] == 7);

    CHECK(b[0][0] == 0);
    CHECK(b[0][1] == 1);
    CHECK(b[1][0] == 2);
    CHECK(b[1][1] == 3);
  }
#endif

#if 0
  {
    array<int, 4> a = {0,1,2,3};
    int b[4] = {4,5,6,7};

    CONCEPT_ASSERT(models::swappable<decltype(a[0]),decltype(b[0])>());
    adl_swap(a[0], b[0]);
    CONCEPT_ASSERT(noexcept(adl_swap(a[0], b[0])));

    CONCEPT_ASSERT(models::swappable<decltype((a)),decltype((b))>());
    adl_swap(a, b);
    CONCEPT_ASSERT(noexcept(adl_swap(a, b)));

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

    CONCEPT_ASSERT(models::swappable<decltype(a[0]),decltype(b[0])>());
    adl_swap(a[0], b[0]);
    CONCEPT_ASSERT(noexcept(adl_swap(a[0], b[0])));

    CONCEPT_ASSERT(models::swappable<decltype((a)),decltype((b))>());
    adl_swap(a, b);
    CONCEPT_ASSERT(noexcept(adl_swap(a, b)));

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

  return ::test_result();
}
