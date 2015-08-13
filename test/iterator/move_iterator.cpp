#include <stl2/iterator.hpp>
#include <stl2/functional.hpp>
#include <vector>
#include "../simple_test.hpp"

struct A {
  static std::size_t copy_count;
  static std::size_t move_count;

  static void clear() { copy_count = move_count = 0; }

  A() = default;
  A(const A&) { ++copy_count; }
  A(A&&) noexcept { ++move_count; }
  A& operator=(const A&) & { ++copy_count; return *this; }
  A& operator=(A&&) & noexcept { ++move_count; return *this; }
};

std::size_t A::copy_count;
std::size_t A::move_count;

void test_move_iterator() {
  static constexpr std::size_t N = 42;
  std::vector<A> vec(N);
  std::vector<A> vec2;
  vec2.reserve(vec.size());

  A::clear();

  {
    auto first = stl2::make_move_iterator(vec.begin()),
      last = stl2::make_move_iterator(vec.end());
    auto out = stl2::back_inserter(vec2);

    for (; first != last; ++first, ++out) {
      *out = *first;
    }
  }

  CHECK(vec2.size() == N);
  CHECK(A::copy_count == std::size_t{0});
  CHECK(A::move_count == N);
}

void test_iter_move() {
  static constexpr std::size_t N = 42;
  std::vector<A> vec(N);
  std::vector<A> vec2;
  vec2.reserve(vec.size());

  A::clear();

  {
    auto first = vec.begin(),
      last = vec.end();
    auto out = stl2::back_inserter(vec2);

    for (; first != last; ++first, ++out) {
      using stl2::iter_move;
      *out = iter_move(first);
    }
  }

  CHECK(vec2.size() == N);
  CHECK(A::copy_count == std::size_t{0});
  CHECK(A::move_count == N);
}

void test_both() {
  static constexpr std::size_t N = 42;
  std::vector<A> vec(N);
  std::vector<A> vec2;
  vec2.reserve(vec.size());

  A::clear();

  {
    auto first = stl2::make_move_iterator(vec.begin()),
      last = stl2::make_move_iterator(vec.end());
    auto out = stl2::back_inserter(vec2);

    for (; first != last; ++first, ++out) {
      using stl2::iter_move;
      *out = iter_move(first);
    }
  }

  CHECK(vec2.size() == N);
  CHECK(A::copy_count == std::size_t{0});
  CHECK(A::move_count == N);
}

template <class T>
class proxy_iterator {
public:
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using iterator_category = stl2::random_access_iterator_tag;

  proxy_iterator() = default;
  explicit proxy_iterator(T* p) :
    ptr_{p} {}

  stl2::reference_wrapper<T> operator*() const {
    return stl2::reference_wrapper<T>{*ptr_};
  }

  bool operator==(const proxy_iterator& that) const {
    return ptr_ == that.ptr_;
  }
  bool operator!=(const proxy_iterator& that) const {
    return !(*this == that);
  }

  proxy_iterator& operator++() & {
    ++ptr_;
    return *this;
  }
  proxy_iterator operator++(int) & {
    auto tmp = *this;
    ++*this;
    return tmp;
  }

  friend decltype(auto) iter_move(proxy_iterator p) {
    return stl2::move(*p.ptr_);
  }

private:
  T* ptr_;
};

void test_proxy_iterator() {
  static constexpr std::size_t N = 42;
  std::vector<A> vec(N);
  std::vector<A> vec2;
  vec2.reserve(vec.size());

  A::clear();

  {
    auto first = stl2::make_move_iterator(proxy_iterator<A>{stl2::data(vec)}),
      last = stl2::make_move_iterator(proxy_iterator<A>{stl2::data(vec) + stl2::size(vec)});
    auto out = stl2::back_inserter(vec2);

    for (; first != last; ++first, ++out) {
      *out = *first;
    }
  }

  CHECK(vec2.size() == N);
  CHECK(A::copy_count == std::size_t{0});
  CHECK(A::move_count == N);
}

int main() {
  test_move_iterator();
  test_iter_move();
  test_both();
  test_proxy_iterator();
  return ::test_result();
}
