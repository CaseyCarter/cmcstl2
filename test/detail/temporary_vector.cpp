#include <stl2/detail/temporary_vector.hpp>
#include "../simple_test.hpp"

namespace stl2 = __stl2;

int main() {
  using stl2::detail::temporary_buffer;
  using stl2::detail::temporary_vector;

  {
    // test over-aligned buffer.
    static constexpr std::size_t alignment = 2 * alignof(std::max_align_t);
    struct alignas(alignment) foo {
      int stuff[alignment / sizeof(int) + 1];
    };
    static_assert(alignof(foo) == alignment);
    static_assert(sizeof(foo) == 2 * alignment);
    auto buf = temporary_buffer<foo>{32};
    CHECK(buf.size() == 32);
    auto address = reinterpret_cast<std::uintptr_t>(buf.data());
    CHECK((address % alignment) == 0u);
  }

  {
    // test REALLY over-aligned buffer.
    static constexpr std::size_t alignment = 256;
    struct alignas(alignment) foo { char c; };
    static_assert(alignof(foo) == alignment);
    auto buf = temporary_buffer<foo>{32};
    CHECK(buf.size() == 32);
    auto address = reinterpret_cast<std::uintptr_t>(buf.data());
    CHECK((address % alignment) == 0u);
  }

  return ::test_result();
}
