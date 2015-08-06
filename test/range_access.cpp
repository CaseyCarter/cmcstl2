#include <stl2/detail/range/concepts.hpp>
#include <stl2/iterator.hpp>
#include <cassert>
#include <iostream>

namespace models = stl2::ext::models;

namespace ns {
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

  static constexpr ns::array<int, 4> some_ints = {{0,1,2,3}};

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
    assert(i == count++);
    if (is_first) {
      is_first = false;
    } else {
      if (output) std::cout << ", ";
    }
    if (output) std::cout << i;
  }
  if (output) std::cout << "}\n";
}
