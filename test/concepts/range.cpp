#include "validate.hpp"

#if VALIDATE_RANGES
#include <range/v3/utility/concepts.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>

#define NAMESPACE ranges

namespace ns {
template <class R>
using IteratorType = ranges::range_iterator_t<R>;

template <class R>
using SentinelType = ranges::range_sentinel_t<R>;

using ranges::size;
using ranges::view_base;
}

namespace models {
template <class T, class U>
using same = ranges::Same<T, U>;

template <class R>
using range = ranges::Range<R>;

template <class R>
using sized_range = ranges::SizedRange<R>;

template <class R>
using sized_range_like = ranges::SizedRangeLike_<R>;

template <class R>
using container_like = ranges::ContainerLike_<R>;

template <class R>
using view = ranges::View<R>;

template <class R>
using bounded_range = ranges::BoundedRange<R>;
}

template <bool allow_nonconst, bool allow_const, bool allow_size>
struct arbitrary_range {
  CONCEPT_REQUIRES(allow_nonconst)
  int* begin();
  CONCEPT_REQUIRES(allow_nonconst)
  int* end();
  CONCEPT_REQUIRES(allow_const)
  const int* begin() const;
  CONCEPT_REQUIRES(allow_const)
  const int* end() const;

  CONCEPT_REQUIRES(allow_size)
  int size() const;
};

#elif VALIDATE_STL2
#include <stl2/iterator.hpp>

#define NAMESPACE stl2

namespace ns {
using stl2::DifferenceType;
using stl2::IteratorType;
using stl2::SentinelType;

using stl2::size;
using stl2::view_base;
}

template <bool allow_nonconst, bool allow_const, bool allow_size>
struct arbitrary_range {
  int* begin() requires allow_nonconst;
  int* end() requires allow_nonconst;
  const int* begin() const requires allow_const;
  const int* end() const requires allow_const;

  int size() const requires allow_size;
};
#endif

#include "../simple_test.hpp"
#include <vector>

using mutable_unsized_range =
  arbitrary_range<true, true, false>;

using mutable_only_unsized_range =
  arbitrary_range<true, false, false>;

using immutable_unsized_range =
  arbitrary_range<false, true, false>;

using mutable_sized_range =
  arbitrary_range<true, true, true>;

using mutable_only_sized_range =
  arbitrary_range<true, false, true>;

using immutable_sized_range =
  arbitrary_range<false, true, true>;

// size() launches the missiles.
struct mutable_badsized_range :
  mutable_sized_range {};

struct mutable_only_badsized_range :
  mutable_only_sized_range {};

struct immutable_badsized_range :
  immutable_sized_range {};

#if VALIDATE_STL2
namespace stl2 {
template <>
struct disable_sized_range<mutable_badsized_range> :
  std::true_type {};
template <>
struct disable_sized_range<mutable_only_badsized_range> :
  std::true_type {};
template <>
struct disable_sized_range<immutable_badsized_range> :
  std::true_type {};
}

#elif VALIDATE_RANGES
namespace ranges {
template <>
struct is_sized_range<mutable_badsized_range> :
  std::false_type {};
template <>
struct is_sized_range<mutable_only_badsized_range> :
  std::false_type {};
template <>
struct is_sized_range<immutable_badsized_range> :
  std::false_type {};
}
#endif

struct strange_view
{
  std::vector<int>::iterator begin();
  std::vector<int>::const_iterator begin() const;

  std::vector<int>::iterator end();
  std::vector<int>::const_iterator end() const;
};

struct strange_view2 : strange_view, ns::view_base {};
struct strange_view3 : strange_view2 {};

#if VALIDATE_STL2
namespace stl2 {
  template <>
  struct enable_view<strange_view> : std::true_type {};
  template <>
  struct enable_view<strange_view3> : std::false_type {};
}

#elif VALIDATE_RANGES
namespace ranges {
  template <>
  struct is_view<strange_view> : std::true_type {};
  template <>
  struct is_view<strange_view3> : std::false_type {};
}
#endif

