#include <stl2/optional.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/utility.hpp>
#include "simple_test.hpp"

namespace models = stl2::ext::models;

#if ASSEMBLY // Only for assembly inspection.
using T = int;
using U = double;

T test_dereference(const stl2::optional<T>& o) noexcept {
  return *o;
}

int test_value(const stl2::optional<T>& o) {
  return o.value();
}

stl2::optional<T> test_copy(const stl2::optional<T>& o)
  noexcept(stl2::is_nothrow_copy_constructible<stl2::optional<T>>::value) {
  return o;
}

bool test_eq(const stl2::optional<T>& l, const stl2::optional<U>& r) {
  return l == r;
}

auto test_convert(stl2::optional<U>&& o) {
  return stl2::optional<T>{stl2::move(o)};
}

auto test_convert(const stl2::optional<U>& o) {
  return stl2::optional<T>{o};
}

auto test_convert(stl2::optional<T>&& o) {
  return stl2::optional<U>{stl2::move(o)};
}

auto test_convert(const stl2::optional<T>& o) {
  return stl2::optional<U>{o};
}

#endif

int main() {
  {
    stl2::optional<int> o;
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

    CHECK(!(stl2::nullopt == o));
    CHECK(stl2::nullopt != o);
    CHECK(stl2::nullopt < o);
    CHECK(!(stl2::nullopt > o));
    CHECK(stl2::nullopt <= o);
    CHECK(!(stl2::nullopt >= o));

    CHECK(!(o == stl2::nullopt));
    CHECK(o != stl2::nullopt);
    CHECK(!(o < stl2::nullopt));
    CHECK(o > stl2::nullopt);
    CHECK(!(o <= stl2::nullopt));
    CHECK(o >= stl2::nullopt);

    CHECK(o == stl2::optional<int>{42});
    CHECK(o == stl2::optional<double>{42});
    CHECK(o != stl2::optional<int>{13});
    CHECK(o != stl2::optional<double>{13});
    CHECK(o.value() == 42);
    o = stl2::nullopt;
    CHECK(!o);
    CHECK(o.value_or(42) == 42);
    o = 3.14;
    CHECK(o);
    CHECK(o == 3);
    o = {};
    CHECK(!o);
    {
      auto oi = stl2::make_optional(42);
      static_assert(models::swappable<stl2::optional<int>&>());
      stl2::swap(o, oi);
      CHECK(!oi);
      CHECK(o);
      CHECK(*o == 42);
      oi = 13;
      stl2::swap(o, oi);
      CHECK(*o == 13);
      CHECK(*oi == 42);
    }
  }

  {
    constexpr auto o = stl2::make_optional(42);
    static_assert(o);
    static_assert(*o == 42);
    static_assert(o == 42);
    static_assert(o < 43);
    static_assert(o.value() == 42);
    static_assert(o.value_or(13) == 42);
    static_assert(o == stl2::optional<int>{42});
    static_assert(o == stl2::optional<double>{42});
    static_assert(o != stl2::optional<int>{13});
    static_assert(o != stl2::optional<double>{3.14});
  }
  {
    constexpr stl2::optional<int> o;
    static_assert(!o);
    static_assert(o.value_or(42) == 42);
    static_assert(o != stl2::optional<int>{13});
    static_assert(o != stl2::optional<double>{3.14});
  }

  {
    using OI = stl2::optional<int>;
    using OD = stl2::optional<double>;
    static_assert(models::totally_ordered<OI, OD>());
    static_assert(!models::swappable<OI&, OD&>());
  }

  return test_result();
}
