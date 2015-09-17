// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <algorithm>
#include <cstring>
#include "../simple_test.hpp"

namespace stl2 = __stl2;

template <stl2::InputIterator I>
  requires stl2::Regular<stl2::ValueType<I>>()
class delimiter {
  stl2::ValueType<I> value_;
public:
  delimiter() = default;
  delimiter(stl2::ValueType<I> value) :
    value_{value} {}

  friend bool operator==(const delimiter&, const delimiter&) {
    return true;
  }
  friend bool operator!=(const delimiter&, const delimiter&) {
    return false;
  }
  friend bool operator==(const I& i, const delimiter& s) {
    return *i == s.value_;
  }
  friend bool operator==(const delimiter& s, const I& i) {
    return i == s;
  }
  friend bool operator!=(const I& i, const delimiter& s) {
    return !(i == s);
  }
  friend bool operator!=(const delimiter& s, const I& i) {
    return !(i == s);
  }
};

int main()
{
    using stl2::begin;
    using stl2::end;
    using stl2::size;

    stl2::pair<int, int> const a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    static_assert(size(a) == 6, "");
    stl2::pair<int, int> out[size(a)] = {};

    auto res = stl2::copy(begin(a), end(a), out);
    CHECK(res.first == end(a));
    CHECK(res.second == out + size(out));
    CHECK(&res.first == &res.in());
    CHECK(&res.second == &res.out());
    CHECK(std::equal(a, a + size(a), out));

    std::fill_n(out, size(out), std::make_pair(0, 0));
    CHECK(!std::equal(a, a + size(a), out));

    res = stl2::copy(a, out);
    CHECK(res.first == a + size(a));
    CHECK(res.second == out + size(out));
    CHECK(std::equal(a, a + size(a), out));

    std::fill_n(out, size(out), std::make_pair(0, 0));

    {
        char const *sz = "hello world";
        char buf[50];
        auto str = stl2::ext::make_range(sz, delimiter<const char*>{'\0'});
        {
          std::fill_n(buf, sizeof(buf), '\0');
          auto res3 = stl2::copy(str, buf);
          *res3.second = '\0';
          CHECK(res3.first == std::next(begin(str), std::strlen(sz)));
          CHECK(res3.second == buf + std::strlen(sz));
          CHECK(std::strcmp(sz, buf) == 0);
        }
        {
          std::fill_n(buf, sizeof(buf), '\0');
          auto res4 = stl2::copy(stl2::move(str), buf);
          *res4.second = '\0';
          CHECK(res4.first.get_unsafe() == std::next(begin(str), std::strlen(sz)));
          CHECK(res4.second == buf + std::strlen(sz));
          CHECK(std::strcmp(sz, buf) == 0);
        }
    }

    return test_result();
}
