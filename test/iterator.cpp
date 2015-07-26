// -*- compile-command: "(cd ~/cmcstl2/build && make iterator && ./test/iterator)" -*-

#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>

#include <stl2/iterator.hpp>

#include "simple_test.hpp"

template <class T>
struct reference_wrapper {
  T* ptr_;

  reference_wrapper() = default;
  reference_wrapper(T& t) noexcept : ptr_{std::addressof(t)} {}
  reference_wrapper(T&&) = delete;

  T& get() const noexcept {
    assert(ptr_);
    return *ptr_;
  }

  reference_wrapper& operator=(const T& t)
    noexcept(std::is_nothrow_copy_assignable<T>::value) {
    get() = t;
    return *this;
  }

  reference_wrapper& operator=(T&& t)
    noexcept(std::is_nothrow_move_assignable<T>::value) {
    get() = stl2::move(t);
    return *this;
  }

  operator T&() const noexcept { return get(); }
};

template <class T, std::size_t N>
struct array {
  T e_[N];

  using reference = reference_wrapper<T>;
  using const_reference = reference_wrapper<const T>;

  reference operator[](const std::size_t n) noexcept {
    return {e_[n]};
  }
  const_reference operator[](const std::size_t n) const noexcept {
    return {e_[n]};
  }

  struct iterator {
    using iterator_category = stl2::random_access_iterator_tag;
    using difference_t = std::ptrdiff_t;
    using value_type = T;

    T* ptr_;

    iterator() = default;
    iterator(T* p) noexcept : ptr_{p} {}

    reference operator*() const noexcept {
      assert(ptr_);
      return {*ptr_};
    }

    T* operator->() const noexcept
      requires std::is_class<T>::value || std::is_union<T>::value {
      assert(ptr_);
      return ptr_;
    }

    iterator& operator++() noexcept {
      ++ptr_;
      return *this;
    }

    iterator operator++(int) noexcept {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    bool operator==(const iterator& other) const noexcept {
      return ptr_ == other.ptr_;
    }

    bool operator!=(const iterator& other) const noexcept {
      return !(*this == other);
    }

    iterator operator+(std::ptrdiff_t n) const noexcept {
      return {ptr_ + n};
    }

    friend decltype(auto) iter_move(iterator i) noexcept {
      //std::cout << "iter_move(" << static_cast<void*>(i.ptr_) << ")\n";
      assert(i.ptr_);
      return static_cast<T&&>(*i.ptr_);
    }
  };

  iterator begin() { return {&e_[0]}; }
  iterator end() { return {&e_[0] + N}; }
};

template <stl2::InputIterator>
constexpr bool f() { return false; }
template <stl2::ext::ContiguousIterator>
constexpr bool f() { return true; }

template <stl2::InputIterator I, stl2::Sentinel<I> S, class O>
  requires stl2::IndirectlyCopyable<I, O>()
bool copy(I first, S last, O o) {
  while (first != last) {
    *o = *first;
    ++first;
    ++o;
  }
  return false;
}

template <stl2::ext::ContiguousIterator I, stl2::Sentinel<I> S,
          stl2::ext::ContiguousIterator O>
  requires stl2::IndirectlyCopyable<I, O>() &&
    stl2::SizedIteratorRange<I, S>() &&
    stl2::ext::ContiguousIterator<O>() &&
    stl2::Same<stl2::ValueType<I>, stl2::ValueType<O>>() &&
    std::is_trivially_copyable<stl2::ValueType<I>>::value
bool copy(I first, S last, O o) {
  auto n = last - first;
  if (n) {
    std::memmove(std::addressof(*o), std::addressof(*first),
                 n * sizeof(stl2::ValueType<I>));
  }
  return true;
}

int main() {
  namespace models = stl2::ext::models;

  {
    int a[] = { 42, 13 };
    stl2::iter_swap2(a + 0, a + 1);
    CHECK(a[0] == 13);
    CHECK(a[1] == 42);
    stl2::iter_swap2(a + 0, a + 1);
    CHECK(a[0] == 42);
    CHECK(a[1] == 13);
  }

  {
    auto a = std::make_unique<int>(42);
    auto b = std::make_unique<int>(13);
    using I = decltype(a);
    static_assert(models::same<I, decltype(b)>(), "");
    static_assert(models::readable<I>(), "");
    using R = stl2::ReferenceType<I>;
    static_assert(models::same<int&, R>(), "");
    using RR = stl2::RvalueReferenceType<I>;
    static_assert(models::same<int&&, RR>(), "");
    static_assert(models::swappable<R, R>(), "");

    // Swappable<R, R>() is true, calls the first overload of
    // iter_swap2 (which delegates to swap(*a, *b)):
    stl2::iter_swap2(a, b);
    CHECK(*a == 13);
    CHECK(*b == 42);
    stl2::iter_swap2(a, b);
    CHECK(*a == 42);
    CHECK(*b == 13);
  }

  {
    auto a = array<int, 4>{0,1,2,3};
    using I = decltype(a.begin());
    static_assert(models::readable<I>(), "");
    using V = stl2::ValueType<I>;
    static_assert(models::same<int, V>(), "");
    using R = stl2::ReferenceType<I>;
    static_assert(models::same<reference_wrapper<int>, R>(), "");
    using RR = stl2::RvalueReferenceType<I>;
    static_assert(models::same<int&&, RR>(), "");

    static_assert(models::same<I, decltype(a.begin() + 2)>(), "");
    static_assert(models::constructible<V, RR>(), "");
    static_assert(models::move_writable<I, V>(), "");
    static_assert(models::move_writable<I, RR>(), "");
    static_assert(!models::swappable<R, R>(), "");
    static_assert(stl2::ext::is_nothrow_indirectly_swappable_v<I, I>, "");

    // Swappable<R, R>() is false, and is_nothrow_indirectly_swappable<I, I>
    // implies IndirectlySwappable<I, I>, so this should resolve to the second
    // overload of iter_swap2.
    stl2::iter_swap2(a.begin() + 1, a.begin() + 3);
    CHECK(a[0] == 0);
    CHECK(a[1] == 3);
    CHECK(a[2] == 2);
    CHECK(a[3] == 1);
  }

  {
    struct A {
      int value;
      A(int i) : value{i} {}
      A(const A& that) :
        value{that.value} {}
    };
    A a[] = {0,1,2,3}, b[] = {4,5,6,7};
    CHECK(!copy(std::begin(a) + 1, std::end(a) - 1, std::begin(b) + 1));
    CHECK(b[0].value == 4);
    CHECK(b[1].value == 1);
    CHECK(b[2].value == 2);
    CHECK(b[3].value == 7);
  }

  {
    int a[] = {0,1,2,3,4,5,6,7}, b[] = {7,6,5,4,3,2,1,0};
    static_assert(f<decltype(std::begin(a))>(), "");
    CHECK(copy(std::begin(a) + 2, std::end(a) - 2, std::begin(b) + 2));
    CHECK(b[0] == 7);
    CHECK(b[1] == 6);
    CHECK(b[2] == 2);
    CHECK(b[3] == 3);
    CHECK(b[4] == 4);
    CHECK(b[5] == 5);
    CHECK(b[6] == 1);
    CHECK(b[7] == 0);
  }
  return ::test_result();
}
