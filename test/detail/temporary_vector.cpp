#include <stl2/detail/temporary_vector.hpp>
#include "../simple_test.hpp"

namespace ranges = __stl2;

namespace {
  template <std::size_t alignment>
  void test_single() {
    using ranges::detail::temporary_buffer;
    using ranges::detail::temporary_vector;

    struct alignas(alignment) foo {
      char space[alignment + 1];
    };
    static_assert(alignof(foo) == alignment);
    static_assert(sizeof(foo) == 2 * alignment);

    auto buf = temporary_buffer<foo>{32};
    CHECK(buf.size() == 32);

    // Verify alignment
    void* const data = buf.data();
    void* ptr = data;
    std::size_t size = sizeof(foo);
    CHECK(std::align(alignment, sizeof(foo), ptr, size) == data);
  }

  template <std::size_t...Sizes>
  void test() {
    (test_single<Sizes>(), ...);
  }
}

int main() {
  test<1, 2, 4, 8, 16, 32, 64, 128>();
  return ::test_result();
}
