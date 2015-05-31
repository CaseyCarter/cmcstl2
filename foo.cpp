// -*- compile-command: "(cd ~/cmcstl2/build && make && ./foo)" -*-

#include <stl2/concepts/all.hpp>
#include <stl2/utility.hpp>

#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <iostream>

////////////
// Test code
//
using namespace stl2::concepts;
using namespace stl2::concepts::test;

namespace same_extents_test {
using stl2::detail::same_extents;

static_assert(same_extents<int, double>(), "");
static_assert(same_extents<int[3], double[3]>(), "");
static_assert(!same_extents<int[3], double[2]>(), "");
static_assert(same_extents<int[2][5][3], double[2][5][3]>(), "");
static_assert(!same_extents<int[2][5][3], double[2][5]>(), "");
static_assert(!same_extents<int[3], int>(), "");
}

static_assert(is_same<int, int>(), "");
static_assert(is_same<double, double>(), "");
static_assert(!is_same<double, int>(), "");
static_assert(!is_same<int, double>(), "");

namespace is_publicly_derived_test {
struct A {};
struct B : A {};
struct C : A {};
struct D : B, C {};

static_assert(is_publicly_derived<int,int>(), "");
static_assert(is_publicly_derived<A,A>(), "");
static_assert(is_publicly_derived<B,B>(), "");
static_assert(is_publicly_derived<C,C>(), "");
static_assert(is_publicly_derived<D,D>(), "");
static_assert(is_publicly_derived<B,A>(), "");
static_assert(is_publicly_derived<C,A>(), "");
static_assert(!is_publicly_derived<A,B>(), "");
static_assert(!is_publicly_derived<A,C>(), "");
static_assert(!is_publicly_derived<A,D>(), "");
static_assert(!is_publicly_derived<D,A>(), "");
static_assert(!is_publicly_derived<int,void>(), "");
}

namespace convertible {
struct A {};
struct B : A {};

static_assert(is_convertible<A, A>(), "");
static_assert(is_convertible<B, B>(), "");
static_assert(!is_convertible<A, B>(), "");
static_assert(is_convertible<B, A>(), "");
static_assert(is_convertible<int, double>(), "");
static_assert(is_convertible<double, int>(), "");
}

namespace common {
struct A {};
}

namespace std {
template <>
struct common_type<::common::A, ::common::A> {
  using type = void;
};
}

namespace common {
static_assert(is_same<CommonType<int, int>, int>(), "");
static_assert(is_same<CommonType<A, A>, A>(), "");
}

static_assert(is_common<int, int>(), "");
static_assert(is_common<int, double>(), "");
static_assert(is_common<double, int>(), "");
static_assert(is_common<double, double>(), "");
static_assert(!is_common<void, int>(), "");
static_assert(!is_common<int*, int>(), "");
static_assert(is_common<void*, int*>(), "");
static_assert(is_common<double,long long>(), "");

namespace boolean {
static_assert(is_boolean<bool>(), "");
static_assert(is_boolean<int>(), "");
static_assert(is_boolean<void*>(), "");
struct A {};
static_assert(!is_boolean<A>(), "");
}

static_assert(is_integral<int>(), "");
static_assert(!is_integral<double>(), "");
static_assert(is_integral<unsigned>(), "");
static_assert(!is_integral<void>(), "");

namespace constructible {
template <class T, class U>
  requires Constructible<T, U>
constexpr bool f() { return false; }
template <class T, Convertible<T> >
constexpr bool f() { return true; }

static_assert(f<int,int>(), "");
static_assert(f<int,long>(), "");

struct A {
  A(int);
};
struct B {
  explicit B(int);
};
static_assert(f<A, int>(), "");
static_assert(!f<B, int>(), "");
}

static_assert(!is_destructible<void>(), "");
static_assert(is_destructible<int>(), "");
static_assert(is_destructible<int&>(), "");
static_assert(is_destructible<int[4]>(), "");
static_assert(!is_destructible<int()>(), "");

namespace swappable {
using stl2::swap;

static_assert(is_swappable<int&>(), "");
static_assert(is_swappable<int(&)[4]>(), "");
static_assert(is_swappable<int(&)[3][4], int(&)[3][4]>(), "");
static_assert(is_swappable<int(&)[3][4][1][2], int(&)[3][4][1][2]>(), "");
static_assert(!is_swappable<int, int>(), "");
static_assert(!is_swappable<int&, double&>(), "");
static_assert(!is_swappable<int(&)[4], bool(&)[4]>(), "");
static_assert(!is_swappable<int(&)[], int(&)[]>(), "");
static_assert(!is_swappable<int(&)[][4], int(&)[][4]>(), "");

static_assert(noexcept(swap(stl2::declval<int&>(),
                            stl2::declval<int&>())), "");

struct A {
  A() = default;
  A(A&&) = delete;
  A& operator=(A&&) = delete;
  friend void swap(A&, A&) noexcept {}
};
static_assert(is_swappable<A&>(), "");
static_assert(noexcept(swap(stl2::declval<A&>(), stl2::declval<A&>())), "");

struct B {
  friend void swap(A&, B&) noexcept {}
  friend void swap(B&, A&) noexcept {}
};
static_assert(is_swappable<B&>(), "");
static_assert(is_swappable<A&, B&>(), "");
static_assert(is_swappable<B(&)[1], A(&)[1]>(), "");
static_assert(is_swappable<B(&)[1][3], A(&)[1][3]>(), "");
static_assert(!is_swappable<B(&)[3][1], A(&)[1][3]>(), "");
#if 1
}
#else

