// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/view/repeat.hpp>
#include <stl2/view/take.hpp>
#include <array>
#include <vector>
#include "../simple_test.hpp"

namespace ranges = __stl2;

int main() {
	{
		auto v = ranges::ext::repeat_view<int>(42);
		static_assert(sizeof(v) == sizeof(int));

		CHECK(v.value() == 42);
		auto first = v.begin();
		CHECK(*first == v.value());

		// Sentinel (unreachable) is empty:
		static_assert(std::is_empty<decltype(v.end())>());

		// int is "cheap" to copy:
		static_assert(ranges::detail::cheaply_copyable<int>);
		// so the iterators hold copies:
		static_assert(sizeof(first) == sizeof(int));
		// and operator* returns copies:
		static_assert(ranges::Same<decltype(*first), int>);
		auto second = first + 1;
		CHECK(*second == *first);

		// operator-> returns a pointer
		static_assert(
			ranges::Same<
				decltype(first.operator->()),
				const int*>);
	}
	{
		using big_type = std::array<double, 128>;
		auto v = ranges::ext::repeat_view<big_type>(big_type{});
		static_assert(sizeof(v) == sizeof(big_type));

		CHECK(v.value() == big_type{});
		auto first = v.begin();
		CHECK(*first == v.value());

		// big_type is too big:
		static_assert(sizeof(big_type) > ranges::detail::cheap_copy_size);
		// and therefore NOT "cheap" to copy:
		static_assert(!ranges::detail::cheaply_copyable<big_type>);
		// so the iterators hold references:
		static_assert(sizeof(first) == sizeof(void*));
		// and operator* returns a reference:
		CHECK(&*first == &v.value());

		// operator-> returns a pointer
		static_assert(
			ranges::Same<
				decltype(first.operator->()),
				const big_type*>);
		CHECK(&*first == first.operator->());
	}
	{
		constexpr unsigned N = 1u << 16;
		auto v = ranges::ext::repeat_view<std::vector<int>>{std::vector<int>(N, 42)};
		static_assert(sizeof(v) == sizeof(std::vector<int>));

		CHECK(v.value() == std::vector<int>(N, 42));
		auto first = v.begin();
		CHECK((*first).size() == N);
		CHECK(*first == v.value());

		// std::vector is small enough:
		static_assert(sizeof(std::vector<int>) <= ranges::detail::cheap_copy_size);
		// but not trivially destructible / copyable due to memory allocation:
		static_assert(!std::is_trivially_destructible<std::vector<int>>());
		// ...and therefore not cheap to copy:
		static_assert(!ranges::detail::cheaply_copyable<std::vector<int>>);
		// So the iterators again hold references instead of copies:
		static_assert(sizeof(first) == sizeof(void*));
		CHECK(&*first == &v.value());
		auto forty_second = first + 41;
		CHECK(&*forty_second == &v.value());

		// operator-> again returns a pointer
		static_assert(
			ranges::Same<
				decltype(first.operator->()),
				const std::vector<int>*>);
		CHECK(first->size() == N);
	}
	{
		auto v = ranges::view::ext::repeat(9) | ranges::view::take(10);
		static_assert(ranges::View<decltype(v)>);
		static_assert(ranges::RandomAccessIterator<decltype(v.begin())>);
		CHECK_EQUAL(v, {9, 9, 9, 9, 9, 9, 9, 9, 9, 9});
	}
	{
		struct empty { 
			bool operator==(empty const&) const noexcept { return true; }
			bool operator!=(empty const&) const noexcept { return false; }
		};

		auto e = empty{};
		auto v2 = ranges::view::ext::repeat(e) | ranges::view::take(3);
		CHECK_EQUAL(v2, {e, e, e});

		auto v3 = ranges::view::ext::repeat(std::move(e)) | ranges::view::take(3);
		CHECK_EQUAL(v2, v3);
	}

	return test_result();
}
