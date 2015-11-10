#include <stl2/detail/temporary_vector.hpp>
#include "../simple_test.hpp"

namespace ranges = __stl2;

using ranges::detail::temporary_buffer;
using ranges::detail::temporary_vector;

namespace {
  template <std::size_t Alignment>
  void test_single_alignment() {
    struct alignas(Alignment) foo {
      char space[Alignment + 1];
    };
    static_assert(alignof(foo) == Alignment);
    static_assert(sizeof(foo) == 2 * Alignment);

    auto buf = temporary_buffer<foo>{32};
    CHECK(buf.size() == 32);

    // Verify alignment
    void* const data = buf.data();
    void* ptr = data;
    std::size_t size = sizeof(foo);
    CHECK(std::align(Alignment, sizeof(foo), ptr, size) == data);
  }

  template <std::size_t...Alignments>
  void test_alignments() {
    (test_single_alignment<Alignments>(), ...);
  }
}

int main() {
  test_alignments<1, 2, 4, 8, 16, 32, 64, 128>();
  return ::test_result();
}