#ifdef STL2_SWAPPABLE_POINTERS
static_assert(is_swappable<int*,int&>(), "");
static_assert(noexcept(stl2::swap(stl2::declval<int*>(), stl2::declval<int&>())), "");
static_assert(!is_swappable<int*&,int&>(), "");
static_assert(is_swappable<A*,B*>(), "");
static_assert(is_swappable<A(*)[4], B(&)[4]>(), "");
static_assert(noexcept(swap(stl2::declval<A(*)[4]>(), stl2::declval<B(&)[4]>())), "");
#endif
}
#endif

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


static_assert(!is_copy_constructible<void>(), "");
static_assert(is_copy_constructible<int>(), "");
static_assert(!is_copy_constructible<int[4]>(), "");
static_assert(is_copy_constructible<int&>(), "");
static_assert(!is_copy_constructible<void()>(), "");


static_assert(is_movable<int>(), "");
static_assert(is_movable<double>(), "");
static_assert(!is_movable<void>(), "");
static_assert(is_movable<copyable>(), "");
static_assert(is_movable<moveonly>(), "");
static_assert(!is_movable<nonmovable>(), "");
static_assert(!is_movable<copyonly>(), "");


static_assert(is_copyable<int>(), "");
static_assert(is_copyable<double>(), "");
static_assert(!is_copyable<void>(), "");
static_assert(is_copyable<copyable>(), "");
static_assert(!is_copyable<moveonly>(), "");
static_assert(!is_copyable<nonmovable>(), "");
static_assert(!is_copyable<copyonly>(), "");


namespace semiregular {
struct A {};

static_assert(is_semiregular<int>(), "");
static_assert(is_semiregular<double>(), "");
static_assert(!is_semiregular<void>(), "");
static_assert(!is_semiregular<int&>(), "");
static_assert(is_semiregular<A>(), "");
}


namespace associated_types {
struct A { int& operator*(); };
struct B : A { using value_type = double; };
struct C : A { using element_type = double; };
struct D : A {
  using value_type = double;
  using element_type = char;
};

using stl2::ReferenceType;
static_assert(is_same<int&, ReferenceType<int*>>(), "");
static_assert(is_same<int&, ReferenceType<int[]>>(), "");
static_assert(is_same<int&, ReferenceType<int[4]>>(), "");
static_assert(is_same<int&, ReferenceType<A>>(), "");
static_assert(is_same<int&, ReferenceType<B>>(), "");
static_assert(is_same<int&, ReferenceType<C>>(), "");
static_assert(is_same<int&, ReferenceType<D>>(), "");
static_assert(is_same<const int&, ReferenceType<const int*>>(), "");

using stl2::ValueType;
static_assert(is_same<int, ValueType<int*>>(), "");
static_assert(is_same<int, ValueType<int[]>>(), "");
static_assert(is_same<int, ValueType<int[4]>>(), "");
static_assert(is_same<int, ValueType<A>>(), "");
static_assert(is_same<double, ValueType<B>>(), "");
static_assert(is_same<double, ValueType<C>>(), "");
static_assert(is_same<double, ValueType<D>>(), "");
static_assert(is_same<int, ValueType<const int*>>(), "");

using stl2::DifferenceType;
using stl2::DistanceType;
static_assert(is_same<std::ptrdiff_t, DifferenceType<int*>>(), "");
static_assert(is_same<std::ptrdiff_t, DifferenceType<int[]>>(), "");
static_assert(is_same<std::ptrdiff_t, DifferenceType<int[4]>>(), "");
static_assert(is_same<std::ptrdiff_t, DifferenceType<std::nullptr_t>>(), "");
static_assert(is_same<std::make_unsigned_t<std::ptrdiff_t>, DistanceType<int*>>(), "");

static_assert(is_same<int, DifferenceType<int>>(), "");
static_assert(is_same<unsigned, DistanceType<int>>(), "");
}

static_assert(!is_readable<void>(), "");
static_assert(!is_readable<void*>(), "");
static_assert(is_readable<int*>(), "");
static_assert(is_readable<const int*>(), "");


