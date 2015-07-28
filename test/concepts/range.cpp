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

#elif VALIDATE_STL2
#include <stl2/detail/concepts/range.hpp>

#define NAMESPACE stl2

namespace ns {
using stl2::IteratorType;
using stl2::SentinelType;

using stl2::size;
}
#endif

#include "../simple_test.hpp"

struct not_sized_range {
  int* begin();
  int* end();
  int size() const; // launches the missiles.
};

namespace NAMESPACE {
template <>
struct is_sized_range<not_sized_range> :
  std::false_type {};
}

template <class> struct show_type;

int main() {
  CHECK(!models::range<void>());
  CHECK(!models::sized_range<void>());

  CHECK(models::range<int(&)[2]>());
  CHECK(models::sized_range_like<int(&)[2]>());
  CHECK(models::sized_range<int(&)[2]>());

  CHECK(models::range<not_sized_range&>());
  CHECK(!models::range<const not_sized_range&>());
  CHECK(!models::sized_range<not_sized_range&>());

  return ::test_result();
}
