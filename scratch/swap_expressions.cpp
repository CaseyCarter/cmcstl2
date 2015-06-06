#include <cassert>
#include <tuple>
#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

//#define SWAPPABLE_POINTERS

template <class T, class U>
concept bool Same = std::is_same<T, U>::value;

template <class T, class U>
concept bool ExplicitlyConvertible =
  Same<T, U> ||
  requires(T&& t) {
    static_cast<U>(std::forward<T>(t));
  };

template <class T, class U>
concept bool Convertible =
  Same<T, U> ||
  (ExplicitlyConvertible<T, U> &&
    std::is_convertible<T, U>::value);

template <class T>
concept bool Destructible =
  requires(T& t) {
    { t.~T() } noexcept;
  };

template <class T>
concept bool Copyable =
  Destructible<T> &&
  requires(T& t, T& b, const T& c) {
    T(c);
    T(std::move(c));
    T(b);
    T(std::move(b));
    t = b;
    t = std::move(b);
    t = c;
    t = std::move(c);
  };

template <class T>
struct reference_value_type {
  using type = T;
};

template <class T>
  requires requires { typename T::reference_value_type; }
struct reference_value_type<T> {
  using type = typename T::reference_value_type;
};

template <class T>
struct reference_value_type<T&> {
  using type = T;
};

template <class T>
struct reference_value_type<T&&> {
  using type = T;
};

template <class...Ts>
struct reference_value_type<std::tuple<Ts...>> {
  using type = std::tuple<std::remove_reference_t<Ts>...>;
};

template <class First, class Second>
struct reference_value_type<std::pair<First, Second>> {
  using type = std::pair<std::remove_reference_t<First>,
                         std::remove_reference_t<Second>>;
};

template <class R>
using ReferenceValueType =
  std::remove_cv_t<std::remove_reference_t<meta::eval<reference_value_type<R>>>>;

template <class R>
concept bool ReadableReference =
  requires {
    typename ReferenceValueType<R>;
    requires !std::is_void<ReferenceValueType<R>>::value;
    // requires !Same<R, ReferenceValueType<R>>;
    // requires std::is_object<ReferenceValueType<R>>::value;
    requires Convertible<R, ReferenceValueType<R>>;
  };

// Axiom?: Convertible<R, T> iff T == R || T == ReferenceType<R>

template <class W, class R>
concept bool WritableReference =
  ReadableReference<R> &&
  requires(W&& w, R&& r) {
    std::forward<W>(w) = std::forward<R>(r);
  };

// Axiom: if remove_cv_t<remove_reference_t<R>> != W, then after
//        w = r, w stores the value previously stored by r

template <class R>
concept bool MutableReference =
  ReadableReference<R> &&
  WritableReference<R, ReferenceValueType<R>>;

// Axiom: After r1 = t, r1 holds the value

template <class R>
concept bool RebindableReference =
  Copyable<R>;

// Axiom: After r1 = r2, r1 and r2 denote the same stored value

template <class T, class U>
concept bool Compatible = Same<T, U>; // FIXME

template <class T, class U>
concept bool SwappableReference =
  MutableReference<T> && MutableReference<U> &&
  Compatible<ReferenceValueType<T>, ReferenceValueType<U>>;

SwappableReference{T, U}
void swap(T&& t, U&& u)
  requires Destructible<ReferenceValueType<T>> {
  using V = ReferenceValueType<T>;
  V tmp(std::move(t));
  t = static_cast<V>(std::move(u));
  u = std::move(tmp);
}

template <class I>
using ReferenceType = decltype(*std::declval<I>());

template <class I>
using ValueType = ReferenceValueType<ReferenceType<I>>;

template <class I>
concept bool Readable =
  // Semiregular<I> &&
  requires(I& i) {
    typename ReferenceType<I>;
    // { *i } -> Same<ReferenceType<I>;
    *i; requires Same<ReferenceType<I>, decltype(*i)>;
    requires ReadableReference<ReferenceType<I>>;
  };

