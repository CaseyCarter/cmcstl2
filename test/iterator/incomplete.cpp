#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>

namespace ranges = __stl2;

struct foo;
static_assert(ranges::Same<std::ptrdiff_t,ranges::difference_type_t<foo*>>);
static_assert(ranges::Same<foo*,ranges::common_type_t<foo*, foo*>>);
struct foo {};

static_assert(ranges::ext::ContiguousIterator<foo*>);

int main() {}
