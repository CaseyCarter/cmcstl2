#include <stl2/detail/iterator/ostream_iterator.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <sstream>
#include "../simple_test.hpp"

using namespace stl2;
namespace models = stl2::ext::models;

namespace {
  template <InputIterator I, Sentinel<I> S, WeakOutputIterator<ValueType<I>> O>
  tagged_pair<tag::in(I), tag::out(O)>
  constexpr copy(I first, S last, O out) {
    for (; first != last; ++first, void(), ++out) {
      *out = *first;
    }
    return {first, out};
  }
}

int main() {
  using I = ostream_iterator<int>;
  static_assert(models::weak_output_iterator<I, int>());
  std::stringstream ss;
  
  static constexpr int some_ints[] = {0, 7, 1, 6, 2, 5, 3, 4};

  ::copy(stl2::begin(some_ints), stl2::end(some_ints), I{ss, " "});
  CHECK(ss.str() == "0 7 1 6 2 5 3 4 ");
  ::check_equal(
    ext::make_range(istream_iterator<int>{ss}, default_sentinel{}),
      some_ints);

  return ::test_result();
}
