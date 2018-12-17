// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/iterator.hpp>
#include <stl2/detail/algorithm/find.hpp>
#include <stl2/view/subrange.hpp>
#include <stl2/view/ref.hpp>
#include <stl2/view/iota.hpp>
#include <iostream>
#include <vector>
#include <utility>
#include "simple_test.hpp"

namespace ranges = __stl2;

void test_range_access_ambiguity() {
	std::vector<ranges::reverse_iterator<int*>> vri{};
	using namespace ranges;
	(void)begin(vri);
	(void)end(vri);
	(void)cbegin(vri);
	(void)cend(vri);
	(void)rbegin(vri);
	(void)rend(vri);
	(void)crbegin(vri);
	(void)crend(vri);
}

void test_initializer_list() {
	std::initializer_list<int> il = {0,1,2};
	{
		int count = 0;
		for (auto p = ranges::begin(il), e = ranges::end(il); p != e; ++p) {
			CHECK(*p == count++);
		}
	}
	{
		int count = 3;
		for (auto p = ranges::rbegin(il), e = ranges::rend(il); p != e; ++p) {
			CHECK(*p == --count);
		}
	}
	CHECK(ranges::size(il) == std::size_t{3});
	CHECK(ranges::data(il) == &*il.begin());
	CHECK(ranges::empty(il) == false);
}

template<class T, T... Is>
void test_array(std::integer_sequence<T, Is...>) {
	T a[sizeof...(Is)] = { Is... };
	{
		int count = 0;
		for (auto p = ranges::begin(a), e = ranges::end(a); p != e; ++p) {
			CHECK(*p == count++);
		}
	}
	{
		int count = sizeof...(Is);
		for (auto p = ranges::rbegin(a), e = ranges::rend(a); p != e; ++p) {
			CHECK(*p == --count);
		}
	}
	CHECK(ranges::size(a) == sizeof...(Is));
	CHECK(ranges::data(a) == a + 0);
	CHECK(ranges::empty(a) == false);
}

namespace begin_testing {
	template<class R>
	META_CONCEPT CanBegin =
		requires(R&& r) {
			ranges::begin((R&&)r);
		};

	template<class R>
	META_CONCEPT CanCBegin =
		requires(R&& r) {
			ranges::cbegin((R&&)r);
		};

	struct A {
		int* begin();
		int* end();
		const int* begin() const;
		const int* end() const;
	};

	struct B : A {};
	void* begin(B&);

	struct C : A {};
	void begin(C&);

	struct D : A {};
	char* begin(D&);

	void test() {
		// Valid
		static_assert(CanBegin<int(&)[2]>);
		static_assert(ranges::Same<decltype(ranges::begin(std::declval<int(&)[2]>())), int*>);
		static_assert(CanBegin<const int(&)[2]>);
		static_assert(ranges::Same<decltype(ranges::begin(std::declval<const int(&)[2]>())), const int*>);

		static_assert(CanCBegin<int(&)[2]>);
		static_assert(ranges::Same<decltype(ranges::cbegin(std::declval<int(&)[2]>())), const int*>);
		static_assert(CanCBegin<const int(&)[2]>);
		static_assert(ranges::Same<decltype(ranges::cbegin(std::declval<const int(&)[2]>())), const int*>);

		// Ill-formed: array rvalue
		static_assert(!CanBegin<int(&&)[2]>);
		static_assert(!CanBegin<const int(&&)[2]>);

		static_assert(!CanCBegin<int(&&)[2]>);
		static_assert(!CanCBegin<const int(&&)[2]>);

		// Valid: only member begin
		static_assert(CanBegin<A&>);
		static_assert(!CanBegin<A>);
		static_assert(ranges::Same<decltype(ranges::begin(std::declval<A&>())), int*>);
		static_assert(CanBegin<const A&>);
		static_assert(!CanBegin<const A>);
		static_assert(ranges::Same<decltype(ranges::begin(std::declval<const A&>())), const int*>);

		// Valid: Both member and non-member begin, but non-member returns non-Iterator.
		static_assert(CanBegin<B&>);
		static_assert(!CanBegin<B>);
		static_assert(ranges::Same<decltype(ranges::begin(std::declval<B&>())), int*>);
		static_assert(CanBegin<const B&>);
		static_assert(!CanBegin<const B>);
		static_assert(ranges::Same<decltype(ranges::begin(std::declval<const B&>())), const int*>);

		// Valid: Both member and non-member begin, but non-member returns non-Iterator.
		static_assert(CanBegin<C&>);
		static_assert(!CanBegin<C>);
		static_assert(CanBegin<const C&>);
		static_assert(!CanBegin<const C>);

		// Valid: Prefer member begin
		static_assert(CanBegin<D&>);
		static_assert(!CanBegin<D>);
		static_assert(ranges::Same<int*, decltype(ranges::begin(std::declval<D&>()))>);
		static_assert(CanBegin<const D&>);
		static_assert(!CanBegin<const D>);
		static_assert(ranges::Same<const int*, decltype(ranges::begin(std::declval<const D&>()))>);

		{
			using T = std::initializer_list<int>;
			// Valid: begin accepts lvalue initializer_list
			static_assert(ranges::Same<const int*, decltype(ranges::begin(std::declval<T&>()))>);
			static_assert(ranges::Same<const int*, decltype(ranges::begin(std::declval<const T&>()))>);
			static_assert(!CanBegin<std::initializer_list<int>>);
			static_assert(!CanBegin<const std::initializer_list<int>>);
		}

		static_assert(CanBegin<ranges::subrange<int*, int*>&>);
		static_assert(CanBegin<const ranges::subrange<int*, int*>&>);
		static_assert(CanBegin<ranges::subrange<int*, int*>>);
		static_assert(CanBegin<const ranges::subrange<int*, int*>>);

		static_assert(CanCBegin<ranges::subrange<int*, int*>&>);
		static_assert(CanCBegin<const ranges::subrange<int*, int*>&>);
		static_assert(CanCBegin<ranges::subrange<int*, int*>>);
		static_assert(CanCBegin<const ranges::subrange<int*, int*>>);

		static_assert(CanBegin<ranges::ref_view<int[5]>&>);
		static_assert(CanBegin<const ranges::ref_view<int[5]>&>);
		static_assert(CanBegin<ranges::ref_view<int[5]>>);
		static_assert(CanBegin<const ranges::ref_view<int[5]>>);

		static_assert(CanCBegin<ranges::ref_view<int[5]>&>);
		static_assert(CanCBegin<const ranges::ref_view<int[5]>&>);
		static_assert(CanCBegin<ranges::ref_view<int[5]>>);
		static_assert(CanCBegin<const ranges::ref_view<int[5]>>);

		static_assert(CanBegin<ranges::iota_view<int, int>&>);
		static_assert(CanBegin<const ranges::iota_view<int, int>&>);
		static_assert(CanBegin<ranges::iota_view<int, int>>);
		static_assert(CanBegin<const ranges::iota_view<int, int>>);

		static_assert(CanCBegin<ranges::iota_view<int, int>&>);
		static_assert(CanCBegin<const ranges::iota_view<int, int>&>);
		static_assert(CanCBegin<ranges::iota_view<int, int>>);
		static_assert(CanCBegin<const ranges::iota_view<int, int>>);
	}
} // namespace begin_testing

