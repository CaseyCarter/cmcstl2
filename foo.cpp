#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <stl2/concept/all.hpp>
#include <stl2/utility.hpp>

namespace stl2 { inline namespace v1 {

////////////////////
// Iterator concepts
//

template <class T>
using ReferenceType =
  decltype(*declval<T>());

namespace detail {
template <class T>
using uncvref =
  std::remove_cv_t<std::remove_reference_t<T>>;

template <class T>
concept bool Void =
  std::is_void<T>::value;

namespace impl {
template <class T>
struct nonvoid { using type = T; };

template <Void T>
struct nonvoid<T> {};
} // namespace impl

template <class T>
using nonvoid = meta::eval<impl::nonvoid<T>>;

template <class T>
using nvuncvref = nonvoid<uncvref<T>>;

template <class T>
concept bool HasValueType =
  requires { typename T::value_type; };

template <class T>
concept bool HasElementType =
  requires { typename T::element_type; };

template <class T>
concept bool HasReferenceType =
  requires { typename ReferenceType<T>; };
} // namespace detail

template <class>
struct value_type {};

template <detail::HasValueType T>
struct value_type<T> { using type = typename T::value_type; };

template <detail::HasElementType T>
  requires !detail::HasValueType<T>
struct value_type<T> { using type = typename T::element_type; };

template <detail::HasReferenceType T>
  requires !detail::HasElementType<T> && !detail::HasValueType<T>
struct value_type<T> { using type = ReferenceType<T>; };

template <class T>
using ValueType =
  detail::nvuncvref<meta::eval<value_type<T>>>;


template <class I>
concept bool Readable =
  Semiregular<I> &&
  requires(I& i) {
    typename ValueType<I>;
    { *i } -> const ValueType<I>&;
  };

template <class Out, class T>
concept bool Writable =
  Semiregular<Out> &&
  requires(Out& o, T&& t) {
    *o = (T&&)t;
  };

template <class I, class Out>
concept bool IndirectlyAssignable =
  Readable<I> &&
  Writable<Out, ReferenceType<I>>;

template <class I1, class I2 = I1>
concept bool IndirectlySwappable =
  Readable<I1> &&
  Readable<I2> &&
  Swappable<ReferenceType<I1>, ReferenceType<I2>>;


namespace detail {
template <class T>
concept bool HasDifferenceType =
  requires { typename T::difference_type; };
} // namespace detail

template <class> struct difference_type {};

template <detail::HasDifferenceType T>
struct difference_type<T> { using type = typename T::difference_type; };

template <class T>
  requires !detail::HasDifferenceType<T> &&
    requires(T& a, T& b) {
      a - b; !detail::Void<decltype(a - b)>;
    }
struct difference_type<T> :
  std::remove_cv<decltype(declval<T>() - declval<T>())> {};

template <>
struct difference_type<std::nullptr_t> {
  using type = std::ptrdiff_t;
};

template <class T>
using DifferenceType =
  detail::nvuncvref<meta::eval<difference_type<T>>>;


namespace detail {
template <class T>
concept bool IntegralDifference =
  requires {
    typename DifferenceType<T>;
    requires Integral<DifferenceType<T>>;
  };
} // namespace detail

template <class> struct distance_type {};
template <detail::IntegralDifference T>
struct distance_type<T> :
  std::make_unsigned<DifferenceType<T>> {};

template <class T>
using DistanceType =
  meta::eval<distance_type<T>>;

template <class I>
concept bool WeaklyIncrementable =
  Semiregular<I> &&
  requires(I& i) {
    typename DistanceType<I>;
    requires Integral<DistanceType<I>>; // Try without this?
    ++i; requires Same<I&, decltype(++i)>;
    i++;
  };

template <class I>
concept bool Incrementable =
  WeaklyIncrementable<I> &&
  EqualityComparable<I> &&
  requires(I& i) {
    i++; requires Same<I, decltype(i++)>;
  };

struct weak_input_iterator_tag {};
struct input_iterator_tag :
  weak_input_iterator_tag {};
struct forward_iterator_tag :
  input_iterator_tag {};
struct bidirectional_iterator_tag :
  forward_iterator_tag {};
struct random_access_iterator_tag :
  bidirectional_iterator_tag {};
struct contiguous_iterator_tag :
  random_access_iterator_tag {};

template <class>
struct iterator_category {};
template <class T>
struct iterator_category<T*> {
  using type = contiguous_iterator_tag;
};
template <class T>
  requires requires { typename T::iterator_category; }
struct iterator_category<T> {
  using type = typename T::iterator_category;
};

template <class T>
using IteratorCategory =
  meta::eval<iterator_category<T>>;

template <class I>
concept bool WeakIterator =
  WeaklyIncrementable<I> &&
  requires(I& i) {
    //{ *i } -> auto&&;
    *i; requires !detail::Void<decltype(*i)>;
  };

template <class I>
concept bool Iterator =
  WeakIterator<I> &&
  EqualityComparable<I>;

template <class S, class I>
concept bool Sentinel =
  Iterator<I> &&
  Regular<S> &&
  EqualityComparable<I, S>;

template <class I>
concept bool WeakInputIterator =
  WeakIterator<I> &&
  Readable<I> &&
  requires(I i) {
    typename IteratorCategory<I>;
    Derived<IteratorCategory<I>, weak_input_iterator_tag>;
    //{ i++ } -> Readable;
    requires Readable<decltype(i++)>;
    requires Same<ValueType<I>,ValueType<decltype(i++)>>;
  };

template <class I>
concept bool InputIterator =
  WeakInputIterator<I> &&
  Iterator<I> &&
  Derived<IteratorCategory<I>, input_iterator_tag>;

template <class I, class T>
concept bool WeakOutputIterator =
  WeakIterator<I> &&
  Writable<I, T>;

template <class I, class T>
concept bool OutputIterator =
  WeakOutputIterator<I, T> &&
  Iterator<I>;

template <class I>
concept bool ForwardIterator =
  InputIterator<I> &&
  Incrementable<I> &&
  Derived<IteratorCategory<I>, forward_iterator_tag>;

template <class I>
concept bool BidirectionalIterator =
  ForwardIterator<I> &&
  Derived<IteratorCategory<I>, bidirectional_iterator_tag> &&
  requires(I i) {
    --i; requires Same<I&, decltype(--i)>;
    i--; requires Same<I, decltype(i--)>;
  };

template <class I, class S = I>
concept bool SizedIteratorRange =
  Sentinel<S, I> &&
  requires(I i, S s) {
    typename DifferenceType<I>;
    // Common<DifferenceType<I>, DistanceType<I>> ??
    { i - i } -> DifferenceType<I>;
    { s - s } -> DifferenceType<I>;
    { s - i } -> DifferenceType<I>;
    { i - s } -> DifferenceType<I>;
  };

template <class I>
concept bool RandomAccessIterator =
  BidirectionalIterator<I> &&
  Derived<IteratorCategory<I>, random_access_iterator_tag> &&
  TotallyOrdered<I> &&
  SizedIteratorRange<I> &&
  requires(I i, I j, DifferenceType<I> n) {
    i += n; requires Same<I&, decltype(i += n)>;
    i + n; requires Same<I, decltype(i + n)>;
    n + i; requires Same<I, decltype(n + i)>;
    i -= n; requires Same<I&, decltype(i -= n)>;
    i - n; requires Same<I, decltype(i - n)>;
    i[n]; requires Same<ReferenceType<I>,decltype(i[n])>;
  };

template <class I>
concept bool ContiguousIterator =
  RandomAccessIterator<I> &&
  Derived<IteratorCategory<I>, contiguous_iterator_tag> &&
  std::is_reference<ReferenceType<I>>::value;


/////////////////
// Range concepts

namespace detail {
using std::begin;
auto adl_begin(auto&& t) ->
  decltype(begin((decltype(t)&&)t)) {
    return begin((decltype(t)&&)t);
}

using std::end;
auto adl_end(auto&& t) ->
  decltype(end((decltype(t)&&)t)) {
    return end((decltype(t)&&)t);
}
} // namespace detail

template <class T>
using IteratorType = decltype(detail::adl_begin(declval<T>()));
template <class T>
using SentinelType = decltype(detail::adl_end(declval<T>()));

template <class T>
concept bool Range =
  requires(T&& t) {
    detail::adl_begin(t);
    detail::adl_end(t);
    typename IteratorType<T>;
    typename SentinelType<T>;
    requires Sentinel<SentinelType<T>, IteratorType<T>>;
  };

////////////
// Test code
//

namespace detail {
static_assert(same_extents<int, double>(), "");
static_assert(same_extents<int[3], double[3]>(), "");
static_assert(!same_extents<int[3], double[2]>(), "");
static_assert(same_extents<int[2][5][3], double[2][5][3]>(), "");
static_assert(!same_extents<int[2][5][3], double[2][5]>(), "");
static_assert(!same_extents<int[3], int>(), "");
static_assert(!same_extents<int[][2][1], double[][2][1]>(), "");
}

namespace concept_test {
template <class, class>
constexpr bool is_same() { return false; }
template <class T, Same<T> >
constexpr bool is_same() { return true; }

static_assert(is_same<int, int>(), "");
static_assert(is_same<double, double>(), "");
static_assert(!is_same<double, int>(), "");
static_assert(!is_same<int, double>(), "");

template <class, class>
constexpr bool is_a() { return false; }
template <class T, class U>
  requires PubliclyDerived<T, U>
constexpr bool is_a() { return true; }

namespace is_a_test {
struct A {};
struct B : A {};
struct C : A {};
struct D : B, C {};

static_assert(is_a<int,int>(), "");
static_assert(is_a<A,A>(), "");
static_assert(is_a<B,B>(), "");
static_assert(is_a<C,C>(), "");
static_assert(is_a<D,D>(), "");
static_assert(is_a<B,A>(), "");
static_assert(is_a<C,A>(), "");
static_assert(!is_a<A,B>(), "");
static_assert(!is_a<A,C>(), "");
static_assert(!is_a<A,D>(), "");
static_assert(!is_a<D,A>(), "");
static_assert(!is_a<int,void>(), "");
}

template <class, class>
constexpr bool is_convertible() { return false; }
template <class T, class U>
  requires Convertible<T, U>
constexpr bool is_convertible() { return true; }

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
}}}}

