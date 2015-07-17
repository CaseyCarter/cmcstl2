// -*- compile-command: "(cd ~/cmcstl2/build && make iterator && ./test/iterator)" -*-

#include <stl2/iterator.hpp>

#include <cassert>
#include <iostream>
#include <memory>

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

    friend decltype(auto) iter_move2(iterator i) noexcept {
      //std::cout << "iter_move2(" << static_cast<void*>(i.ptr_) << ")\n";
      assert(i.ptr_);
      return static_cast<T&&>(*i.ptr_);
    }
  };

  iterator begin() { return {&e_[0]}; }
  iterator end() { return {&e_[0] + N}; }
};

namespace adl {
using stl2::iter_move2;

template <class RR1, class RR2,
          class R1 = std::remove_reference_t<RR1>,
          class R2 = std::remove_reference_t<RR2>>
void foo(RR1&& r1, RR2&& r2)
  noexcept(stl2::detail::is_nothrow_iter_swappable_v<R1, R2>) {
  stl2::ValueType<R1> tmp(iter_move2(r1));
  *r1 = iter_move2(r2);
  *r2 = stl2::move(tmp);
}
}

using adl::foo;

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
    static_assert(stl2::detail::is_nothrow_iter_swappable_v<I, I>, "");

    // foo - an unconstrained copy of iter_swap2 - works:
    foo(a.begin() + 0, a.begin() + 2);
    CHECK(a[0] == 2);
    CHECK(a[1] == 1);
    CHECK(a[2] == 0);
    CHECK(a[3] == 3);

    // Swappable<R, R>() is false, and is_nothrow_iter_swappable<I, I>
    // implies IterSwappable<I, I>, so this should resolve to the second
    // overload of iter_swap2. Except that it won't compile because
    // the constraints are not satisfied?!?
    //stl2::iter_swap2(a.begin() + 1, a.begin() + 3);
    CHECK(a[0] == 2);
    CHECK(a[1] == 3);
    CHECK(a[2] == 0);
    CHECK(a[3] == 1);
  }

  return ::test_result();
}