template <class O, class T>
concept bool Writable =
  // Semiregular<O> &&
  requires(O& o) {
    typename ReferenceType<O>;
    // { *o } -> Same<ReferenceType<O>;
    *o; requires Same<ReferenceType<O>, decltype(*o)>;
    requires WritableReference<ReferenceType<O>, T>;
  };

template <class I>
concept bool Mutable =
  Readable<I> && Writable<I, ValueType<I>>;

template <class T, class U>
concept bool IndirectSwappable =
  Mutable<T> && Mutable<U> &&
  SwappableReference<ReferenceType<T>, ReferenceType<U>>;

#ifdef SWAPPABLE_POINTERS
IndirectSwappable{T, U}
void swap(T&& t, U&& u)
  requires(!std::is_reference<T>::value &&
           !std::is_reference<U>::value)
{ swap(*t, *u); }

template <Mutable T, MutableReference U>
  requires(Compatible<ValueType<T>, ReferenceValueType<U>> &&
           !std::is_reference<T>::value)
void swap(T&& t, U&& u) {
  swap(*t, u);
}

template <MutableReference T, Mutable U>
  requires(Compatible<ReferenceValueType<T>, ValueType<U>> &&
           !std::is_reference<U>::value)
void swap(T&& t, U&& u) {
  swap(t, *u);
}
#endif // SWAPPABLE_POINTERS

IndirectSwappable{T, U}
void iter_swap(T t, U u) {
  swap(*t, *u);
}

template <class T>
struct reference_wrapper {
  T* ptr_;

  using reference_value_type = T;

  reference_wrapper() = default;
  reference_wrapper(T& t) : ptr_{std::addressof(t)} {}
  reference_wrapper(T&&) = delete;

  T& get() const { assert(ptr_); return *ptr_; }

  reference_wrapper& operator=(const T& t) {
    get() = t;
    return *this;
  }

  reference_wrapper& operator=(T&& t) {
    get() = std::move(t);
    return *this;
  }

  operator T&() const { return get(); }
};

template <class T, std::size_t N>
struct array {
  T e_[N];

  using reference = reference_wrapper<T>;

  reference operator[](std::size_t n) {
    return {e_[n]};
  }

  struct iterator {
    using iterator_category = std::random_access_iterator_tag;
    using difference_t = std::ptrdiff_t;

    T* ptr_;

    iterator() = default;
    iterator(T* p) : ptr_{p} {}

    reference operator*() const {
      return {*ptr_};
    }

    T* operator->() const
      requires(std::is_class<T>::value || std::is_union<T>::value)
    { return ptr_; }

    iterator& operator++() {
      ++ptr_;
      return *this;
    }

    iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    bool operator==(const iterator& other) const {
      return ptr_ == other.ptr_;
    }

    bool operator!=(const iterator& other) const {
      return !(*this == other);
    }

    iterator operator+(std::ptrdiff_t n) const {
      return {ptr_ + n};
    }
  };

  iterator begin() { return {&e_[0]}; }
  iterator end() { return {&e_[0] + N}; }
};

template <class T>
  requires std::is_integral<T>::value
struct irange {
  T first_;
  T last_;

  irange() = default;
  irange(T first, T last) : first_{first}, last_{last} {}

  struct iterator {
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::make_unsigned_t<T>;

    T current_;

    iterator() = default;
    iterator(T t) : current_{t} {}

    T operator*() const { return current_; }

    bool operator==(const iterator& other) const {
      return current_ == other.current_;
    }
    bool operator!=(const iterator& other) const {
      return !(*this == other);
    }

    iterator& operator++() {
      ++current_;
      return *this;
    }

    iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    iterator operator+(T t) const {
      return {current_ + t};
    }

    // ...
  };

  iterator begin() const { return {first_}; }
  iterator end() const { return {last_}; }
};

