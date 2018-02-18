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
#include <iostream>
#include <vector>
#include "simple_test.hpp"

void test_range_access_ambiguity() {
	std::vector<__stl2::reverse_iterator<int*>> vri{};
	using namespace __stl2;
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
		for (auto p = __stl2::begin(il), e = __stl2::end(il); p != e; ++p) {
			CHECK(*p == count++);
		}
	}
	{
		int count = 3;
		for (auto p = __stl2::rbegin(il), e = __stl2::rend(il); p != e; ++p) {
			CHECK(*p == --count);
		}
	}
	CHECK(__stl2::size(il) == std::size_t{3});
	CHECK(__stl2::data(il) == &*il.begin());
	CHECK(__stl2::empty(il) == false);
}

template <class T, T... Is>
void test_array(std::integer_sequence<T, Is...>) {
	T a[sizeof...(Is)] = { Is... };
	{
		int count = 0;
		for (auto p = __stl2::begin(a), e = __stl2::end(a); p != e; ++p) {
			CHECK(*p == count++);
		}
	}
	{
		int count = sizeof...(Is);
		for (auto p = __stl2::rbegin(a), e = __stl2::rend(a); p != e; ++p) {
			CHECK(*p == --count);
		}
	}
	CHECK(__stl2::size(a) == sizeof...(Is));
	CHECK(__stl2::data(a) == a + 0);
	CHECK(__stl2::empty(a) == false);
}

namespace begin_testing {
template <class R>
concept bool CanBegin =
	requires(R&& r) {
		__stl2::begin((R&&)r);
	};

template <class>
constexpr bool can_begin = false;
CanBegin{R}
constexpr bool can_begin<R> = true;

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
	namespace models = __stl2::models;

	// Valid
	static_assert(can_begin<int(&)[2]>);
	static_assert(models::Same<decltype(__stl2::begin(__stl2::declval<int(&)[2]>())), int*>);
	static_assert(can_begin<const int(&)[2]>);
	static_assert(models::Same<decltype(__stl2::begin(__stl2::declval<const int(&)[2]>())), const int*>);

	// Ill-formed: array rvalue
	static_assert(!can_begin<int(&&)[2]>);

	// Valid: only member begin
	static_assert(can_begin<A&>);
	static_assert(!can_begin<A>);
	static_assert(models::Same<decltype(__stl2::begin(__stl2::declval<A&>())), int*>);
	static_assert(can_begin<const A&>);
	static_assert(models::Same<decltype(__stl2::begin(__stl2::declval<const A&>())), const int*>);

	// Valid: Both member and non-member begin, but non-member returns non-Iterator.
	static_assert(can_begin<B&>);
	static_assert(models::Same<decltype(__stl2::begin(__stl2::declval<B&>())), int*>);
	static_assert(can_begin<const B&>);
	static_assert(models::Same<decltype(__stl2::begin(__stl2::declval<const B&>())), const int*>);

	// Valid: Both member and non-member begin, but non-member returns non-Iterator.
	static_assert(can_begin<C&>);
	static_assert(can_begin<const C&>);

	// Valid: Prefer member begin
	static_assert(can_begin<D&>);
	static_assert(models::Same<int*, decltype(__stl2::begin(__stl2::declval<D&>()))>);
	static_assert(can_begin<const D&>);
	static_assert(models::Same<const int*, decltype(__stl2::begin(__stl2::declval<const D&>()))>);

	{
		using T = std::initializer_list<int>;
		// Valid: begin accepts lvalue initializer_list
		static_assert(models::Same<const int*, decltype(__stl2::begin(__stl2::declval<T&>()))>);
		static_assert(models::Same<const int*, decltype(__stl2::begin(__stl2::declval<const T&>()))>);
	}

	static_assert(can_begin<__stl2::ext::subrange<int*, int*>&>);
	static_assert(can_begin<__stl2::ext::subrange<int*, int*>&&>);
}
} // namespace begin_testing

namespace X {
template <class T, std::size_t N>
	requires N != 0
struct array {
	T elements_[N];

	constexpr bool empty() const noexcept { return N == 0; }
	constexpr T* data() noexcept { return elements_; }
	constexpr const T* data() const noexcept { return elements_; }
};

template <class T, std::size_t N>
constexpr T* begin(array<T, N>& a) noexcept { return a.elements_; }
template <class T, std::size_t N>
constexpr T* end(array<T, N>& a) noexcept { return a.elements_ + N; }
template <class T, std::size_t N>
constexpr const T* begin(const array<T, N>& a) noexcept { return a.elements_; }
template <class T, std::size_t N>
constexpr const T* end(const array<T, N>& a) noexcept { return a.elements_ + N; }
}

using I = int*;
using CI = const int*;
static_assert(__stl2::models::Iterator<I>);
static_assert(__stl2::models::Iterator<CI>);

int main() {
	using namespace __stl2;

	static constexpr X::array<int, 4> some_ints = {{0,1,2,3}};

	constexpr auto first = begin(some_ints);
	constexpr auto last = end(some_ints);
	static_assert(models::Same<const CI, decltype(first)>);
	static_assert(models::Same<const CI, decltype(last)>);
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

	return ::test_result();
}