namespace X {
	template<class T, std::size_t N>
		requires N != 0
	struct array {
		T elements_[N];

		constexpr bool empty() const noexcept { return N == 0; }
		constexpr T* data() noexcept { return elements_; }
		constexpr const T* data() const noexcept { return elements_; }
	};

	template<class T, std::size_t N>
	constexpr T* begin(array<T, N>& a) noexcept { return a.elements_; }
	template<class T, std::size_t N>
	constexpr T* end(array<T, N>& a) noexcept { return a.elements_ + N; }
	template<class T, std::size_t N>
	constexpr const T* begin(const array<T, N>& a) noexcept { return a.elements_; }
	template<class T, std::size_t N>
	constexpr const T* end(const array<T, N>& a) noexcept { return a.elements_ + N; }
} // namespace X

using I = int*;
using CI = const int*;
static_assert(ranges::Iterator<I>);
static_assert(ranges::Iterator<CI>);

void test_string_view_p0970() {
	// basic_string_views are non-dangling
	using I = ranges::iterator_t<std::string_view>;
	static_assert(ranges::Same<I, decltype(ranges::begin(std::declval<std::string_view>()))>);
	static_assert(ranges::Same<I, decltype(ranges::end(std::declval<std::string_view>()))>);
	static_assert(ranges::Same<I, decltype(ranges::begin(std::declval<const std::string_view>()))>);
	static_assert(ranges::Same<I, decltype(ranges::end(std::declval<const std::string_view>()))>);

	{
		const char hw[] = "Hello, World!";
		auto result = ranges::find(std::string_view{hw}, 'W');
		static_assert(ranges::Same<I, decltype(result)>);
		CHECK(result == std::string_view{hw}.begin() + 7);
	}
}

int main() {
	using namespace ranges;

	static constexpr X::array<int, 4> some_ints = {{0,1,2,3}};

	constexpr auto first = begin(some_ints);
	constexpr auto last = end(some_ints);
	static_assert(ranges::Same<const CI, decltype(first)>);
	static_assert(ranges::Same<const CI, decltype(last)>);
	static_assert(first == cbegin(some_ints));
	static_assert(last == cend(some_ints));

	static_assert(noexcept(begin(some_ints)));
	static_assert(noexcept(end(some_ints)));
	static_assert(noexcept(cbegin(some_ints)));
	static_assert(noexcept(cend(some_ints)));
	static_assert(noexcept(empty(some_ints)));
	static_assert(noexcept(data(some_ints)));

	constexpr bool output = false;
	static_assert(!empty(some_ints));
	if (output) std::cout << '{';
	auto is_first = true;
	auto count = 0;
	for (auto&& i : some_ints) {
		CHECK(i == count++);
		if (is_first) {
			is_first = false;
		} else {
			if (output) std::cout << ", ";
		}
		if (output) std::cout << i;
	}
	if (output) std::cout << "}\n";

	test_initializer_list();
	test_array(std::make_integer_sequence<int, 3>{});
	test_array(std::make_integer_sequence<const int, 3>{});
	begin_testing::test();

	test_string_view_p0970();

	return ::test_result();
}
