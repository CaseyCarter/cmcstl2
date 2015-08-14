
#include <tuple>
#include <stl2/type_traits.hpp>
#include "simple_test.hpp"

using std::tuple;
using namespace stl2;

static_assert(is_same<common_type_t<int, short&, int, char>, int>(), "");
static_assert(!meta::has_type<common_type<int, short, int, char*>>(), "");

struct X {};
struct Y { explicit Y(X){} };

namespace stl2 {
template <> struct common_type<X, Y> { typedef Y type; };
template <> struct common_type<Y, X> { typedef Y type; };
}

static_assert(is_same<common_type_t<X, Y>, Y>(), "");    // (A)
static_assert(is_same<common_type_t<X, Y, Y>, Y>(), ""); // (B)
static_assert(is_same<common_type_t<X, X, Y>, Y>(), ""); // (C)

struct AA {
  AA() = default;
  AA(AA &&) = delete;
  AA(AA const &) = delete;
};
struct BB : AA { };

static_assert(is_same<common_type_t<AA, BB>, AA>(), ""); // (C)

static_assert(is_same<common_reference_t<int &&, int const &, int volatile &>, int const volatile &>(), "");
static_assert(is_same<common_reference_t<int &&, int const &, float &>, float>(), "");
static_assert(!meta::has_type<common_reference<int, short, int, char*>>(), "");

namespace stl2 {
template <class...T, class...U, template <class> class TQual, template <class> class UQual>
  requires meta::_v<meta::and_c<meta::_v<meta::has_type<common_reference<TQual<T>, UQual<U>>>>...>>
  //requires _Valid<common_reference_t, TQual<T>, UQual<U>> &&...
struct basic_common_reference<tuple<T...>, tuple<U...>, TQual, UQual> {
  using type = tuple<common_reference_t<TQual<T>, UQual<U>>...>;
};
}

static_assert(is_same<
  common_reference_t<const tuple<int, short> &, tuple<int&,short volatile&>>,
  tuple<const int&,const volatile short&>>(), "");

static_assert(is_same<
  common_reference_t<volatile tuple<int, short> &, const tuple<int,short>&>,
  const volatile tuple<int, short>&>(), "");

static_assert(!meta::has_type<
  common_reference<volatile tuple<short> &, const tuple<int,short>&>>(), "");

struct B {};
struct D : B {};

struct noncopyable
{
    noncopyable() = default;
    noncopyable(noncopyable const &) = delete;
    noncopyable(noncopyable &&) = default;
    noncopyable &operator=(noncopyable const &) = delete;
    noncopyable &operator=(noncopyable &&) = default;
};

struct noncopyable2 : noncopyable
{};

static_assert(is_same<common_reference_t<B &, D &>, B &>::value, "");
static_assert(is_same<common_reference_t<B &, D const &>, B const &>::value, "");
static_assert(is_same<common_reference_t<B &, D const &, D &>, B const &>::value, "");
static_assert(is_same<common_reference_t<B const &, D &>, B const &>::value, "");
static_assert(is_same<common_reference_t<B &, D &, B &, D &>, B &>::value, "");

static_assert(is_same<common_reference_t<B &&, D &&>, B &&>::value, "");
static_assert(is_same<common_reference_t<B const &&, D &&>, B const &&>::value, "");
static_assert(is_same<common_reference_t<B &&, D const &&>, B const &&>::value, "");

static_assert(is_same<common_reference_t<B &, D &&>, B const &>::value, "");
static_assert(is_same<common_reference_t<B &, D const &&>, B const &>::value, "");
static_assert(is_same<common_reference_t<B const &, D &&>, B const &>::value, "");

static_assert(is_same<common_reference_t<B &&, D &>, B const &>::value, "");
static_assert(is_same<common_reference_t<B &&, D const &>, B const &>::value, "");
static_assert(is_same<common_reference_t<B const &&, D &>, B const &>::value, "");

static_assert(is_same<common_reference_t<int, short>, int>::value, "");
static_assert(is_same<common_reference_t<int, short &>, int>::value, "");
static_assert(is_same<common_reference_t<int &, short &>, int>::value, "");
static_assert(is_same<common_reference_t<int &, short>, int>::value, "");

// tricky volatile reference case
static_assert(is_same<common_reference_t<int &&, int volatile &>, int>::value, "");
static_assert(is_same<common_reference_t<int volatile &, int &&>, int>::value, "");
static_assert(is_same<common_reference_t<int const volatile &&, int volatile &&>, int const volatile &&>::value, "");
static_assert(is_same<common_reference_t<int &&, int const &, int volatile &>, int const volatile &>(), "");

// Array types?? Yup!
static_assert(is_same<common_reference_t<int (&)[10], int (&&)[10]>, int const(&)[10]>::value, "");
static_assert(is_same<common_reference_t<int const (&)[10], int volatile (&)[10]>, int const volatile(&)[10]>::value, "");
static_assert(is_same<common_reference_t<int (&)[10], int (&)[11]>, int *>::value, "");

// Some tests with noncopyable types
static_assert(is_same<
    common_reference_t<noncopyable const &, noncopyable>,
    noncopyable
>::value, "");

static_assert(is_same<
    common_reference_t<noncopyable2 const &, noncopyable>,
    noncopyable
>::value, "");

static_assert(is_same<
    common_reference_t<noncopyable const &, noncopyable2>,
    noncopyable
>::value, "");

struct X2 {};
struct Y2 {};
struct Z2 {};

namespace stl2{
template<>
struct common_type<X2, Y2>
{
    using type = Z2;
};
template<>
struct common_type<Y2, X2>
{
    using type = Z2;
};
}

static_assert(is_same<
    common_reference_t<X2 &, Y2 const &>,
    Z2
>::value, "");

int main() {
  return ::test_result();
}
