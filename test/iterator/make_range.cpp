#include <stl2/iterator.hpp>
#include "../simple_test.hpp"

using namespace __stl2;

int main() {
	{
		using I = ext::subrange<int*, int*>;
		using CI = ext::subrange<const int*, const int*>;
		static_assert(models::View<I>);
		static_assert(models::SizedRange<I>);
		static_assert(models::ContiguousRange<I>);
		static_assert(models::BoundedRange<I>);
		static_assert(models::View<CI>);
		static_assert(models::SizedRange<CI>);
		static_assert(models::ContiguousRange<CI>);
		static_assert(models::BoundedRange<CI>);
	}

	{
		static constexpr int some_ints[] = {2, 3, 5, 7, 11, 13};
		static constexpr auto n = static_cast<std::ptrdiff_t>(size(some_ints));
		constexpr auto r = ext::subrange(some_ints + 0, some_ints + n);
		using R = decltype(r);
		static_assert(models::View<std::remove_const_t<R>>);
		static_assert(models::SizedRange<R>);
		static_assert(models::ContiguousRange<R>);
		static_assert(models::BoundedRange<R>);

		static_assert(begin(r) == some_ints + 0);
		static_assert(end(r) == some_ints + n);
		static_assert(!empty(r));
		static_assert(r);
		static_assert(size(r) == n);
		static_assert(data(r) == some_ints);

		static_assert(sizeof(r) == 2 * sizeof(void*));

		(void)R{std::pair<int const*, int const*>{r}};
	}

	return test_result();
}