static_assert(is_weakly_incrementable<int>(), "");
static_assert(is_weakly_incrementable<unsigned int>(), "");
static_assert(!is_weakly_incrementable<void>(), "");
static_assert(is_weakly_incrementable<int*>(), "");
static_assert(is_weakly_incrementable<const int*>(), "");


#if 0
static_assert(is_incrementable<int>(), "");
static_assert(is_incrementable<unsigned int>(), "");
static_assert(!is_incrementable<void>(), "");
static_assert(is_incrementable<int*>(), "");
static_assert(is_incrementable<const int*>(), "");
#endif

namespace detail {
struct destroy_fn {
  template <stl2::Destructible T>
  void operator()(T& o) const noexcept {
    o.~T();
  }

  template <class T, std::size_t N>
    requires stl2::Destructible<std::remove_all_extents<T>>
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

namespace {
template <class T, std::size_t N>
struct array {
  T elements_[N];

  T& operator[](std::size_t i) {
    assert(i < N);
    return elements_[i];
  }
};

using stl2::swap;

template <class T, class U, std::size_t N>
  requires stl2::Swappable<T&, U&>
void swap(array<T, N>& a, array<U, N>& b)
  noexcept(noexcept(swap(a.elements_, b.elements_))) {
  swap(a.elements_, b.elements_);
}

template <class T, class U, std::size_t N>
  requires stl2::Swappable<T&, U&>
void swap(array<T, N>& a, U (&b)[N])
  noexcept(noexcept(swap(a.elements_, b))) {
  swap(a.elements_, b);
}

template <class T, class U, std::size_t N>
  requires stl2::Swappable<T&, U&>
void swap(T (&b)[N], array<U, N>& a)
  noexcept(noexcept(swap(b, a.elements_))) {
  swap(b, a.elements_);
}
} // unnamed namespace

namespace {
struct tag {};
struct A { A() = default; A(int) {} explicit A(tag) {} };
struct B : A {};
struct C : B {};

#if 1
void f(A) { std::cout << "exactly A\n"; }

void f(stl2::PubliclyDerived<A>) { std::cout << "Publicly derived from A\n"; }

void f(stl2::Convertible<A>) { std::cout << "Implicitly convertible to A\n"; }

void f(stl2::ExplicitlyConvertible<A>) { std::cout << "Explicitly convertible to A\n"; }

void f(auto) { std::cout << "Nothing to do with A\n"; }

#else

void f(A) {
  std::cout << "exactly A\n";
}

template <class T>
std::enable_if_t<std::is_base_of<A, T>::value &&
                 std::is_convertible<T,A>::value &&
                 !(std::is_same<A,T>::value)>
f(T) {
  std::cout << "Publicly derived from A\n";
}

template <class T>
std::enable_if_t<
  std::is_convertible<T,A>::value &&
  !(std::is_base_of<A,T>::value ||
    std::is_same<A,T>::value)>
f(T) {
  std::cout << "Implicitly convertible to A\n";
}

template <class T>
std::enable_if_t<
  std::is_constructible<A,T>::value &&
  !(std::is_convertible<T,A>::value ||
    std::is_base_of<A,T>::value ||
    std::is_same<A,T>::value)>
f(T) {
  std::cout << "Explicitly convertible to A\n";
}

template <class T>
std::enable_if_t<
  !(std::is_constructible<A,T>::value ||
    std::is_convertible<T,A>::value ||
    std::is_base_of<A,T>::value ||
    std::is_same<A,T>::value)>
f(T) {
  std::cout << "Nothing to do with A\n";
}
#endif

void relationship_test() {
  f(A{});
  { const A a{}; f(a); }
  f(B{});
  { const B b{}; f(b); }
  f(42);
  f(tag{});
  f("foo");
}
} // unnamed namespace

int main() {
  {
    int a[2][2] = {{0, 1}, {2, 3}};
    int b[2][2] = {{4, 5}, {6, 7}};

    using stl2::swap;
    static_assert(noexcept(swap(a, b)), "");
    swap(a, b);

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

    using stl2::swap;

    //static_assert(stl2::concept_test::is_swappable<decltype((a)),decltype((b))>(), "");
    swap(a, b);

    assert(a[0] == 4);
    assert(a[1] == 5);
    assert(a[2] == 6);
    assert(a[3] == 7);

    assert(b[0] == 0);
    assert(b[1] == 1);
    assert(b[2] == 2);
    assert(b[3] == 3);
  }
#if 0
  {
    array<array<int, 2>, 3> a = {{{{0, 1}}, {{2, 3}}, {{4, 5}}}};
    int b[3][2] = {{6, 7}, {8, 9}, {10, 11}};

    using stl2::swap;
    swap(a, b);
  }
#endif
#ifdef STL2_SWAPPABLE_POINTERS
  {
    struct A {
      void foo(A& other) {
        using stl2::swap;
        swap(this, other);
      }
    };

    A a, b;
    a.foo(b);
  }
#endif

  relationship_test();
}
