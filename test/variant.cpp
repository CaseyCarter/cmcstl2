#include <cassert>
#include <iostream>
#include <stl2/type_traits.hpp>
#include <stl2/variant.hpp>
#include "simple_test.hpp"

using namespace stl2;
namespace models = stl2::ext::models;

template <class> class show_type;

// variants of trivially {destructible,move constructible,copy constructible} types
// are trivially {destructible,move constructible,copy constructible}.
using VDI = stl2::variant<char, short, int, long, long long, float, double>;
static_assert(stl2::is_trivially_destructible<VDI>());
static_assert(stl2::is_trivially_move_constructible<VDI>());
static_assert(stl2::is_trivially_copy_constructible<VDI>());

// The triviality goal ensures that trivial variants are passed by value in
// registers on x64. This function optimizes to two register moves:
VDI v_test(VDI source) {
  return source;
}

struct nontrivial {
  static int create_count;
  static int move_count;
  static int copy_count;
  static int destroy_count;
  nontrivial() noexcept { ++create_count; }
  nontrivial(const nontrivial&) { ++copy_count; }
  nontrivial(nontrivial&&) noexcept { ++move_count; }
  nontrivial& operator=(nontrivial&&) & noexcept { return *this; }
  nontrivial& operator=(const nontrivial&) & { return *this; }
  ~nontrivial() { ++destroy_count; }
};

int nontrivial::create_count;
int nontrivial::move_count;
int nontrivial::copy_count;
int nontrivial::destroy_count;

struct moveonly {
  moveonly() = default;
  moveonly(moveonly&&) = default;
  moveonly& operator=(moveonly&&) & = default;
};

void test_raw() {
  __variant::data<int, double, nontrivial, reference_wrapper<int>> v;
  static_assert(sizeof(v) == sizeof(double));
  {
    int& i = __variant::raw_get(meta::size_t<0>{}, v);
    {
      const int& i = __variant::raw_get(meta::size_t<0>{}, v);
      (void)i;
    }
    {
      const int& i = __variant::raw_get(meta::size_t<0>{}, const_cast<const decltype(v)&>(v));
      (void)i;
    }
    {
      int&& i = __variant::raw_get(meta::size_t<0>{}, move(v));
      (void)i;
    }
    i = 42;
    CHECK(__variant::raw_get(meta::size_t<0>{}, v) == 42);
  }

  {
    double& d = __variant::raw_get(meta::size_t<1>{}, v);
    ::new(&d) double;
    {
      const double& d = __variant::raw_get(meta::size_t<1>{}, v);
      (void)d;
    }
    {
      const double& d = __variant::raw_get(meta::size_t<1>{}, const_cast<const decltype(v)&>(v));
      (void)d;
    }
    {
      double&& d = __variant::raw_get(meta::size_t<1>{}, move(v));
      (void)d;
    }
    d = 3.14;
    CHECK(__variant::raw_get(meta::size_t<1>{}, v) == 3.14);
  }

  {
    nontrivial& n = __variant::raw_get(meta::size_t<2>{}, v);
    ::new(&n) nontrivial;
    {
      const nontrivial& n = __variant::raw_get(meta::size_t<2>{}, v);
      (void)n;
    }
    {
      const nontrivial& n = __variant::raw_get(meta::size_t<2>{}, const_cast<const decltype(v)&>(v));
      (void)n;
    }
    {
      nontrivial&& n = __variant::raw_get(meta::size_t<2>{}, move(v));
      (void)n;
    }
    n.~nontrivial();
  }

  {
    int value = 42;
    reference_wrapper<int>& r = __variant::raw_get(meta::size_t<3>{}, v);
    ::new(&r) reference_wrapper<int>{value};
    int& i = __variant::cook<int&>(r);
    CHECK(i == 42);
    CHECK(&i == &value);
    reference_wrapper<int>&& rr = __variant::raw_get(meta::size_t<3>{}, move(v));
    int&& ii = __variant::cook<int&&>(move(rr));
    CHECK(&ii == &value);
    r.~reference_wrapper();
  }
}

