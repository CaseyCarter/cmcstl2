#include <stl2/iterator.hpp>
#include <iostream>
#include <vector>
#include "simple_test.hpp"

namespace models = stl2::ext::models;

void test_range_access_ambiguity() {
  std::vector<stl2::reverse_iterator<int*>> vri{};
  using namespace stl2;
  (void)begin(vri);
  (void)end(vri);
  (void)cbegin(vri);
  (void)cend(vri);
  (void)rbegin(vri);
  (void)rend(vri);
  (void)crbegin(vri);
  (void)crend(vri);
}

void test_initializer_list() {
  std::initializer_list<int> il = {0,1,2};
  {
    int count = 0;
    for (auto p = stl2::begin(il), e = stl2::end(il); p != e; ++p) {
      CHECK(*p == count++);
    }
  }
  {
    int count = 3;
    for (auto p = stl2::rbegin(il), e = stl2::rend(il); p != e; ++p) {
      CHECK(*p == --count);
    }
  }
  CHECK(stl2::size(il) == std::size_t{3});

  // libstdc++ has overloads of data and empty that are found by
  // ADL but don't work for initializer_list.
  CHECK(stl2::__data::data(il) == &*il.begin());
  CHECK(stl2::__empty::empty(il) == false);
}

void test_array() {
  const int a[] = {0,1,2};
  {
    int count = 0;
    for (auto p = stl2::begin(a), e = stl2::end(a); p != e; ++p) {
      CHECK(*p == count++);
    }
  }
  {
    int count = 3;
    for (auto p = stl2::rbegin(a), e = stl2::rend(a); p != e; ++p) {
      CHECK(*p == --count);
    }
  }
  CHECK(stl2::size(a) == std::size_t{3});
  CHECK(stl2::data(a) == a + 0);
  CHECK(stl2::empty(a) == false);
}

namespace X {
template <class T, std::size_t N>
  requires N != 0
struct array {
  T elements_[N];

  constexpr bool empty() const noexcept { return N == 0; }
  constexpr T* data() noexcept { return elements_; }
  constexpr const T* data() const noexcept { return elements_; }
};

template <class T, std::size_t N>
constexpr T* begin(array<T, N>& a) noexcept { return a.elements_; }
template <class T, std::size_t N>
constexpr T* end(array<T, N>& a) noexcept { return a.elements_ + N; }
template <class T, std::size_t N>
constexpr const T* begin(const array<T, N>& a) noexcept { return a.elements_; }
template <class T, std::size_t N>
constexpr const T* end(const array<T, N>& a) noexcept { return a.elements_ + N; }
}

using I = int*;
using CI = const int*;
static_assert(models::iterator<I>());
static_assert(models::iterator<CI>());

int main() {
  using namespace stl2;

  static constexpr X::array<int, 4> some_ints = {{0,1,2,3}};

  constexpr auto first = begin(some_ints);
  constexpr auto last = end(some_ints);
  static_assert(models::same<const CI, decltype(first)>());
  static_assert(models::same<const CI, decltype(last)>());
  static_assert(first == cbegin(some_ints));
  static_assert(last == cend(some_ints));

  static_assert(noexcept(begin(some_ints)));
  static_assert(noexcept(end(some_ints)));
  static_assert(noexcept(cbegin(some_ints)));
  static_assert(noexcept(cend(some_ints)));
  static_assert(noexcept(empty(some_ints)));
  static_assert(noexcept(data(some_ints)));

  constexpr bool output = false;
  static_assert(!empty(some_ints));
  if (output) std::cout << '{';
  auto is_first = true;
  auto count = 0;
  for (auto&& i : some_ints) {
    CHECK(i == count++);
    if (is_first) {
      is_first = false;
    } else {
      if (output) std::cout << ", ";
    }
    if (output) std::cout << i;
  }
  if (output) std::cout << "}\n";

  test_initializer_list();
  test_array();
  return ::test_result();
}
