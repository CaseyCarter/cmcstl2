#include <stl2/iterator.hpp>

#include <cassert>
#include <memory>

#include "simple_test.hpp"

template <class T>
struct reference_wrapper {
  T* ptr_;

  reference_wrapper() = default;
  reference_wrapper(T& t) : ptr_{std::addressof(t)} {}
  reference_wrapper(T&&) = delete;

  T& get() const { assert(ptr_); return *ptr_; }

  reference_wrapper& operator=(const T& t) {
    get() = t;
    return *this;
  }

  reference_wrapper& operator=(T&& t) {
    get() = stl2::move(t);
    return *this;
  }

  operator T&() const { return get(); }
};

template <class T, std::size_t N>
struct array {
  T e_[N];

  using reference = reference_wrapper<T>;
  using const_reference = reference_wrapper<const T>;

  reference operator[](const std::size_t n) {
    return {e_[n]};
  }
  const_reference operator[](const std::size_t n) const {
    return {e_[n]};
  }

  struct iterator {
    using iterator_category = stl2::random_access_iterator_tag;
    using difference_t = std::ptrdiff_t;
    using value_type = T;

    T* ptr_;

    iterator() = default;
    iterator(T* p) : ptr_{p} {}

    reference operator*() const {
      return {*ptr_};
    }

    T* operator->() const
      requires std::is_class<T>::value ||
        std::is_union<T>::value
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

    friend T&& iter_move2(iterator i) {
      assert(i.ptr_);
      return stl2::move(*i.ptr_);
    }
  };

  iterator begin() { return {&e_[0]}; }
  iterator end() { return {&e_[0] + N}; }
};

int main() {
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
    stl2::iter_swap2(a, b);
    CHECK(*a == 13);
    CHECK(*b == 42);
    stl2::iter_swap2(a, b);
    CHECK(*a == 42);
    CHECK(*b == 13);
  }

  {
    namespace models = stl2::ext::models;

    array<int, 4> a{0,1,2,3};
    using I = decltype(a.begin());
    static_assert(models::readable<I>(), "");
    using V = stl2::ValueType<I>;
    static_assert(models::same<int, V>(), "");
    using R = stl2::ReferenceType<I>;
    static_assert(models::same<reference_wrapper<int>, R>(), "");
    using RR = stl2::RvalueReferenceType<I>;
    static_assert(models::same<int&&, RR>(), "");
    static_assert(models::constructible<V, RR>(), "");
    static_assert(models::move_writable<I, V>(), "");
    static_assert(models::move_writable<I, RR>(), "");
    static_assert(!models::swappable<stl2::ReferenceType<I>, stl2::ReferenceType<I>>(), "");

    stl2::iter_swap2(a.begin() + 0, a.begin() + 2);
    CHECK(a[0] == 2);
    CHECK(a[0] == 1);
    CHECK(a[0] == 0);
    CHECK(a[0] == 3);
  }

  return ::test_result();
}