namespace std {
template <>
struct common_type<::stl2::concept_test::common::A, ::stl2::concept_test::common::A> {
  using type = void;
};
}

namespace stl2 { namespace v1 { namespace concept_test { namespace common {
static_assert(is_same<CommonType<int, int>, int>(), "");
static_assert(is_same<CommonType<A, A>, A>(), "");
}

template <class, class>
constexpr bool is_common() { return false; }
template <class T, Common<T> >
constexpr bool is_common() { return true; }

static_assert(is_common<int, int>(), "");
static_assert(is_common<int, double>(), "");
static_assert(is_common<double, int>(), "");
static_assert(is_common<double, double>(), "");
static_assert(!is_common<void, int>(), "");
static_assert(!is_common<int*, int>(), "");
static_assert(is_common<void*, int*>(), "");
static_assert(is_common<double,long long>(), "");

template <class>
constexpr bool is_boolean() { return false; }
template <Boolean>
constexpr bool is_boolean() { return true; }

namespace boolean {
static_assert(is_boolean<bool>(), "");
static_assert(is_boolean<int>(), "");
static_assert(is_boolean<void*>(), "");
struct A {};
static_assert(!is_boolean<A>(), "");
}

template <class>
constexpr bool is_integral() { return false; }
template <Integral>
constexpr bool is_integral() { return true; }

static_assert(is_integral<int>(), "");
static_assert(!is_integral<double>(), "");
static_assert(is_integral<unsigned>(), "");
static_assert(!is_integral<void>(), "");

template <class, class...>
constexpr bool is_constructible() { return false; }
Constructible{T, ...Args}
constexpr bool is_constructible() { return false; }

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

template <class>
constexpr bool is_destructible() { return false; }
template <Destructible>
constexpr bool is_destructible() { return true; }

static_assert(!is_destructible<void>(), "");
static_assert(is_destructible<int>(), "");
static_assert(is_destructible<int&>(), "");
static_assert(is_destructible<int[4]>(), "");
static_assert(!is_destructible<int()>(), "");

template <class>
constexpr bool is_swappable() { return false; }
template <class T>
  requires Swappable<T>
constexpr bool is_swappable() { return true; }
template <class, class>
constexpr bool is_swappable() { return false; }
template <class T, Swappable<T> U>
constexpr bool is_swappable() { return true; }

namespace swappable {
static_assert(is_swappable<int&>(), "");
static_assert(is_swappable<int(&)[4]>(), "");
static_assert(is_swappable<int(&)[3][4], int(&)[3][4]>(), "");
static_assert(is_swappable<int(&)[3][4][1][2], int(&)[3][4][1][2]>(), "");
static_assert(!is_swappable<int, int>(), "");
static_assert(!is_swappable<int&, double&>(), "");
static_assert(!is_swappable<int(&)[4], bool(&)[4]>(), "");
static_assert(!is_swappable<int(&)[3][4], int(&)[4][3]>(), "");

static_assert(noexcept(swap(declval<int&>(), declval<int&>())), "");

struct A {
  A() = default;
  A(A&&) = delete;
  A& operator=(A&&) = delete;
  friend void swap(A&, A&) noexcept {}
};
static_assert(is_swappable<A&>(), "");
static_assert(noexcept(swap(declval<A&>(), declval<A&>())), "");

struct B {
  friend void swap(A&, B&) noexcept {}
  friend void swap(B&, A&) noexcept {}
};
static_assert(is_swappable<B&>(), "");
static_assert(is_swappable<A&, B&>(), "");
static_assert(is_swappable<B(&)[1], A(&)[1]>(), "");
static_assert(is_swappable<B(&)[1][3], A(&)[1][3]>(), "");
static_assert(!is_swappable<B(&)[3][1], A(&)[1][3]>(), "");

#ifdef SWAPPABLE_POINTERS
static_assert(is_swappable<int*,int&>(), "");
static_assert(noexcept(::swap(declval<int*>(), declval<int&>())), "");
static_assert(!is_swappable<int*&,int&>(), "");
static_assert(is_swappable<A*,B*>(), "");
static_assert(is_swappable<A(*)[4], B(&)[4]>(), "");
static_assert(noexcept(::swap(declval<A(*)[4]>(), declval<B(&)[4]>())), "");
#endif
}


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


template <class>
constexpr bool is_copy_constructible() { return false; }
template <CopyConstructible>
constexpr bool is_copy_constructible() { return true; }

static_assert(!is_copy_constructible<void>(), "");
static_assert(is_copy_constructible<int>(), "");
static_assert(!is_copy_constructible<int[4]>(), "");
static_assert(is_copy_constructible<int&>(), "");
static_assert(!is_copy_constructible<void()>(), "");


template <class>
constexpr bool is_movable() { return false; }
template <Movable>
constexpr bool is_movable() { return true; }

static_assert(is_movable<int>(), "");
static_assert(is_movable<double>(), "");
static_assert(!is_movable<void>(), "");
static_assert(is_movable<copyable>(), "");
static_assert(is_movable<moveonly>(), "");
static_assert(!is_movable<nonmovable>(), "");
static_assert(!is_movable<copyonly>(), "");


template <class>
constexpr bool is_copyable() { return false; }
template <Copyable>
constexpr bool is_copyable() { return true; }

static_assert(is_copyable<int>(), "");
static_assert(is_copyable<double>(), "");
static_assert(!is_copyable<void>(), "");
static_assert(is_copyable<copyable>(), "");
static_assert(!is_copyable<moveonly>(), "");
static_assert(!is_copyable<nonmovable>(), "");
static_assert(!is_copyable<copyonly>(), "");


template <class>
constexpr bool is_semiregular() { return false; }
template <Semiregular>
constexpr bool is_semiregular() { return true; }

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

static_assert(is_same<int&, ReferenceType<int*>>(), "");
static_assert(is_same<int&, ReferenceType<int[]>>(), "");
static_assert(is_same<int&, ReferenceType<int[4]>>(), "");
static_assert(is_same<int&, ReferenceType<A>>(), "");
static_assert(is_same<int&, ReferenceType<B>>(), "");
static_assert(is_same<int&, ReferenceType<C>>(), "");
static_assert(is_same<int&, ReferenceType<D>>(), "");
static_assert(is_same<const int&, ReferenceType<const int*>>(), "");

static_assert(is_same<int, ValueType<int*>>(), "");
static_assert(is_same<int, ValueType<int[]>>(), "");
static_assert(is_same<int, ValueType<int[4]>>(), "");
static_assert(is_same<int, ValueType<A>>(), "");
static_assert(is_same<double, ValueType<B>>(), "");
static_assert(is_same<double, ValueType<C>>(), "");
static_assert(is_same<double, ValueType<D>>(), "");
static_assert(is_same<int, ValueType<const int*>>(), "");

static_assert(is_same<std::ptrdiff_t, DifferenceType<int*>>(), "");
static_assert(is_same<std::ptrdiff_t, DifferenceType<int[]>>(), "");
static_assert(is_same<std::ptrdiff_t, DifferenceType<int[4]>>(), "");
static_assert(is_same<std::ptrdiff_t, DifferenceType<std::nullptr_t>>(), "");
static_assert(is_same<std::make_unsigned_t<std::ptrdiff_t>, DistanceType<int*>>(), "");

static_assert(is_same<int, DifferenceType<int>>(), "");
static_assert(is_same<unsigned, DistanceType<int>>(), "");
}

template <class>
constexpr bool is_readable() { return false; }
template <Readable>
constexpr bool is_readable() { return true; }

static_assert(!is_readable<void>(), "");
static_assert(!is_readable<void*>(), "");
static_assert(is_readable<int*>(), "");
static_assert(is_readable<const int*>(), "");


template <class>
constexpr bool is_weakly_incrementable() { return false; }
template <WeaklyIncrementable>
constexpr bool is_weakly_incrementable() { return true; }

static_assert(is_weakly_incrementable<int>(), "");
static_assert(is_weakly_incrementable<unsigned int>(), "");
static_assert(!is_weakly_incrementable<void>(), "");
static_assert(is_weakly_incrementable<int*>(), "");
static_assert(is_weakly_incrementable<const int*>(), "");


template <class>
constexpr bool is_incrementable() { return false; }
template <Incrementable>
constexpr bool is_incrementable() { return true; }

#if 0
static_assert(is_incrementable<int>(), "");
static_assert(is_incrementable<unsigned int>(), "");
static_assert(!is_incrementable<void>(), "");
static_assert(is_incrementable<int*>(), "");
static_assert(is_incrementable<const int*>(), "");
#endif
}}} // namespace stl2::v1::concept_test

#include <iostream>

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
