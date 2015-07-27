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

enum class category {
  none, weak_output, output,
    weak_input, input, forward, bidirectional, random_access, contiguous
};

template <class>
constexpr category iterator_dispatch() { return category::none; }
#if 0
// These don't overload properly thanks to GCC bug 67038.
template <stl2::WeakOutputIterator<int> I>
  requires !stl2::WeakInputIterator<I>()
constexpr category iterator_dispatch() { return category::weak_output; }
template <stl2::OutputIterator<int> I>
  requires !stl2::WeakInputIterator<I>()
constexpr category iterator_dispatch() { return category::output; }
#endif
template <stl2::WeakInputIterator>
constexpr category iterator_dispatch() { return category::weak_input; }
template <stl2::InputIterator>
constexpr category iterator_dispatch() { return category::input; }
template <stl2::ForwardIterator>
constexpr category iterator_dispatch() { return category::forward; }
template <stl2::BidirectionalIterator>
constexpr category iterator_dispatch() { return category::bidirectional; }
template <stl2::RandomAccessIterator>
constexpr category iterator_dispatch() { return category::random_access; }
template <stl2::ext::ContiguousIterator>
constexpr category iterator_dispatch() { return category::contiguous; }

struct weak_output_iterator_tag {};
struct output_iterator_tag : weak_output_iterator_tag {};

template <class C>
struct arbitrary_iterator {
  using difference_type = std::ptrdiff_t;

  arbitrary_iterator& operator*();
  arbitrary_iterator& operator=(int);

  arbitrary_iterator& operator++();
  arbitrary_iterator& operator++(int);

  bool operator==(arbitrary_iterator) const
    requires stl2::DerivedFrom<C, output_iterator_tag>();
  bool operator!=(arbitrary_iterator) const
    requires stl2::DerivedFrom<C, output_iterator_tag>();
};

template <stl2::DerivedFrom<stl2::weak_input_iterator_tag> C>
struct arbitrary_iterator<C> {
  using iterator_category = C;
  using value_type = int;
  using difference_type = std::ptrdiff_t;

  value_type& operator*() const;

  arbitrary_iterator& operator++();
  arbitrary_iterator operator++(int);

  bool operator==(arbitrary_iterator) const
    requires stl2::DerivedFrom<C, stl2::input_iterator_tag>();
  bool operator!=(arbitrary_iterator) const
    requires stl2::DerivedFrom<C, stl2::input_iterator_tag>();

  arbitrary_iterator& operator--()
    requires stl2::DerivedFrom<C, stl2::bidirectional_iterator_tag>();
  arbitrary_iterator operator--(int)
    requires stl2::DerivedFrom<C, stl2::bidirectional_iterator_tag>();

  bool operator<(arbitrary_iterator) const
    requires stl2::DerivedFrom<C, stl2::random_access_iterator_tag>();
  bool operator>(arbitrary_iterator) const
    requires stl2::DerivedFrom<C, stl2::random_access_iterator_tag>();
  bool operator<=(arbitrary_iterator) const
    requires stl2::DerivedFrom<C, stl2::random_access_iterator_tag>();
  bool operator>=(arbitrary_iterator) const
    requires stl2::DerivedFrom<C, stl2::random_access_iterator_tag>();

  arbitrary_iterator& operator+=(difference_type)
    requires stl2::DerivedFrom<C, stl2::random_access_iterator_tag>();
  arbitrary_iterator& operator-=(difference_type)
    requires stl2::DerivedFrom<C, stl2::random_access_iterator_tag>();

  arbitrary_iterator operator-(difference_type) const
    requires stl2::DerivedFrom<C, stl2::random_access_iterator_tag>();
  difference_type operator-(arbitrary_iterator) const
    requires stl2::DerivedFrom<C, stl2::random_access_iterator_tag>();

  value_type& operator[](difference_type) const
    requires stl2::DerivedFrom<C, stl2::random_access_iterator_tag>();
};

template <stl2::DerivedFrom<stl2::random_access_iterator_tag> C>
arbitrary_iterator<C> operator+(arbitrary_iterator<C>,
                                typename arbitrary_iterator<C>::difference_type);

template <stl2::DerivedFrom<stl2::random_access_iterator_tag> C>
arbitrary_iterator<C> operator+(typename arbitrary_iterator<C>::difference_type,
                                arbitrary_iterator<C>);

void test_iterator_dispatch() {
  namespace models = stl2::ext::models;

  CHECK(iterator_dispatch<void>() == category::none);
  CHECK(iterator_dispatch<int*>() == category::contiguous);

  {
    using I = arbitrary_iterator<stl2::weak_input_iterator_tag>;
    static_assert(models::weak_input_iterator<I>());
    CHECK(iterator_dispatch<I>() == category::weak_input);
  }
  {
    using I = arbitrary_iterator<stl2::input_iterator_tag>;
    static_assert(models::input_iterator<I>());
    CHECK(iterator_dispatch<I>() == category::input);
  }
  {
    using I = arbitrary_iterator<stl2::forward_iterator_tag>;
    static_assert(models::forward_iterator<I>());
    CHECK(iterator_dispatch<I>() == category::forward);
  }
  {
    using I = arbitrary_iterator<stl2::bidirectional_iterator_tag>;
    static_assert(models::bidirectional_iterator<I>());
    CHECK(iterator_dispatch<I>() == category::bidirectional);
  }
  {
    using I = arbitrary_iterator<stl2::random_access_iterator_tag>;
    static_assert(models::random_access_iterator<I>());
    CHECK(iterator_dispatch<I>() == category::random_access);
  }
  {
    using I = arbitrary_iterator<stl2::ext::contiguous_iterator_tag>;
    static_assert(models::contiguous_iterator<I>());
    CHECK(iterator_dispatch<I>() == category::contiguous);
  }

  {
    using I = arbitrary_iterator<weak_output_iterator_tag>;
    static_assert(models::weak_output_iterator<I, int>());
    CHECK(iterator_dispatch<I>() == category::weak_output);
  }
  {
    using I = arbitrary_iterator<output_iterator_tag>;
    static_assert(models::output_iterator<I, int>());
    CHECK(iterator_dispatch<I>() == category::output);
  }
}

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

void test_copy() {
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

  test_iterator_dispatch();
  test_copy();

  return ::test_result();
}
