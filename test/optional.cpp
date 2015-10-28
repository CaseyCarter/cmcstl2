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

namespace models = ::__stl2::models;

#if ASSEMBLY // Only for assembly inspection.
using T = int;
using U = double;

T test_dereference(const __stl2::optional<T>& o) noexcept {
  return *o;
}

int test_value(const __stl2::optional<T>& o) {
  return o.value();
}

__stl2::optional<T> test_copy(const __stl2::optional<T>& o)
  noexcept(__stl2::is_nothrow_copy_constructible<__stl2::optional<T>>::value) {
  return o;
}

bool test_eq(const __stl2::optional<T>& l, const __stl2::optional<U>& r) {
  return l == r;
}

auto test_convert(__stl2::optional<U>&& o) {
  return __stl2::optional<T>{__stl2::move(o)};
}

auto test_convert(const __stl2::optional<U>& o) {
  return __stl2::optional<T>{o};
}

auto test_convert(__stl2::optional<T>&& o) {
  return __stl2::optional<U>{__stl2::move(o)};
}

auto test_convert(const __stl2::optional<T>& o) {
  return __stl2::optional<U>{o};
}

#endif

int main() {
  {
    __stl2::optional<int> o;
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

    CHECK(!(__stl2::nullopt == o));
    CHECK(__stl2::nullopt != o);
    CHECK(__stl2::nullopt < o);
    CHECK(!(__stl2::nullopt > o));
    CHECK(__stl2::nullopt <= o);
    CHECK(!(__stl2::nullopt >= o));

    CHECK(!(o == __stl2::nullopt));
    CHECK(o != __stl2::nullopt);
    CHECK(!(o < __stl2::nullopt));
    CHECK(o > __stl2::nullopt);
    CHECK(!(o <= __stl2::nullopt));
    CHECK(o >= __stl2::nullopt);

    CHECK(o == __stl2::optional<int>{42});
    CHECK(o == __stl2::optional<double>{42});
    CHECK(o != __stl2::optional<int>{13});
    CHECK(o != __stl2::optional<double>{13});
    CHECK(o.value() == 42);
    o = __stl2::nullopt;
    CHECK(!o);
    CHECK(o.value_or(42) == 42);
    o = 3.14;
    CHECK(o);
    CHECK(o == 3);
    o = {};
    CHECK(!o);
    {
      auto oi = __stl2::make_optional(42);
      static_assert(models::Swappable<__stl2::optional<int>&>);
      __stl2::swap(o, oi);
      CHECK(!oi);
      CHECK(o);
      CHECK(*o == 42);
      oi = 13;
      __stl2::swap(o, oi);
      CHECK(*o == 13);
      CHECK(*oi == 42);
    }
  }

  {
    constexpr auto o = __stl2::make_optional(42);
    static_assert(o);
    static_assert(*o == 42);
    static_assert(o == 42);
    static_assert(o < 43);
    static_assert(o.value() == 42);
    static_assert(o.value_or(13) == 42);
    static_assert(o == __stl2::optional<int>{42});
    static_assert(o == __stl2::optional<double>{42});
    static_assert(o != __stl2::optional<int>{13});
    static_assert(o != __stl2::optional<double>{3.14});
  }
  {
    constexpr __stl2::optional<int> o;
    static_assert(!o);
    static_assert(o.value_or(42) == 42);
    static_assert(o != __stl2::optional<int>{13});
    static_assert(o != __stl2::optional<double>{3.14});
  }

  {
    using OI = __stl2::optional<int>;
    using OD = __stl2::optional<double>;
    static_assert(models::StrictTotallyOrdered<OI, OD>);
    static_assert(!models::Swappable<OI&, OD&>);
  }

  return test_result();
}
