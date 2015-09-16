#include <stl2/iterator.hpp>
#include "../simple_test.hpp"

using namespace __stl2;

int main() {
  {
    using I = ext::range<int*, int*>;
    using CI = ext::range<const int*, const int*>;
    static_assert(models::View<I>);
    static_assert(models::SizedRange<I>);
    static_assert(models::ContiguousRange<I>);
    static_assert(models::BoundedRange<I>);
    static_assert(models::View<CI>);
    static_assert(models::SizedRange<CI>);
    static_assert(models::ContiguousRange<CI>);
    static_assert(models::BoundedRange<CI>);
  }

  {
    static constexpr int some_ints[] = {2, 3, 5, 7, 11, 13};
    static constexpr std::size_t n = size(some_ints);
    auto r = ext::make_range(some_ints + 0, some_ints + n);
    using R = decltype(r);
    static_assert(models::View<R>);
    static_assert(models::SizedRange<R>);
    static_assert(models::ContiguousRange<R>);
    static_assert(models::BoundedRange<R>);

    CHECK(begin(r) == some_ints + 0);
    CHECK(end(r) == some_ints + n);
    CHECK(!empty(r));
    CHECK(std::size_t(size(r)) == n);
    CHECK(data(r) == some_ints);

    constexpr auto c = ext::make_range(some_ints, some_ints + n);
    static_assert(begin(c) == begin(some_ints));
    static_assert(end(c) == end(some_ints));
    static_assert(!empty(c));
    static_assert(size(c) == size(some_ints));
    // static_assert(data(c) == data(some_ints)); // std::addressof is not constexpr.
  }

  return test_result();
}
