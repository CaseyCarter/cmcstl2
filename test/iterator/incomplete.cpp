#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>

namespace ranges = __stl2;

struct foo;
static_assert(ranges::same_as<std::ptrdiff_t,ranges::iter_difference_t<foo*>>);
static_assert(ranges::same_as<foo*,ranges::common_type_t<foo*, foo*>>);
struct foo {};

static_assert(ranges::ContiguousIterator<foo*>);

int main() {}
