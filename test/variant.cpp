#include <cassert>
#include <iostream>
#include <vector>
#include <stl2/type_traits.hpp>
#include <stl2/variant.hpp>
#include "simple_test.hpp"

using namespace stl2;
namespace models = stl2::ext::models;

template <class> class show_type;

// variants of trivially {destructible,move constructible,copy constructible} types
// are trivially {destructible,move constructible,copy constructible}.
using VDI = stl2::variant<char, short, int, long, long long,
                          float, double>;
static_assert(stl2::is_trivially_destructible<VDI>());
static_assert(stl2::is_trivially_move_constructible<VDI>());
static_assert(stl2::is_trivially_copy_constructible<VDI>());
static_assert(stl2::is_trivially_move_assignable<VDI>());
static_assert(stl2::is_trivially_copy_assignable<VDI>());

// The triviality goal ensures that trivial variants are passed by value in
// registers on x64. This function optimizes to two register moves:
VDI v_test(VDI source) {
  return source;
}

// This is two stores:
void v_assign(VDI& target, VDI source) {
  target = move(source);
}

struct nontrivial {
  struct counts {
    unsigned create;
    unsigned move;
    unsigned copy;
    unsigned move_assign;
    unsigned copy_assign;
    unsigned destroy;
  };
  static counts count;

  static void zero() {
    count = {};
  }

  ~nontrivial() { ++count.destroy; }
  nontrivial() noexcept { ++count.create; }
  nontrivial(const nontrivial&) { ++count.copy; }
  nontrivial(nontrivial&&) noexcept { ++count.move; }
  nontrivial& operator=(nontrivial&&) & noexcept { ++count.move_assign; return *this; }
  nontrivial& operator=(const nontrivial&) & { ++count.copy_assign; return *this; }
};

nontrivial::counts nontrivial::count;

struct nontrivial_literal {
  int value;

  constexpr nontrivial_literal(int v = 0) : value{v} {}
  constexpr nontrivial_literal(nontrivial_literal&& that) : value{that.value} {}
  constexpr nontrivial_literal(const nontrivial_literal& that) : value{that.value} {}
  constexpr nontrivial_literal& operator=(nontrivial_literal&& that) & {
    value = that.value;
    return *this;
  }
  constexpr nontrivial_literal& operator=(const nontrivial_literal& that) & {
    value = that.value;
    return *this;
  }

  constexpr operator int() const {
    return value;
  }

  friend std::ostream& operator<<(std::ostream& os, const nontrivial_literal& ntl) {
    return os << "nontrivial_literal{" << ntl.value << '}';
  }
};

struct moveonly {
  moveonly() = default;
  moveonly(moveonly&&) = default;
  moveonly& operator=(moveonly&&) & = default;
};