void test_construction() {
  {
    nontrivial::create_count = 0;
    nontrivial::destroy_count = 0;
    {
      variant<nontrivial> v;
      CHECK(v.index() == std::size_t{0});
      CHECK(holds_alternative<nontrivial>(v));
      CHECK(nontrivial::create_count == 1);
    }
    CHECK(nontrivial::destroy_count == 1);
  }
  {
    variant<nontrivial, int, double> vnid;
    CHECK(vnid.index() == std::size_t{0});
    CHECK(holds_alternative<nontrivial>(vnid));
    static_assert(!is_trivially_destructible<variant<int, double, nontrivial>>());
  }
  {
    nontrivial::destroy_count = 0;
    variant<nontrivial>{};
    CHECK(nontrivial::destroy_count == 1);
    variant<nontrivial, int, double>{};
    CHECK(nontrivial::destroy_count == 2);
  }
  {
    //variant<int, int> v{42}; // ill-formed: ambiguous.
    static_assert(!models::constructible<variant<int,int>, int&&>());
    variant<int, int> v{emplaced_index<1>, 42};
    CHECK(v.index() == std::size_t{1});
    //get<int>(v); // ill-formed: ambiguous.
    //CHECK(holds_alternative<int>(v)); // ill-formed: ambiguous.
  }
  {
    int i = 42;
    double d = 3.14;
    variant<int&, double&> vi{i};
    CHECK(holds_alternative<int&>(vi));
    variant<int&, double&> vd{d};
    CHECK(holds_alternative<double&>(vd));
    variant<int&&, double&&> vri{move(i)};
    CHECK(holds_alternative<int&&>(vri));
    variant<int&&, double&&> vrd{move(d)};
    CHECK(holds_alternative<double&&>(vrd));
  }
  {
    int i = 42;
    {
      variant<int> vi{i};
      CHECK(holds_alternative<int>(vi));
      variant<int&> vir{i};
      CHECK(holds_alternative<int&>(vir));
      variant<int&&> virr{move(i)};
      CHECK(holds_alternative<int&&>(virr));
    }
    // variant<int, int&> v1{i}; // ill-formed: ambiguous
    static_assert(!models::constructible<variant<int,int&>, int&>());
    variant<int, int&> v2{move(i)};
    // variant<int, int&&> v3{move(i)}; // ill-formed: ambiguous
    static_assert(!models::constructible<variant<int,int&&>, int&&>());
    variant<int, int&&> v4{i};
  }
  {
    moveonly m{};
    auto const& cm = m;
    // variant<moveonly, const moveonly&> v1{move(m)}; // ill-formed: ambiguous
    static_assert(!models::constructible<variant<moveonly,const moveonly&>, moveonly&&>());
    variant<moveonly, moveonly&> v2{m};
    CHECK(holds_alternative<moveonly&>(v2));
    variant<moveonly, const moveonly&> v3{m};
    CHECK(holds_alternative<const moveonly&>(v3));
    variant<moveonly, const moveonly&> v4{cm};
    CHECK(holds_alternative<const moveonly&>(v4));
    variant<moveonly, moveonly&> v5{moveonly{}};
    CHECK(holds_alternative<moveonly>(v5));
  }

  {
    variant<int&&> v{emplaced_index<0>, 42};
    CHECK(holds_alternative<int&&>(v));
    variant<int, int, int, int&&> v2{emplaced_index<3>, 42};
    CHECK(holds_alternative<int&&>(v2));
    variant<int&&, int&&, int&&, int&&> v3{emplaced_index<2>, 42};
    CHECK(v3.index() == std::size_t{2});
  }
}

void test_get() {
  {
    variant<int, double> v{42};
    CHECK(holds_alternative<int>(v));
    CHECK(get<0>(v) == 42);
    CHECK(get<int>(v) == 42);
    get<0>(v) = 13;
    CHECK(get<0>(v) == 13);
    CHECK(get<int>(v) == 13);
  }
  {
    variant<int, double> v{3.14};
    CHECK(holds_alternative<double>(v));
    CHECK(get<1>(v) == 3.14);
    CHECK(get<double>(v) == 3.14);
    get<double>(v) = 1.414;
    CHECK(get<1>(v) == 1.414);
    CHECK(get<double>(v) == 1.414);
  }
  {
    variant<int, int> v{emplaced_index<1>, 42};
    CHECK(v.index() == std::size_t{1});
    CHECK(get<1>(v) == 42);
    //get<int>(v); // ill-formed: not unique.
  }
}

int main() {
  test_raw();
  test_construction();
  test_get();

  {
    static_assert(!models::default_constructible<variant<>>());
    static_assert(!models::default_constructible<variant<int&>>());
  }

  {
    int i = 42;
    variant<int&> vir{meta::size_t<0>{}, i};
    CHECK(vir.index() == std::size_t{0});
    CHECK(holds_alternative<int&>(vir));
    CHECK(&get<0>(vir) == &i);
    CHECK(&get<int&>(vir) == &i);
    static_assert(is_trivially_destructible<variant<int&>>());
  }

  {
    constexpr variant<int> vi{meta::size_t<0>{}, 42};
    static_assert(vi.index() == std::size_t{0});
    static_assert(holds_alternative<int>(vi));
    CHECK(get<0>(vi) == 42);
    CHECK(get<int>(vi) == 42);
    CHECK(&get<0>(vi) == &get<int>(vi));
  }
  {
    constexpr variant<double, int> vdi{meta::size_t<1>{}, 42};
    static_assert(vdi.index() == 1);
    static_assert(holds_alternative<int>(vdi));
    static_assert(is_trivially_destructible<variant<int, double>>());
  }

  {
    static_assert(is_trivially_copyable<variant<int, double>>());
    static_assert(is_trivially_move_constructible<variant<int, double>>());
    variant<int, double>{variant<int, double>{}};
  }

  {
    static_assert(!is_trivially_copyable<variant<int, double, nontrivial>>());
    static_assert(is_copy_constructible<variant<int, double, nontrivial>>());
    static_assert(!is_trivially_move_constructible<variant<int, double, nontrivial>>());
    nontrivial::copy_count = 0;
    variant<int, double, nontrivial> v{};
    CHECK(v.index() == std::size_t{0});
    CHECK(holds_alternative<int>(v));
    auto copy1 = v;
    CHECK(copy1.index() == std::size_t{0});
    CHECK(holds_alternative<int>(copy1));
    auto copy2 = copy1;
    CHECK(copy2.index() == std::size_t{0});
    CHECK(holds_alternative<int>(copy2));
  }

  {
    constexpr variant<int> v{meta::size_t<0>{}, 42};
    CHECK(v.index() == std::size_t{0});
    CHECK(holds_alternative<int>(v));
    constexpr variant<int> c = v;
    CHECK(c.index() == std::size_t{0});
    CHECK(holds_alternative<int>(c));
  }

  {
    nontrivial::move_count = 0;
    nontrivial::copy_count = 0;
    variant<nontrivial, int, double> v;
    auto move1 = stl2::move(v);
    auto move2 = stl2::move(move1);
    CHECK(nontrivial::move_count == 2);
    CHECK(nontrivial::copy_count == 0);
  }

  return ::test_result();
}
