#include "validate.hpp"

#if VALIDATE_RANGES
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>

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

namespace ns {
using stl2::IteratorType;
using stl2::SentinelType;

using stl2::size;
}
#endif

#include "../simple_test.hpp"

template <class> struct show_type;

int main() {
  using ns::size;

  CONCEPT_ASSERT(!models::range<void>());
  CONCEPT_ASSERT(!models::sized_range<void>());

  CONCEPT_ASSERT(models::range<int(&)[2]>());
  CONCEPT_ASSERT(models::sized_range_like<int(&)[2]>());
  CONCEPT_ASSERT(models::sized_range<int(&)[2]>());

  return ::test_result();
}
