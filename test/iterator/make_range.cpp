#include <stl2/iterator.hpp>
#include "../simple_test.hpp"

using namespace __stl2;

int main() {
	{
		using I = subrange<int*, int*>;
		using CI = subrange<const int*, const int*>;
		static_assert(view<I>);
		static_assert(sized_range<I>);
		static_assert(contiguous_range<I>);
		static_assert(common_range<I>);
		static_assert(view<CI>);
		static_assert(sized_range<CI>);
		static_assert(contiguous_range<CI>);
		static_assert(common_range<CI>);
	}

	{
		static constexpr int some_ints[] = {2, 3, 5, 7, 11, 13};
		static constexpr std::size_t n = size(some_ints);
		auto r = subrange(some_ints + 0, some_ints + n);
		using R = decltype(r);
		static_assert(view<R>);
		static_assert(sized_range<R>);
		static_assert(contiguous_range<R>);
		static_assert(common_range<R>);

		CHECK(begin(r) == some_ints + 0);
		CHECK(end(r) == some_ints + n);
		CHECK(!empty(r));
		CHECK(std::size_t(size(r)) == n);
		CHECK(data(r) == some_ints);
	}

	return test_result();
}
