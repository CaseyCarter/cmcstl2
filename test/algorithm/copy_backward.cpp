// Range v3 library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <stl2/detail/algorithm/copy_backward.hpp>
#include <stl2/view/repeat.hpp>
#include <cstring>
#include <utility>
#include <algorithm>
#include "../simple_test.hpp"

namespace stl2 = __stl2;

namespace {
  void test_repeat_view() {
    {
      auto v = stl2::repeat_view<int>(42);
      int target[8]{};
      auto result = stl2::copy_backward(stl2::make_counted_iterator(v.begin(), 4),
                                        stl2::make_counted_iterator(v.begin(), 0),
                                        stl2::end(target));
      CHECK(result.in().count() == 0);
      CHECK(result.in().base() == v.begin());
      CHECK(result.out() == target + 4);
      CHECK(std::count(target, target + 4, 0) == 4);
      CHECK(std::count(target + 4, target + 8, 42) == 4);
    }
    {
      auto v = stl2::repeat_view<int>(42);
      int target[8]{};
      auto result = stl2::copy_backward(stl2::make_counted_iterator(v.begin(), 4),
                                        stl2::default_sentinel{},
                                        stl2::end(target));
      CHECK(result.in().count() == 0);
      CHECK(result.in().base() == v.begin());
      CHECK(result.out() == target + 4);
      CHECK(std::count(target, target + 4, 0) == 4);
      CHECK(std::count(target + 4, target + 8, 42) == 4);
    }
  }
}

int main()
{
    using stl2::begin;
    using stl2::end;
    using stl2::size;

    std::pair<int, int> const a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    static_assert(size(a) == 6, "");
    std::pair<int, int> out[size(a)] = {};

    auto res = stl2::copy_backward(begin(a), end(a), end(out));
    CHECK(res.first == end(a));
    CHECK(res.second == begin(out));
    CHECK(std::equal(a, a + size(a), out));

    std::fill_n(out, size(out), std::make_pair(0, 0));
    CHECK(!std::equal(a, a + size(a), out));

    res = stl2::copy_backward(a, end(out));
    CHECK(res.first == end(a));
    CHECK(res.second == begin(out));
    CHECK(std::equal(a, a + size(a), out));

    std::fill_n(out, size(out), std::make_pair(0, 0));
    auto res2 = stl2::copy_backward(stl2::move(a), end(out));
    CHECK(res2.first.get_unsafe() == end(a));
    CHECK(res2.second == begin(out));
    CHECK(std::equal(a, a + size(a), out));

    test_repeat_view();

    return test_result();
}
