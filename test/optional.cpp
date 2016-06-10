// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/optional.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/utility.hpp>
#include "simple_test.hpp"

namespace ranges = ::std::experimental::ranges;
namespace models = ranges::models;

#if ASSEMBLY // Only for assembly inspection.
using T = int;
using U = double;

T test_dereference(const ranges::optional<T>& o) noexcept {
  return *o;
}

int test_value(const ranges::optional<T>& o) {
  return o.value();
}

ranges::optional<T> test_copy(const ranges::optional<T>& o)
  noexcept(ranges::is_nothrow_copy_constructible<ranges::optional<T>>::value) {
  return o;
}

bool test_eq(const ranges::optional<T>& l, const ranges::optional<U>& r) {
  return l == r;
}

auto test_convert(ranges::optional<U>&& o) {
  return ranges::optional<T>{ranges::move(o)};
}

auto test_convert(const ranges::optional<U>& o) {
  return ranges::optional<T>{o};
}

auto test_convert(ranges::optional<T>&& o) {
  return ranges::optional<U>{ranges::move(o)};
}

auto test_convert(const ranges::optional<T>& o) {
  return ranges::optional<U>{o};
}

#endif

int main() {
  {
    ranges::optional<int> o;
    CHECK(!o);
    o = 42;
    CHECK(o);
    CHECK(*o == 42);
    CHECK(o == 42);
    CHECK(o < 43);
    CHECK(o <= 43);
    CHECK(o > 41);
    CHECK(o >= 41);
    CHECK(!(o < o));
    CHECK(!(o > o));
    CHECK(o <= o);
    CHECK(o >= o);

    CHECK(!(ranges::nullopt == o));
    CHECK(ranges::nullopt != o);
    CHECK(ranges::nullopt < o);
    CHECK(!(ranges::nullopt > o));
    CHECK(ranges::nullopt <= o);
    CHECK(!(ranges::nullopt >= o));

    CHECK(!(o == ranges::nullopt));
    CHECK(o != ranges::nullopt);
    CHECK(!(o < ranges::nullopt));
    CHECK(o > ranges::nullopt);
    CHECK(!(o <= ranges::nullopt));
    CHECK(o >= ranges::nullopt);

    CHECK(o == ranges::optional<int>{42});
    CHECK(o == ranges::optional<double>{42});
    CHECK(o != ranges::optional<int>{13});
    CHECK(o != ranges::optional<double>{13});
    CHECK(o.value() == 42);
    o = ranges::nullopt;
    CHECK(!o);
    CHECK(o.value_or(42) == 42);
    o = 3.14;
    CHECK(o);
    CHECK(o == 3);
    o = {};
    CHECK(!o);
    {
      auto oi = ranges::make_optional(42);
      static_assert(models::Swappable<ranges::optional<int>&>);
      ranges::swap(o, oi);
      CHECK(!oi);
      CHECK(o);
      CHECK(*o == 42);
      oi = 13;
      ranges::swap(o, oi);
      CHECK(*o == 13);
      CHECK(*oi == 42);
    }
  }

  {
    constexpr auto o = ranges::make_optional(42);
    static_assert(o);
    static_assert(*o == 42);
    static_assert(o == 42);
    static_assert(o < 43);
    static_assert(o.value() == 42);
    static_assert(o.value_or(13) == 42);
    static_assert(o == ranges::optional<int>{42});
    static_assert(o == ranges::optional<double>{42});
    static_assert(o != ranges::optional<int>{13});
    static_assert(o != ranges::optional<double>{3.14});
  }
  {
    constexpr ranges::optional<int> o;
    static_assert(!o);
    static_assert(o.value_or(42) == 42);
    static_assert(o != ranges::optional<int>{13});
    static_assert(o != ranges::optional<double>{3.14});
  }

  {
    using OI = ranges::optional<int>;
    using OD = ranges::optional<double>;
    static_assert(models::StrictTotallyOrdered<OI, OD>);
    static_assert(!models::Swappable<OI&, OD&>);
  }

  {
    using OI = ranges::optional<int>;
    constexpr OI four{4};
    constexpr OI empty;
    static_assert(!(four == empty));
    static_assert(four != empty);
    static_assert(!(four < empty));
    static_assert(four > empty);
    static_assert(!(four <= empty));
    static_assert(four >= empty);

    static_assert(!(empty == four));
    static_assert(empty != four);
    static_assert(empty < four);
    static_assert(!(empty > four));
    static_assert(empty <= four);
    static_assert(!(empty >= four));
  }

  {
    using OI = ranges::optional<int>;
    using OL = ranges::optional<long>;
    constexpr OI four{4};
    constexpr OL three{3};
    static_assert(!(four == three));
    static_assert(four != three);
    static_assert(!(four < three));
    static_assert(four > three);
    static_assert(!(four <= three));
    static_assert(four >= three);

    static_assert(!(three == four));
    static_assert(three != four);
    static_assert(three < four);
    static_assert(!(three > four));
    static_assert(three <= four);
    static_assert(!(three >= four));
  }

  return test_result();
}
