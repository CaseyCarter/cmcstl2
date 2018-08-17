#include <stl2/iterator.hpp>
#include "../simple_test.hpp"

using namespace __stl2;

int main() {
	{
		using I = subrange<int*, int*>;
		using CI = subrange<const int*, const int*>;
		static_assert(View<I>);
		static_assert(SizedRange<I>);
		static_assert(ContiguousRange<I>);
		static_assert(CommonRange<I>);
		static_assert(View<CI>);
		static_assert(SizedRange<CI>);
		static_assert(ContiguousRange<CI>);
		static_assert(CommonRange<CI>);
	}

	{
		static constexpr int some_ints[] = {2, 3, 5, 7, 11, 13};
		static constexpr std::size_t n = size(some_ints);
		auto r = subrange(some_ints + 0, some_ints + n);
		using R = decltype(r);
		static_assert(View<R>);
		static_assert(SizedRange<R>);
		static_assert(ContiguousRange<R>);
		static_assert(CommonRange<R>);

		CHECK(begin(r) == some_ints + 0);
		CHECK(end(r) == some_ints + n);
		CHECK(!empty(r));
		CHECK(std::size_t(size(r)) == n);
		CHECK(data(r) == some_ints);
	}

	return test_result();
}