void ridiculously_exhaustive_range_property_test() {
  CHECK(!models::range<void>());
  CHECK(!models::sized_range<void>());
  CHECK(!models::container_like<void>());
  CHECK(!models::view<void>());

  using I = int*;
  using CI = const int*;
  
  static_assert(stl2::ext::models::iterator<I>());
  static_assert(stl2::ext::models::iterator<CI>());

  CHECK(models::same<ns::IteratorType<int[2]>, I>());
  CHECK(models::same<ns::SentinelType<int[2]>, I>());
  CHECK(models::range<int[2]>());
  CHECK(models::sized_range<int[2]>());
  CHECK(models::container_like<int[2]>());
  CHECK(!models::view<int[2]>());

  CHECK(models::same<ns::IteratorType<int(&)[2]>, I>());
  CHECK(models::same<ns::SentinelType<int(&)[2]>, I>());
  CHECK(models::range<int(&)[2]>());
  CHECK(models::sized_range<int(&)[2]>());
  CHECK(!models::container_like<int(&)[2]>());
  CHECK(!models::view<int(&)[2]>());

  CHECK(models::same<ns::IteratorType<const int[2]>, CI>());
  CHECK(models::same<ns::SentinelType<const int[2]>, CI>());
  CHECK(models::range<const int[2]>());
  CHECK(models::sized_range<const int[2]>());
  CHECK(!models::container_like<const int[2]>());
  CHECK(!models::view<const int[2]>());

  CHECK(models::same<ns::IteratorType<const int(&)[2]>, CI>());
  CHECK(models::same<ns::SentinelType<const int(&)[2]>, CI>());
  CHECK(models::range<const int(&)[2]>());
  CHECK(models::sized_range<const int(&)[2]>());
  CHECK(!models::container_like<const int(&)[2]>());
  CHECK(!models::view<const int(&)[2]>());


  CHECK(models::same<ns::IteratorType<mutable_unsized_range>, I>());
  CHECK(models::same<ns::SentinelType<mutable_unsized_range>, I>());
  CHECK(models::range<mutable_unsized_range>());
  CHECK(!models::sized_range<mutable_unsized_range>());
  CHECK(models::container_like<mutable_unsized_range>());
  CHECK(!models::view<mutable_unsized_range>());

  CHECK(models::same<ns::IteratorType<mutable_unsized_range&>, I>());
  CHECK(models::same<ns::SentinelType<mutable_unsized_range&>, I>());
  CHECK(models::range<mutable_unsized_range&>());
  CHECK(!models::sized_range<mutable_unsized_range&>());
  CHECK(models::container_like<mutable_unsized_range>());
  CHECK(!models::view<mutable_unsized_range>());

  CHECK(models::same<ns::IteratorType<const mutable_unsized_range>, CI>());
  CHECK(models::same<ns::SentinelType<const mutable_unsized_range>, CI>());
  CHECK(models::range<const mutable_unsized_range>());
  CHECK(!models::sized_range<const mutable_unsized_range>());
  CHECK(!models::container_like<const mutable_unsized_range>());
  CHECK(!models::view<const mutable_unsized_range>());

  CHECK(models::same<ns::IteratorType<const mutable_unsized_range&>, CI>());
  CHECK(models::same<ns::SentinelType<const mutable_unsized_range&>, CI>());
  CHECK(models::range<const mutable_unsized_range&>());
  CHECK(!models::sized_range<const mutable_unsized_range&>());
  CHECK(!models::container_like<const mutable_unsized_range&>());
  CHECK(!models::view<const mutable_unsized_range&>());


  CHECK(models::same<ns::IteratorType<mutable_only_unsized_range&>, I>());
  CHECK(models::same<ns::SentinelType<mutable_only_unsized_range&>, I>());
  CHECK(models::range<mutable_only_unsized_range>());
  CHECK(!models::sized_range<mutable_only_unsized_range>());
  CHECK(!models::container_like<mutable_only_unsized_range>());
  CHECK(models::view<mutable_only_unsized_range>());

  CHECK(models::same<ns::IteratorType<mutable_only_unsized_range&>, I>());
  CHECK(models::same<ns::SentinelType<mutable_only_unsized_range&>, I>());
  CHECK(models::range<mutable_only_unsized_range&>());
  CHECK(!models::sized_range<mutable_only_unsized_range&>());
  CHECK(!models::container_like<mutable_only_unsized_range&>());
  CHECK(!models::view<mutable_only_unsized_range&>());

  CHECK(!models::range<const mutable_only_unsized_range>());
  CHECK(!models::sized_range<const mutable_only_unsized_range>());
  CHECK(!models::container_like<const mutable_only_unsized_range>());
  CHECK(!models::view<const mutable_only_unsized_range>());

  CHECK(!models::range<const mutable_only_unsized_range&>());
  CHECK(!models::sized_range<const mutable_only_unsized_range&>());
  CHECK(!models::container_like<const mutable_only_unsized_range&>());
  CHECK(!models::view<const mutable_only_unsized_range&>());


  CHECK(models::same<ns::IteratorType<immutable_unsized_range>, CI>());
  CHECK(models::same<ns::SentinelType<immutable_unsized_range>, CI>());
  CHECK(models::range<immutable_unsized_range>());
  CHECK(!models::sized_range<immutable_unsized_range>());
  CHECK(!models::container_like<immutable_unsized_range>());
  CHECK(models::view<immutable_unsized_range>());

  CHECK(models::same<ns::IteratorType<immutable_unsized_range&>, CI>());
  CHECK(models::same<ns::SentinelType<immutable_unsized_range&>, CI>());
  CHECK(models::range<immutable_unsized_range&>());
  CHECK(!models::sized_range<immutable_unsized_range&>());
  CHECK(!models::container_like<immutable_unsized_range&>());
  CHECK(!models::view<immutable_unsized_range&>());

  CHECK(models::same<ns::IteratorType<const immutable_unsized_range>, CI>());
  CHECK(models::same<ns::SentinelType<const immutable_unsized_range>, CI>());
  CHECK(models::range<const immutable_unsized_range>());
  CHECK(!models::sized_range<const immutable_unsized_range>());
  CHECK(!models::container_like<const immutable_unsized_range>());
  CHECK(!models::view<const immutable_unsized_range>());

  CHECK(models::same<ns::IteratorType<const immutable_unsized_range&>, CI>());
  CHECK(models::same<ns::SentinelType<const immutable_unsized_range&>, CI>());
  CHECK(models::range<const immutable_unsized_range&>());
  CHECK(!models::sized_range<const immutable_unsized_range&>());
  CHECK(!models::container_like<const immutable_unsized_range&>());
  CHECK(!models::view<const immutable_unsized_range&>());


  CHECK(models::same<ns::IteratorType<mutable_sized_range>, I>());
  CHECK(models::same<ns::SentinelType<mutable_sized_range>, I>());
  CHECK(models::range<mutable_sized_range>());
  CHECK(models::sized_range<mutable_sized_range>());
  CHECK(models::container_like<mutable_sized_range>());
  CHECK(!models::view<mutable_sized_range>());

  CHECK(models::same<ns::IteratorType<mutable_sized_range&>, I>());
  CHECK(models::same<ns::SentinelType<mutable_sized_range&>, I>());
  CHECK(models::range<mutable_sized_range&>());
  CHECK(models::sized_range<mutable_sized_range&>());
  CHECK(!models::container_like<mutable_sized_range&>());
  CHECK(!models::view<mutable_sized_range&>());

  CHECK(models::same<ns::IteratorType<const mutable_sized_range>, CI>());
  CHECK(models::same<ns::SentinelType<const mutable_sized_range>, CI>());
  CHECK(models::range<const mutable_sized_range>());
  CHECK(models::sized_range<const mutable_sized_range>());
  CHECK(!models::container_like<const mutable_sized_range>());
  CHECK(!models::view<const mutable_sized_range>());

  CHECK(models::same<ns::IteratorType<const mutable_sized_range&>, CI>());
  CHECK(models::same<ns::SentinelType<const mutable_sized_range&>, CI>());
  CHECK(models::range<const mutable_sized_range&>());
  CHECK(models::sized_range<const mutable_sized_range&>());
  CHECK(!models::container_like<const mutable_sized_range&>());
  CHECK(!models::view<const mutable_sized_range&>());


  CHECK(models::same<ns::IteratorType<mutable_only_sized_range>, I>());
  CHECK(models::same<ns::SentinelType<mutable_only_sized_range>, I>());
  CHECK(models::range<mutable_only_sized_range>());
  CHECK(models::sized_range<mutable_only_sized_range>());
  CHECK(!models::container_like<mutable_only_sized_range>());
  CHECK(models::view<mutable_only_sized_range>());

  CHECK(models::same<ns::IteratorType<mutable_only_sized_range&>, I>());
  CHECK(models::same<ns::SentinelType<mutable_only_sized_range&>, I>());
  CHECK(models::range<mutable_only_sized_range&>());
  CHECK(models::sized_range<mutable_only_sized_range&>());
  CHECK(!models::container_like<mutable_only_sized_range&>());
  CHECK(!models::view<mutable_only_sized_range&>());

  CHECK(!models::range<const mutable_only_sized_range>());
  CHECK(!models::sized_range<const mutable_only_sized_range>());
  CHECK(!models::container_like<const mutable_only_sized_range>());
  CHECK(!models::view<const mutable_only_sized_range>());

  CHECK(!models::range<const mutable_only_sized_range&>());
  CHECK(!models::sized_range<const mutable_only_sized_range&>());
  CHECK(!models::container_like<const mutable_only_sized_range&>());
  CHECK(!models::view<const mutable_only_sized_range&>());


  CHECK(models::same<ns::IteratorType<immutable_sized_range>, CI>());
  CHECK(models::same<ns::SentinelType<immutable_sized_range>, CI>());
  CHECK(models::range<immutable_sized_range>());
  CHECK(models::sized_range<immutable_sized_range>());
  CHECK(!models::container_like<immutable_sized_range>());
  CHECK(models::view<immutable_sized_range>());

  CHECK(models::same<ns::IteratorType<immutable_sized_range&>, CI>());
  CHECK(models::same<ns::SentinelType<immutable_sized_range&>, CI>());
  CHECK(models::range<immutable_sized_range&>());
  CHECK(models::sized_range<immutable_sized_range&>());
  CHECK(!models::container_like<immutable_sized_range&>());
  CHECK(!models::view<immutable_sized_range&>());

  CHECK(models::same<ns::IteratorType<const immutable_sized_range>, CI>());
  CHECK(models::same<ns::SentinelType<const immutable_sized_range>, CI>());
  CHECK(models::range<const immutable_sized_range>());
  CHECK(models::sized_range<const immutable_sized_range>());
  CHECK(!models::container_like<const immutable_sized_range>());
  CHECK(!models::view<const immutable_sized_range>());

  CHECK(models::same<ns::IteratorType<const immutable_sized_range&>, CI>());
  CHECK(models::same<ns::SentinelType<const immutable_sized_range&>, CI>());
  CHECK(models::range<const immutable_sized_range&>());
  CHECK(models::sized_range<const immutable_sized_range&>());
  CHECK(!models::container_like<const immutable_sized_range&>());
  CHECK(!models::view<const immutable_sized_range&>());


  CHECK(models::same<ns::IteratorType<mutable_badsized_range>, I>());
  CHECK(models::same<ns::SentinelType<mutable_badsized_range>, I>());
  CHECK(models::range<mutable_badsized_range>());
  CHECK(!models::sized_range<mutable_badsized_range>());
  CHECK(models::container_like<mutable_badsized_range>());
  CHECK(!models::view<mutable_badsized_range>());

  CHECK(models::same<ns::IteratorType<mutable_badsized_range&>, I>());
  CHECK(models::same<ns::SentinelType<mutable_badsized_range&>, I>());
  CHECK(models::range<mutable_badsized_range&>());
  CHECK(!models::sized_range<mutable_badsized_range&>());
  CHECK(!models::container_like<mutable_badsized_range&>());
  CHECK(!models::view<mutable_badsized_range&>());

  CHECK(models::same<ns::IteratorType<const mutable_badsized_range>, CI>());
  CHECK(models::same<ns::SentinelType<const mutable_badsized_range>, CI>());
  CHECK(models::range<const mutable_badsized_range>());
  CHECK(!models::sized_range<const mutable_badsized_range>());
  CHECK(!models::container_like<const mutable_badsized_range>());
  CHECK(!models::view<const mutable_badsized_range>());

  CHECK(models::same<ns::IteratorType<const mutable_badsized_range&>, CI>());
  CHECK(models::same<ns::SentinelType<const mutable_badsized_range&>, CI>());
  CHECK(models::range<const mutable_badsized_range&>());
  CHECK(!models::sized_range<const mutable_badsized_range&>());
  CHECK(!models::container_like<const mutable_badsized_range&>());
  CHECK(!models::view<const mutable_badsized_range&>());


  CHECK(models::same<ns::IteratorType<mutable_only_badsized_range>, I>());
  CHECK(models::same<ns::SentinelType<mutable_only_badsized_range>, I>());
  CHECK(models::range<mutable_only_badsized_range>());
  CHECK(!models::sized_range<mutable_only_badsized_range>());
  CHECK(!models::container_like<mutable_only_badsized_range>());
  CHECK(models::view<mutable_only_badsized_range>());

  CHECK(models::same<ns::IteratorType<mutable_only_badsized_range&>, I>());
  CHECK(models::same<ns::SentinelType<mutable_only_badsized_range&>, I>());
  CHECK(models::range<mutable_only_badsized_range&>());
  CHECK(!models::sized_range<mutable_only_badsized_range&>());
  CHECK(!models::container_like<mutable_only_badsized_range&>());
  CHECK(!models::view<mutable_only_badsized_range&>());

  CHECK(!models::range<const mutable_only_badsized_range>());
  CHECK(!models::sized_range<const mutable_only_badsized_range>());
  CHECK(!models::container_like<const mutable_only_badsized_range>());
  CHECK(!models::view<const mutable_only_badsized_range>());

  CHECK(!models::range<const mutable_only_badsized_range&>());
  CHECK(!models::sized_range<const mutable_only_badsized_range&>());
  CHECK(!models::container_like<const mutable_only_badsized_range&>());
  CHECK(!models::view<const mutable_only_badsized_range&>());


  CHECK(models::same<ns::IteratorType<immutable_badsized_range>, CI>());
  CHECK(models::same<ns::SentinelType<immutable_badsized_range>, CI>());
  CHECK(models::range<immutable_badsized_range>());
  CHECK(!models::sized_range<immutable_badsized_range>());
  CHECK(!models::container_like<immutable_badsized_range>());
  CHECK(models::view<immutable_badsized_range>());

  CHECK(models::same<ns::IteratorType<immutable_badsized_range&>, CI>());
  CHECK(models::same<ns::SentinelType<immutable_badsized_range&>, CI>());
  CHECK(models::range<immutable_badsized_range&>());
  CHECK(!models::sized_range<immutable_badsized_range&>());
  CHECK(!models::container_like<immutable_badsized_range&>());
  CHECK(!models::view<immutable_badsized_range&>());

  CHECK(models::same<ns::IteratorType<const immutable_badsized_range>, CI>());
  CHECK(models::same<ns::SentinelType<const immutable_badsized_range>, CI>());
  CHECK(models::range<const immutable_badsized_range>());
  CHECK(!models::sized_range<const immutable_badsized_range>());
  CHECK(!models::container_like<const immutable_badsized_range>());
  CHECK(!models::view<const immutable_badsized_range>());

  CHECK(models::same<ns::IteratorType<const immutable_badsized_range&>, CI>());
  CHECK(models::same<ns::SentinelType<const immutable_badsized_range&>, CI>());
  CHECK(models::range<const immutable_badsized_range&>());
  CHECK(!models::sized_range<const immutable_badsized_range&>());
  CHECK(!models::container_like<const immutable_badsized_range&>());
  CHECK(!models::view<const immutable_badsized_range&>());


  CHECK(models::same<ns::IteratorType<std::vector<int>>, std::vector<int>::iterator>());
  CHECK(models::same<ns::SentinelType<std::vector<int>>, std::vector<int>::iterator>());
  CHECK(models::range<std::vector<int>>());
  CHECK(models::sized_range<std::vector<int>>());
  CHECK(models::container_like<std::vector<int>>());
  CHECK(!models::view<std::vector<int>>());


  CHECK(models::range<strange_view>());
  CHECK(models::range<strange_view&>());
  CHECK(models::view<strange_view>());
  CHECK(!models::view<strange_view&>());
  CHECK(!models::view<const strange_view>());

  CHECK(models::range<strange_view2>());
  CHECK(models::range<strange_view2&>());
  CHECK(models::view<strange_view2>());
  CHECK(!models::view<strange_view2&>());
  CHECK(!models::view<const strange_view2>());

  CHECK(models::range<strange_view3>());
  CHECK(models::range<strange_view3&>());
  CHECK(!models::view<strange_view3>());
  CHECK(!models::view<strange_view3&>());
  CHECK(!models::view<const strange_view3>());

  CHECK(models::range<mutable_only_unsized_range&>());
  CHECK(models::range<mutable_only_unsized_range>());
  CHECK(models::view<mutable_only_unsized_range>());
  CHECK(!models::view<mutable_only_unsized_range&>());
  CHECK(!models::view<mutable_only_unsized_range&&>());
  CHECK(!models::range<const mutable_only_unsized_range&>());
  CHECK(!models::view<const mutable_only_unsized_range&>());
}

