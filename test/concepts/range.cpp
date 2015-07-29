#include "validate.hpp"

#if VALIDATE_RANGES
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>

#define NAMESPACE ranges

namespace ns {
template <class R>
using IteratorType = ranges::range_iterator_t<R>;

template <class R>
using SentinelType = ranges::range_sentinel_t<R>;

using ranges::size;
}

namespace models {
template <class R>
using range = ranges::Range<R>;

template <class R>
using sized_range = ranges::SizedRange<R>;

template <class R>
using sized_range_like = ranges::SizedRangeLike_<R>;
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
#include <stl2/detail/concepts/range.hpp>

#define NAMESPACE stl2

namespace ns {
using stl2::IteratorType;
using stl2::SentinelType;

using stl2::size;
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

namespace NAMESPACE {
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

int main() {
  CHECK(!models::range<void>());
  CHECK(!models::sized_range<void>());

  CHECK(models::range<int(&)[2]>());
  CHECK(models::sized_range_like<int(&)[2]>());
  CHECK(models::sized_range<int(&)[2]>());

  CHECK(models::range<mutable_unsized_range>());
  CHECK(models::range<mutable_unsized_range&>());
  CHECK(models::range<const mutable_unsized_range>());
  CHECK(models::range<const mutable_unsized_range&>());
  CHECK(!models::sized_range_like<mutable_unsized_range>());
  CHECK(!models::sized_range_like<mutable_unsized_range&>());
  CHECK(!models::sized_range_like<const mutable_unsized_range>());
  CHECK(!models::sized_range_like<const mutable_unsized_range&>());
  CHECK(!models::sized_range<mutable_unsized_range>());
  CHECK(!models::sized_range<mutable_unsized_range&>());
  CHECK(!models::sized_range<const mutable_unsized_range>());
  CHECK(!models::sized_range<const mutable_unsized_range&>());

  CHECK(!models::range<mutable_only_unsized_range>());
  CHECK(models::range<mutable_only_unsized_range&>());
  CHECK(!models::range<const mutable_only_unsized_range>());
  CHECK(!models::range<const mutable_only_unsized_range&>());
  CHECK(!models::sized_range_like<mutable_only_unsized_range>());
  CHECK(!models::sized_range_like<mutable_only_unsized_range&>());
  CHECK(!models::sized_range_like<const mutable_only_unsized_range>());
  CHECK(!models::sized_range_like<const mutable_only_unsized_range&>());
  CHECK(!models::sized_range<mutable_only_unsized_range>());
  CHECK(!models::sized_range<mutable_only_unsized_range&>());
  CHECK(!models::sized_range<const mutable_only_unsized_range>());
  CHECK(!models::sized_range<const mutable_only_unsized_range&>());

  CHECK(models::range<immutable_unsized_range>());
  CHECK(models::range<immutable_unsized_range&>());
  CHECK(models::range<const immutable_unsized_range>());
  CHECK(models::range<const immutable_unsized_range&>());
  CHECK(!models::sized_range_like<immutable_unsized_range>());
  CHECK(!models::sized_range_like<immutable_unsized_range&>());
  CHECK(!models::sized_range_like<const immutable_unsized_range>());
  CHECK(!models::sized_range_like<const immutable_unsized_range&>());
  CHECK(!models::sized_range<immutable_unsized_range>());
  CHECK(!models::sized_range<immutable_unsized_range&>());
  CHECK(!models::sized_range<const immutable_unsized_range>());
  CHECK(!models::sized_range<const immutable_unsized_range&>());

  CHECK(models::range<mutable_sized_range>());
  CHECK(models::range<mutable_sized_range&>());
  CHECK(models::range<const mutable_sized_range>());
  CHECK(models::range<const mutable_sized_range&>());
  CHECK(models::sized_range_like<mutable_sized_range>());
  CHECK(models::sized_range_like<mutable_sized_range&>());
  CHECK(models::sized_range_like<const mutable_sized_range>());
  CHECK(models::sized_range_like<const mutable_sized_range&>());
  CHECK(models::sized_range<mutable_sized_range>());
  CHECK(models::sized_range<mutable_sized_range&>());
  CHECK(models::sized_range<const mutable_sized_range>());
  CHECK(models::sized_range<const mutable_sized_range&>());

  CHECK(!models::range<mutable_only_sized_range>());
  CHECK(models::range<mutable_only_sized_range&>());
  CHECK(!models::range<const mutable_only_sized_range>());
  CHECK(!models::range<const mutable_only_sized_range&>());
  CHECK(models::sized_range_like<mutable_only_sized_range>());
  CHECK(models::sized_range_like<mutable_only_sized_range&>());
  CHECK(!models::sized_range_like<const mutable_only_sized_range>());
  CHECK(!models::sized_range_like<const mutable_only_sized_range&>());
  CHECK(!models::sized_range<mutable_only_sized_range>());
  CHECK(models::sized_range<mutable_only_sized_range&>());
  CHECK(!models::sized_range<const mutable_only_sized_range>());
  CHECK(!models::sized_range<const mutable_only_sized_range&>());

  CHECK(models::range<immutable_sized_range>());
  CHECK(models::range<immutable_sized_range&>());
  CHECK(models::range<const immutable_sized_range>());
  CHECK(models::range<const immutable_sized_range&>());
  CHECK(models::sized_range_like<immutable_sized_range>());
  CHECK(models::sized_range_like<immutable_sized_range&>());
  CHECK(models::sized_range_like<const immutable_sized_range>());
  CHECK(models::sized_range_like<const immutable_sized_range&>());
  CHECK(models::sized_range<immutable_sized_range>());
  CHECK(models::sized_range<immutable_sized_range&>());
  CHECK(models::sized_range<const immutable_sized_range>());
  CHECK(models::sized_range<const immutable_sized_range&>());

  CHECK(models::range<mutable_badsized_range>());
  CHECK(models::range<mutable_badsized_range&>());
  CHECK(models::range<const mutable_badsized_range>());
  CHECK(models::range<const mutable_badsized_range&>());
  CHECK(models::sized_range_like<mutable_badsized_range>());
  CHECK(models::sized_range_like<mutable_badsized_range&>());
  CHECK(models::sized_range_like<const mutable_badsized_range>());
  CHECK(models::sized_range_like<const mutable_badsized_range&>());
  CHECK(!models::sized_range<mutable_badsized_range>());
  CHECK(!models::sized_range<mutable_badsized_range&>());
  CHECK(!models::sized_range<const mutable_badsized_range>());
  CHECK(!models::sized_range<const mutable_badsized_range&>());

  CHECK(!models::range<mutable_only_badsized_range>());
  CHECK(models::range<mutable_only_badsized_range&>());
  CHECK(!models::range<const mutable_only_badsized_range>());
  CHECK(!models::range<const mutable_only_badsized_range&>());
  CHECK(models::sized_range_like<mutable_only_badsized_range>());
  CHECK(models::sized_range_like<mutable_only_badsized_range&>());
  CHECK(!models::sized_range_like<const mutable_only_badsized_range>());
  CHECK(!models::sized_range_like<const mutable_only_badsized_range&>());
  CHECK(!models::sized_range<mutable_only_badsized_range>());
  CHECK(!models::sized_range<mutable_only_badsized_range&>());
  CHECK(!models::sized_range<const mutable_only_badsized_range>());
  CHECK(!models::sized_range<const mutable_only_badsized_range&>());

  CHECK(models::range<immutable_badsized_range>());
  CHECK(models::range<immutable_badsized_range&>());
  CHECK(models::range<const immutable_badsized_range>());
  CHECK(models::range<const immutable_badsized_range&>());
  CHECK(models::sized_range_like<immutable_badsized_range>());
  CHECK(models::sized_range_like<immutable_badsized_range&>());
  CHECK(models::sized_range_like<const immutable_badsized_range>());
  CHECK(models::sized_range_like<const immutable_badsized_range&>());
  CHECK(!models::sized_range<immutable_badsized_range>());
  CHECK(!models::sized_range<immutable_badsized_range&>());
  CHECK(!models::sized_range<const immutable_badsized_range>());
  CHECK(!models::sized_range<const immutable_badsized_range&>());

  return ::test_result();
}
