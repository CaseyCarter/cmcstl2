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
#include "validate.hpp"

#if VALIDATE_RANGES
#include <utility>

#elif VALIDATE_STL2
#include <stl2/detail/concepts/object.hpp>
#include <stl2/utility.hpp>

using ranges::ext::is_nothrow_swappable_v;

#endif

#include <cstddef>

#include "../simple_test.hpp"

using namespace ranges;

namespace swappable_test {
	CONCEPT_ASSERT(swappable<int>);
	CONCEPT_ASSERT(swappable_with<int&, int&>);
	CONCEPT_ASSERT(swappable<int[4]>);
	CONCEPT_ASSERT(swappable_with<int(&)[4], int(&)[4]>);
	CONCEPT_ASSERT(!swappable_with<int, int>);
	CONCEPT_ASSERT(!swappable_with<int&, double&>);
	CONCEPT_ASSERT(!swappable_with<int(&)[4], bool(&)[4]>);
	CONCEPT_ASSERT(!swappable<int[]>);
	CONCEPT_ASSERT(!swappable<int[][4]>);

	CONCEPT_ASSERT(noexcept(swap(std::declval<int&>(), std::declval<int&>())));
	CONCEPT_ASSERT(is_nothrow_swappable_v<int&, int&>);
	CONCEPT_ASSERT(is_nothrow_swappable_v<int(&)[42], int(&)[42]>);

#if VALIDATE_STL2
	// range-v3 doesn't support swapping multidimensional arrays
	CONCEPT_ASSERT(swappable<int[3][4]>);
	CONCEPT_ASSERT(swappable_with<int(&)[3][4], int(&)[3][4]>);
	CONCEPT_ASSERT(swappable<int[3][4][1][2]>);
	CONCEPT_ASSERT(swappable_with<int(&)[3][4][1][2], int(&)[3][4][1][2]>);
	CONCEPT_ASSERT(!swappable_with<int(&)[3][4][1][2], int(&)[4][4][1][2]>);
	CONCEPT_ASSERT(is_nothrow_swappable_v<int(&)[6][7], int(&)[6][7]>);

	struct unswappable : std::string { // Has std:: as an associated namespace
		unswappable() = default;
		unswappable(const unswappable&) = delete;
		unswappable(unswappable&&) = delete;
	};
	CONCEPT_ASSERT(!swappable_with<unswappable&, unswappable&>);
	namespace __constrained_swappable {
		// Has a constrained swap findable via ADL:
		struct constrained_swappable {
			constrained_swappable() = default;
			constrained_swappable(const constrained_swappable&) = default;
			constrained_swappable(constrained_swappable&&) = default;
		};
		template<class T>
		META_CONCEPT ConstrainedSwappable = same_as<T, constrained_swappable>;
		template<ConstrainedSwappable T, ConstrainedSwappable U>
		void swap(T&, U&) {}
		template<ConstrainedSwappable T>
		void swap(T &, T &) {}
	}
	using __constrained_swappable::constrained_swappable;
	CONCEPT_ASSERT(swappable_with<constrained_swappable&, constrained_swappable&>);
	CONCEPT_ASSERT(!swappable_with<const volatile constrained_swappable&, const volatile constrained_swappable&>);
#endif

	namespace {
		struct A {
			A() = default;
			A(A&&) = delete;
			A& operator=(A&&) = delete;
			friend void swap(A&, A&) noexcept {}
		};

		CONCEPT_ASSERT(swappable<A>);
		CONCEPT_ASSERT(noexcept(swap(std::declval<A&>(), std::declval<A&>())));
		CONCEPT_ASSERT(is_nothrow_swappable_v<A&, A&>);
	}

	namespace {
		struct B {
			friend void swap(B&, B&) {}
		};

		CONCEPT_ASSERT(swappable<B>);
		CONCEPT_ASSERT(!noexcept(swap(std::declval<B&>(), std::declval<B&>())));
		CONCEPT_ASSERT(!is_nothrow_swappable_v<B&, B&>);
	}
} // namespace swappable_test

#if VALIDATE_STL2
namespace example {
	template<class T, ranges::swappable_with<T> U>
	void value_swap(T&& t, U&& u) {
		ranges::swap(std::forward<T>(t), std::forward<U>(u));
	}

	template<ranges::swappable T>
	void lv_swap(T& t1, T& t2) {
		ranges::swap(t1, t2);
	}

	namespace N {
		struct A { int m; };
		struct Proxy {
			A* a;

			Proxy(A& a) : a{&a} {}

			friend void swap(Proxy&& x, Proxy&& y) {
				ranges::swap(*x.a, *y.a);
			}
		};
		Proxy proxy(A& a) { return Proxy(a); }
	}

	void test() {
		int i = 1, j = 2;
		lv_swap(i, j);
		CHECK(i == 2);
		CHECK(j == 1);

		N::A a1 = { 5 }, a2 = { -5 };
		value_swap(a1, proxy(a2));
		CHECK(a1.m == -5);
		CHECK(a2.m == 5);

		// Additional checks for paths not exercised by the example
		value_swap(proxy(a1), a2);
		CHECK(a1.m == 5);
		CHECK(a2.m == -5);

		value_swap(proxy(a1), proxy(a2));
		CHECK(a1.m == -5);
		CHECK(a2.m == 5);

		N::Proxy p1{a1}, p2{a2};
		ranges::swap(p1, p2);
		CHECK(a1.m == -5);
		CHECK(a2.m == 5);
		CHECK(p1.a == &a2);
		CHECK(p2.a == &a1);
	}
}
#endif

int main() {
#if VALIDATE_STL2
	{
		int a[2][2] = {{0, 1}, {2, 3}};
		int b[2][2] = {{4, 5}, {6, 7}};

		CONCEPT_ASSERT(swappable_with<decltype((a)),decltype((b))>);
		swap(a, b);
		CONCEPT_ASSERT(noexcept(swap(a, b)));

		CHECK(a[0][0] == 4);
		CHECK(a[0][1] == 5);
		CHECK(a[1][0] == 6);
		CHECK(a[1][1] == 7);

		CHECK(b[0][0] == 0);
		CHECK(b[0][1] == 1);
		CHECK(b[1][0] == 2);
		CHECK(b[1][1] == 3);
	}

	example::test();
#endif

	return ::test_result();
}