static_assert(std::is_same<ReferenceValueType<const int&>,int>(), "");
static_assert(std::is_same<ValueType<const int*>,int>(), "");
static_assert(std::is_same<ReferenceValueType<array<int, 4>::reference>,int>(), "");
static_assert(std::is_same<ValueType<array<int, 4>::iterator>,int>(), "");
static_assert(std::is_same<ValueType<irange<int>::iterator>,int>(), "");

template <class>
constexpr bool is_readable_reference() { return false; }
template <ReadableReference>
constexpr bool is_readable_reference() { return true; }

static_assert(is_readable_reference<int>(), "");
static_assert(is_readable_reference<int&>(), "");
static_assert(is_readable_reference<const int&>(), "");
static_assert(is_readable_reference<int&&>(), "");
static_assert(is_readable_reference<const int&&>(), "");
static_assert(is_readable_reference<reference_wrapper<int>>(), "");

template <class, class>
constexpr bool is_writable_reference() { return false; }
template <class R, class T>
requires WritableReference<R, T>
constexpr bool is_writable_reference() { return true; }

static_assert(!is_writable_reference<int, int>(), "");
static_assert(is_writable_reference<int&, int>(), "");
static_assert(!is_writable_reference<const int&, int>(), "");
static_assert(!is_writable_reference<int&&, int>(), "");
static_assert(!is_writable_reference<const int&&, int>(), "");
static_assert(is_writable_reference<reference_wrapper<int>, int>(), "");

template <class>
constexpr bool is_mutable_reference() { return false; }
template <MutableReference>
constexpr bool is_mutable_reference() { return true; }

static_assert(!is_mutable_reference<int>(), "");
static_assert(is_mutable_reference<int&>(), "");
static_assert(!is_mutable_reference<const int&>(), "");
static_assert(!is_mutable_reference<int&&>(), "");
static_assert(!is_mutable_reference<const int&&>(), "");
static_assert(is_mutable_reference<reference_wrapper<int>>(), "");

template <class>
constexpr bool is_readable() { return false; }
template <Readable>
constexpr bool is_readable() { return true; }

static_assert(is_readable<int*>(), "");
static_assert(is_readable<const int*>(), "");
static_assert(is_readable<irange<int>::iterator>(), "");

template <class>
constexpr bool is_mutable() { return false; }
template <Mutable>
constexpr bool is_mutable() { return true; }

static_assert(is_mutable<int*>(), "");
static_assert(!is_mutable<const int*>(), "");
static_assert(!is_mutable<irange<int>::iterator>(), "");

int main() {
  {
    int a = 42, b = 13;
    auto pa = &a, pb = &b;
    swap(pa, pb);
    assert(pa == &b && pb == &a && a == 42 && b == 13);
  }

  {
    int a = 42, b = 13;

    swap(a, b);
#ifdef SWAPPABLE_POINTERS
    swap(&a, b);
    swap(a, &b);
    swap(&a, &b);
#endif // SWAPPABLE_POINTERS
    iter_swap(&a, &b);
  }

  {
    array<int, 4> a{};
    int i = 42;

    for (auto j = 0; j < 4; ++j) {
      swap(i, a[j]);
    }

    {
      a[0] = 42;
      a[1] = 13;

      // Swapping rvalue smart references swaps the referents
      swap(a[0], a[1]);
      assert(a[0] == 13 && a[1] == 42);

      // Swapping lvalue smart references swaps the references
      auto&& a0 = a[0];
      auto&& a1 = a[1];
      swap(a0, a1);
      assert(a0 == 42 && a1 == 13);
      assert(a[0] == 13 && a[1] == 42);
    }

    {
      a[0] = 42;
      a[1] = 13;
      
      auto p = a.begin();
      auto q = p;
      ++q;

      swap(*p, *q);
      assert(a[0] == 13 && a[1] == 42);
      iter_swap(p, q);
      assert(a[0] == 42 && a[1] == 13);
      iter_swap(a.begin(), a.begin() + 1);
      assert(a[0] == 13 && a[1] == 42);
      swap(*p, a[1]);
      assert(a[0] == 42 && a[1] == 13);
    }
  }
}