struct nonmovable {
  nonmovable() = default;
  nonmovable(nonmovable&&) = delete;
  nonmovable& operator=(nonmovable&&) & = delete;
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

void test_emplaced_index() {
  {
    using V = variant<int, int&, int&&, const int, const int&, const int&&>;
    int i = 42;
    const int& ci = i;

    {
      V v{emplaced_index<0>, i};
      CHECK(v.index() == std::size_t{0});
    }
    {
      V v{emplaced_index<0>, ci};
      CHECK(v.index() == std::size_t{0});
    }
    {
      V v{emplaced_index<0>, move(i)};
      CHECK(v.index() == std::size_t{0});
    }
    {
      V v{emplaced_index<0>, move(ci)};
      CHECK(v.index() == std::size_t{0});
    }

    {
      V v{emplaced_index<1>, i};
      CHECK(v.index() == std::size_t{1});
    }
    {
      // V v{emplaced_index<1>, ci}; // ill-formed
      static_assert(!models::constructible<V, emplaced_index_t<1>, const int&>());
    }
    {
      // V v{emplaced_index<1>, move(i)}; // ill-formed
      static_assert(!models::constructible<V, emplaced_index_t<1>, int&&>());
    }
    {
      // V v{emplaced_index<1>, move(ci)}; // ill-formed
      static_assert(!models::constructible<V, emplaced_index_t<1>, const int&&>());
    }

    {
      // V v{emplaced_index<2>, i}; // ill-formed
      static_assert(!models::constructible<V, emplaced_index_t<2>, int&>());
    }
    {
      // V v{emplaced_index<2>, ci}; // ill-formed
      static_assert(!models::constructible<V, emplaced_index_t<2>, const int&>());
    }
    {
      V v{emplaced_index<2>, move(i)};
      CHECK(v.index() == std::size_t{2});
    }
    {
      // V v{emplaced_index<2>, move(ci)}; // ill-formed
      static_assert(!models::constructible<V, emplaced_index_t<2>, const int&&>());
    }

    {
      V v{emplaced_index<3>, i};
      CHECK(v.index() == std::size_t{3});
    }
    {
      V v{emplaced_index<3>, ci};
      CHECK(v.index() == std::size_t{3});
    }
    {
      V v{emplaced_index<3>, move(i)};
      CHECK(v.index() == std::size_t{3});
    }
    {
      V v{emplaced_index<3>, move(ci)};
      CHECK(v.index() == std::size_t{3});
    }

    {
      V v{emplaced_index<4>, i};
      CHECK(v.index() == std::size_t{4});
    }
    {
      V v{emplaced_index<4>, ci};
      CHECK(v.index() == std::size_t{4});
    }
    {
      V v{emplaced_index<4>, move(i)};
      CHECK(v.index() == std::size_t{4});
    }
    {
      V v{emplaced_index<4>, move(ci)};
      CHECK(v.index() == std::size_t{4});
    }

    {
      // V v{emplaced_index<5>, i}; // ill-formed
      static_assert(!models::constructible<V, emplaced_index_t<5>, int&>());
    }
    {
      // V v{emplaced_index<5>, ci}; // ill-formed
      static_assert(!models::constructible<V, emplaced_index_t<5>, const int&>());
    }
    {
      V v{emplaced_index<5>, move(i)};
      CHECK(v.index() == std::size_t{5});
    }
    {
      V v{emplaced_index<5>, move(ci)};
      CHECK(v.index() == std::size_t{5});
    }
  }

  {
    nontrivial::zero();
    {
      using V = variant<std::vector<nontrivial>>;
      V v{emplaced_index<0>, 4u};
      CHECK(v.index() == 0u);
      CHECK(get<0>(v).size() == 4u);
      CHECK(nontrivial::count.create == 4u);
      CHECK(nontrivial::count.move == 0u);
      CHECK(nontrivial::count.copy == 0u);
      CHECK(nontrivial::count.destroy == 0u);
      get<0>(v)[0];
    }
    CHECK(nontrivial::count.create == 4u);
    CHECK(nontrivial::count.move == 0u);
    CHECK(nontrivial::count.copy == 0u);
    CHECK(nontrivial::count.destroy == 4u);
  }

  {
    using V = variant<std::vector<int>>;
    V v{emplaced_index<0>, {1,2,3,4}};
    CHECK(v.index() == 0u);
    CHECK(get<0>(v).size() == 4u);
    CHECK(get<0>(v)[2] == 3);
  }
}

void test_emplaced_type() {
  {
    using V = variant<int, int&, int&&, const int, const int&, const int&&>;
    int i = 42;
    const int& ci = i;

    {
      V v{emplaced_type<int>, i};
      CHECK(holds_alternative<int>(v));
    }
    {
      V v{emplaced_type<int>, ci};
      CHECK(holds_alternative<int>(v));
    }
    {
      V v{emplaced_type<int>, move(i)};
      CHECK(holds_alternative<int>(v));
    }
    {
      V v{emplaced_type<int>, move(ci)};
      CHECK(holds_alternative<int>(v));
    }

    {
      V v{emplaced_type<int&>, i};
      CHECK(holds_alternative<int&>(v));
    }
    {
      // V v{emplaced_type<int&>, ci}; // ill-formed
      static_assert(!models::constructible<V, emplaced_type_t<int&>, const int&>());
    }
    {
      // V v{emplaced_type<int&>, move(i)}; // ill-formed
      static_assert(!models::constructible<V, emplaced_type_t<int&>, int&&>());
    }
    {
      // V v{emplaced_type<int&>, move(ci)}; // ill-formed
      static_assert(!models::constructible<V, emplaced_type_t<int&>, const int&&>());
    }

    {
      // V v{emplaced_type<int&&>, i}; // ill-formed
      static_assert(!models::constructible<V, emplaced_type_t<int&&>, int&>());
    }
    {
      // V v{emplaced_type<int&&>, ci}; // ill-formed
      static_assert(!models::constructible<V, emplaced_type_t<int&&>, const int&>());
    }
    {
      V v{emplaced_type<int&&>, move(i)};
      CHECK(holds_alternative<int&&>(v));
    }
    {
      // V v{emplaced_type<int&&>, move(ci)}; // ill-formed
      static_assert(!models::constructible<V, emplaced_type_t<int&&>, const int&&>());
    }

    {
      V v{emplaced_type<const int>, i};
      CHECK(holds_alternative<const int>(v));
    }
    {
      V v{emplaced_type<const int>, ci};
      CHECK(holds_alternative<const int>(v));
    }
    {
      V v{emplaced_type<const int>, move(i)};
      CHECK(holds_alternative<const int>(v));
    }
    {
      V v{emplaced_type<const int>, move(ci)};
      CHECK(holds_alternative<const int>(v));
    }

    {
      V v{emplaced_type<const int&>, i};
      CHECK(holds_alternative<const int&>(v));
    }
    {
      V v{emplaced_type<const int&>, ci};
      CHECK(holds_alternative<const int&>(v));
    }
    {
      V v{emplaced_type<const int&>, move(i)};
      CHECK(holds_alternative<const int&>(v));
    }
    {
      V v{emplaced_type<const int&>, move(ci)};
      CHECK(holds_alternative<const int&>(v));
    }

    {
      // V v{emplaced_type<const int&&>, i}; // ill-formed
      static_assert(!models::constructible<V, emplaced_type_t<const int&&>, int&>());
    }
    {
      // V v{emplaced_type<const int&&>, ci}; // ill-formed
      static_assert(!models::constructible<V, emplaced_type_t<const int&&>, const int&>());
    }
    {
      V v{emplaced_type<const int&&>, move(i)};
      CHECK(holds_alternative<const int&&>(v));
    }
    {
      V v{emplaced_type<const int&&>, move(ci)};
      CHECK(holds_alternative<const int&&>(v));
    }
  }

  {
    nontrivial::zero();
    {
      using V = variant<std::vector<nontrivial>>;
      V v{emplaced_type<std::vector<nontrivial>>, 4u};
      CHECK(holds_alternative<std::vector<nontrivial>>(v));
      CHECK(get<0>(v).size() == 4u);
      CHECK(nontrivial::count.create == 4u);
      CHECK(nontrivial::count.move == 0u);
      CHECK(nontrivial::count.copy == 0u);
      CHECK(nontrivial::count.destroy == 0u);
      get<0>(v)[0];
    }
    CHECK(nontrivial::count.create == 4u);
    CHECK(nontrivial::count.move == 0u);
    CHECK(nontrivial::count.copy == 0u);
    CHECK(nontrivial::count.destroy == 4u);
  }

  {
    using V = variant<std::vector<int>>;
    V v{emplaced_type<std::vector<int>>, {1,2,3,4}};
    CHECK(holds_alternative<std::vector<int>>(v));
    CHECK(get<0>(v).size() == 4u);
    CHECK(get<0>(v)[2] == 3);
  }

  {
    using V = variant<int, int>;
    // V v1{emplaced_type<int>}; // ill-formed: ambiguous
    static_assert(!models::constructible<V, emplaced_type_t<int>>());
    // V v2{emplaced_type<int>, 42}; // ill-formed: ambiguous
    static_assert(!models::constructible<V, emplaced_type_t<int>, int>());
  }
}

void test_construction() {
  {
    nontrivial::zero();
    {
      variant<nontrivial> v;
      CHECK(v.index() == 0u);
      CHECK(holds_alternative<nontrivial>(v));
      CHECK(nontrivial::count.create == 1u);
    }
    CHECK(nontrivial::count.destroy == 1u);
  }
  {
    variant<nontrivial, int, double> vnid;
    CHECK(vnid.index() == 0u);
    CHECK(holds_alternative<nontrivial>(vnid));
    static_assert(!is_trivially_destructible<variant<int, double, nontrivial>>());
  }
  {
    nontrivial::zero();
    variant<nontrivial>{};
    CHECK(nontrivial::count.destroy == 1u);
    variant<nontrivial, int, double>{};
    CHECK(nontrivial::count.destroy == 2u);
  }
  {
    //variant<int, int> v{42}; // ill-formed: ambiguous.
    static_assert(!models::constructible<variant<int,int>, int&&>());
    variant<int, int> v{emplaced_index<1>, 42};
    CHECK(v.index() == 1u);
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

  test_emplaced_index();
  test_emplaced_type();
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

void test_move_assignment() {
  {
    // trivial
    using V = variant<int, double, int&>;
    static_assert(is_trivially_move_assignable<V>());
    V v;
    CHECK(holds_alternative<int>(v));
    CHECK(get<int>(v) == 0);
    v = V{42};
    CHECK(holds_alternative<int>(v));
    CHECK(get<int>(v) == 42);
    v = V{3.14};
    CHECK(holds_alternative<double>(v));
    CHECK(get<double>(v) == 3.14);
    v = V{42};
    CHECK(holds_alternative<int>(v));
    CHECK(get<int>(v) == 42);
    int i = 42;
    v = V{emplaced_type<int&>, i};
    CHECK(holds_alternative<int&>(v));
    CHECK(get<int&>(v) == 42);
    CHECK(&get<int&>(v) == &i);
    int j = 13;
    v = V{emplaced_type<int&>, j};
    CHECK(holds_alternative<int&>(v));
    CHECK(get<int&>(v) == 13);
    CHECK(&get<int&>(v) == &j);
  }
  {
    // nontrivial
    using V = variant<int, double, int&, nontrivial>;
    static_assert(!is_trivially_move_assignable<V>());
    V v;
    CHECK(holds_alternative<int>(v));
    CHECK(get<int>(v) == 0);
    v = V{42};
    CHECK(holds_alternative<int>(v));
    CHECK(get<int>(v) == 42);
    v = V{3.14};
    CHECK(holds_alternative<double>(v));
    CHECK(get<double>(v) == 3.14);
    v = V{42};
    CHECK(holds_alternative<int>(v));
    CHECK(get<int>(v) == 42);
    int i = 42;
    v = V{emplaced_type<int&>, i};
    CHECK(holds_alternative<int&>(v));
    CHECK(get<int&>(v) == 42);
    CHECK(&get<int&>(v) == &i);
    int j = 13;
    v = V{emplaced_type<int&>, j};
    CHECK(holds_alternative<int&>(v));
    CHECK(get<int&>(v) == 13);
    CHECK(&get<int&>(v) == &j);

    nontrivial::zero();
    v = V{emplaced_type<nontrivial>};
    CHECK(holds_alternative<nontrivial>(v));
    CHECK(nontrivial::count.move == 1u);
    CHECK(nontrivial::count.move_assign == 0u);
    v = V{emplaced_type<nontrivial>};
    CHECK(holds_alternative<nontrivial>(v));
    CHECK(nontrivial::count.move == 1u);
    CHECK(nontrivial::count.move_assign == 1u);
  }

  {
    // Really nontrivial
    using V = variant<int, double, std::vector<nontrivial>>;
    constexpr auto N = std::size_t{42};
    nontrivial::zero();
    V v{emplaced_index<2>, N};
    CHECK(v.index() == 2u);
    CHECK(get<2>(v).size() == N);
    CHECK(nontrivial::count.create == N);
    V v2;
    CHECK(v2.index() == 0u);
    v2 = move(v);
    CHECK(v2.index() == 2u);
    CHECK(get<2>(v).empty());
    CHECK(get<2>(v2).size() == N);
    v = move(v2);
    CHECK(get<2>(v2).empty());
    CHECK(get<2>(v).size() == N);
  }
}

void test_void() {
  {
    using V = variant<int, void, double>;
    static_assert(is_trivially_destructible<V>());
    static_assert(is_trivially_move_constructible<V>());
    static_assert(is_trivially_copy_constructible<V>());
    static_assert(is_trivially_move_assignable<V>());
    static_assert(is_trivially_copy_assignable<V>());
    V v1{42};
    V{3.14};
    V{emplaced_index<0>};
    V{emplaced_index<2>};
    // V{emplaced_type<void>}; // ill-formed
    static_assert(!models::constructible<V, emplaced_type_t<void>>());
    // V{emplaced_index<1>}; // ill-formed
    static_assert(!models::constructible<V, emplaced_index_t<1>>());

    static_assert(sizeof(variant<char>) == sizeof(variant<char, void>));
  }
}

struct print_fn {
  stl2::reference_wrapper<std::ostream> os;

  int operator()(const auto& t) const {
    os.get() << t << '\n';
    return 1;
  }
  int operator()(const auto& first, const auto&...rest) const {
    os.get() << first << ", ";
    return 1 + (*this)(stl2::forward<decltype(rest)>(rest)...);
  }
};

void test_visit() {
  using V = variant<int, double, /* FIXME: void, */ nontrivial_literal>;
  auto print = print_fn{std::cout};

  CHECK(visit(print, V{42}) == 1);
  CHECK(visit(print, V{3.14}) == 1);
  CHECK(visit(print, V{nontrivial_literal{13}}) == 1);

  CHECK(visit(print, V{42}, V{13}) == 2);
  CHECK(visit(print, V{42}, V{3.14}) == 2);
  CHECK(visit(print, V{42}, V{nontrivial_literal{42}}) == 2);
  CHECK(visit(print, V{3.14}, V{1.414}) == 2);
  CHECK(visit(print, V{3.14}, V{42}) == 2);
  CHECK(visit(print, V{3.14}, V{nontrivial_literal{42}}) == 2);
  CHECK(visit(print, V{nontrivial_literal{42}}, V{13}) == 2);
  CHECK(visit(print, V{nontrivial_literal{42}}, V{3.14}) == 2);

  CHECK(visit(print, V{42}, V{3.14}, V{nontrivial_literal{42}}) == 3);

#if 0
  CHECK(visit(print, V{0}) == 1);
  CHECK(visit(print, V{0}, V{1}) == 2);
  CHECK(visit(print, V{0}, V{1}, V{2}) == 3);
  CHECK(visit(print, V{0}, V{1}, V{2}, V{3}) == 4);
  CHECK(visit(print, V{0}, V{1}, V{2}, V{3}, V{4}) == 5);
  CHECK(visit(print, V{0}, V{1}, V{2}, V{3}, V{4}, V{5}) == 6);
  CHECK(visit(print, V{0}, V{1}, V{2}, V{3}, V{4}, V{5}, V{6}) == 7);
  CHECK(visit(print, V{0}, V{1}, V{2}, V{3}, V{4}, V{5}, V{6}, V{7}) == 8);
  CHECK(visit(print, V{0}, V{1}, V{2}, V{3}, V{4}, V{5}, V{6}, V{7}, V{8}) == 9);
#endif
}

void test_tagged() {
  using V = tagged_variant<tag::in(int), tag::out(double)>;
  static_assert(is_trivially_destructible<V>());
  static_assert(is_trivially_move_constructible<V>());
  static_assert(is_trivially_copy_constructible<V>());
  V v1{42};
  CHECK(get<0>(v1) == 42);
  CHECK(v1.in() == 42);
  V v2{3.14};
  CHECK(get<1>(v2) == 3.14);
  CHECK(v2.out() == 3.14);
}

int main() {
  test_raw();
  test_construction();
  test_get();
  test_move_assignment();

  {
    // variant<>{}; // ill-formed
    static_assert(!models::default_constructible<variant<>>());
    static_assert(!models::default_constructible<variant<int&>>());
    static_assert(models::move_constructible<variant<int, double, nontrivial>>());
    static_assert(models::move_constructible<variant<int, double, moveonly>>());
    // variant<nonmovable>{variant<nonmovable>{}}; // ill-formed
    static_assert(!models::move_constructible<variant<nonmovable>>());
    static_assert(models::copy_constructible<variant<int, double, nontrivial>>());
    // { variant<moveonly> v; auto c = v; } // ill-formed
    static_assert(!models::copy_constructible<variant<moveonly>>());
    // { variant<nonmovable> v; auto c = v; } // ill-formed
    static_assert(!models::copy_constructible<variant<nonmovable>>());
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
    nontrivial::count.copy = 0;
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
    nontrivial::count.move = 0u;
    nontrivial::count.copy = 0u;
    variant<nontrivial, int, double> v;
    auto move1 = stl2::move(v);
    auto move2 = stl2::move(move1);
    CHECK(nontrivial::count.move == 2u);
    CHECK(nontrivial::count.copy == 0u);
  }

  {
    using V = variant<nontrivial_literal>;
    constexpr V v1{nontrivial_literal{42}};
    // constexpr V v2 = move(v1); // FIXME: this seems to be non-implementable.
    // constexpr V v3 = v2; // FIXME: this seems to be non-implementable.
  }

  test_void();
  test_visit();
  test_tagged();

  return ::test_result();
}

#if 0
template <class T>
constexpr bool is_variant = is_variant<__uncvref<T>>;
template <_Unqual T>
constexpr bool is_variant<T> = false;
template <class...Types>
constexpr bool is_variant<variant<Types...>> = true;

template <class T>
concept bool IsVariant = _Is<is_variant>;

void f(char);
void f(short);
void f(int);
void f(char, char);
void f(short, char);
void f(int, char);
void f(char, short);
void f(short, short);
void f(int, short);
void f(char, int);
void f(short, int);
void f(int, int);

using VV = variant<char, short, int>;

void test_foo(VV a) {
  visit([](auto&& t){ f(t); }, a);
}

void test_foo(VV a, VV b) {
  visit([](auto&& t, auto&& u){ f(t, u); }, a, b);
}

void test_bar() {
  test_foo(VV{42}, VV{'a'});
}
#endif
