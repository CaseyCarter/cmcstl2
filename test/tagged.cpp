#include <array>
#include <stl2/detail/tagged.hpp>
#include <stl2/utility.hpp>
#include "simple_test.hpp"

namespace models = stl2::ext::models;

using TP = stl2::tagged_pair<stl2::tag::in(int), stl2::tag::out(double)>;
using TT = stl2::tagged_tuple<stl2::tag::in(int), stl2::tag::out(double)>;

constexpr auto tp(int i, double d) {
  return TP{i, d};
}

constexpr auto tt(int i, double d) {
  return TT{i, d};
}

void test_pair() {
  auto t = tp(42, 3.14);
  static_assert(models::same<decltype(t), TP>());
  CHECK(t.first == 42);
  CHECK(std::get<0>(t) == 42);
  CHECK(t.in() == 42);
  CHECK(t.second == 3.14);
  CHECK(std::get<1>(t) == 3.14);
  CHECK(t.out() == 3.14);
}

void test_constexpr_pair() {
  constexpr auto t = tp(42, 3.14);
  static_assert(models::same<decltype(t), const TP>());
  constexpr int f = t.first;
  CHECK(f == 42);
  constexpr int fg = std::get<0>(t);
  CHECK(fg == 42);
  constexpr int ff = t.in();
  CHECK(ff == 42);
  constexpr double s = t.second;
  CHECK(s == 3.14);
  constexpr double sg = std::get<1>(t);
  CHECK(sg == 3.14);
  constexpr double sf = t.out();
  CHECK(sf == 3.14);
}

void test_tuple() {
  auto t = tt(42, 3.14);
  static_assert(models::same<decltype(t), TT>());
  CHECK(std::get<0>(t) == 42);
  CHECK(t.in() == 42);
  CHECK(std::get<1>(t) == 3.14);
  CHECK(t.out() == 3.14);
}

void test_constexpr_tuple() {
  constexpr auto t = tt(42, 3.14);
  static_assert(models::same<decltype(t), const TT>());
  constexpr int fg = std::get<0>(t);
  CHECK(fg == 42);
  constexpr int ff = t.in();
  CHECK(ff == 42);
  constexpr double sg = std::get<1>(t);
  CHECK(sg == 3.14);
  constexpr double sf = t.out();
  CHECK(sf == 3.14);
}

void test_conversions() {
  auto t = tt(42, 3.14);
  CHECK(t == tt(42, 3.14));
  t = tt(13, 1.414);
  CHECK(t == tt(13, 1.414));
  t = tp(42, 3.14);
  CHECK(t == tt(42, 3.14));
  t = std::make_tuple(13, 1.414);
  CHECK(t == tt(13, 1.414));
  t = std::make_pair(42, 3.14);
  CHECK(t == tt(42, 3.14));
}

void test_swap() {
  using stl2::swap;

  auto t1 = tt(42, 3.14);
  decltype(t1) t2{};

  std::pair<int, double> stdp;
  swap(t1, t2);

  CHECK(t1.in() == 0);
  CHECK(t1.out() == 0);
  CHECK(t2.in() == 42);
  CHECK(t2.out() == 3.14);
}

template <class T> T& lv();

struct nothrow_pair {
  int first; int second;

  nothrow_pair() = default;
  constexpr nothrow_pair(int f, int s) noexcept : first(f), second(s) {}
};

namespace std {
  template <>
  struct tuple_size<nothrow_pair> : meta::size_t<2> {};
  template <std::size_t N>
    requires N < 2
  struct tuple_element<N, nothrow_pair> : meta::id<int> {};
}

void test_noexcept() {
  using T = stl2::tagged<nothrow_pair, stl2::tag::in1, stl2::tag::in2>;
  constexpr T t{1, 2}; (void)t;

  CHECK(std::is_nothrow_default_constructible<T>());
  CHECK(std::is_nothrow_move_constructible<T>());
  CHECK(std::is_nothrow_copy_constructible<T>());
  CHECK(std::is_nothrow_move_assignable<T>());
  CHECK(std::is_nothrow_copy_assignable<T>());
  CHECK(noexcept(T{1, 2}));
  CHECK(stl2::ext::is_nothrow_swappable_v<T&>);
}

// Why? Because we can.
void test_array() {
  using T = stl2::tagged<std::array<int, 3>, stl2::tag::in, stl2::tag::in1, stl2::tag::in2>;
  auto t = T{std::array<int, 3>{{0, 1, 2}}};
  CHECK(t.in() == 0);
  CHECK(t.in1() == 1);
  CHECK(t.in2() == 2);
}

int main() {
  test_pair();
  test_tuple();
  test_constexpr_pair();
  test_constexpr_tuple();
  test_conversions();
  test_swap();
  test_noexcept();
  test_array();
  return ::test_result();
}