template <stl2::InputIterator I, stl2::Sentinel<I> S>
I complicated_algorithm(I i, S s) {
  static constexpr bool output = false;
  if (output) std::cout << '{';
  if (i != s) {
    if (output) std::cout << *i;
    while (++i != s) {
      if (output) std::cout << ", " << *i;
    }
  }
  if (output) std::cout << "}\n";
  return i;
}

stl2::InputRange{R}
stl2::IteratorType<R> complicated_algorithm(R&& r) {
  using stl2::begin; using stl2::end;
  return complicated_algorithm(begin(r), end(r));
}

template <class T>
struct array_view {
  T* first_;
  std::size_t n_;

  array_view() = default;
  template <std::size_t N>
  array_view(T (&a)[N]) : first_{a}, n_{N} {}

  auto begin() const { return first_; }
  auto end() const { return first_ + n_; }
  auto size() const { return n_; }
};

void complicated_algorithm_test() {
  static int some_ints[] = {2, 3, 5, 7};
  CHECK(models::range<decltype(some_ints)>());
  CHECK(models::sized_range<decltype(some_ints)>());
  CHECK(models::container_like<decltype(some_ints)>());
  CHECK(!models::view<decltype(some_ints)>());
  CHECK(complicated_algorithm(some_ints) == std::end(some_ints));
  CHECK(models::range<array_view<int>>());
  CHECK(models::sized_range<array_view<int>>());
  CHECK(!models::container_like<array_view<int>>());
  CHECK(models::view<array_view<int>>());
  CHECK(complicated_algorithm(array_view<int>{some_ints}) == std::end(some_ints));
}

int main() {
  ridiculously_exhaustive_range_property_test();
  complicated_algorithm_test();

  {
    using T = int[2];
    CHECK(models::bounded_range<T>());
    CHECK(models::output_range<T, int>());
    CHECK(models::input_range<T>());
    CHECK(models::forward_range<T>());
    CHECK(models::bidirectional_range<T>());
    CHECK(models::random_access_range<T>());
    CHECK(models::contiguous_range<T>());
  }

  return ::test_result();
}
