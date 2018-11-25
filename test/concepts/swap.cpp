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

namespace ns {
	using std::declval;
	using std::forward;

	using ranges::is_nothrow_swappable;
	using ranges::swap;
}

#elif VALIDATE_STL2
#include <stl2/detail/concepts/object.hpp>
#include <stl2/utility.hpp>

namespace ns = ::__stl2;
#endif

#include <cstddef>

#include "../simple_test.hpp"

namespace swappable_test {
	CONCEPT_ASSERT(ranges::Swappable<int>);
	CONCEPT_ASSERT(ranges::SwappableWith<int&, int&>);
	CONCEPT_ASSERT(ranges::Swappable<int[4]>);
	CONCEPT_ASSERT(ranges::SwappableWith<int(&)[4], int(&)[4]>);
	CONCEPT_ASSERT(!ranges::SwappableWith<int, int>);
	CONCEPT_ASSERT(!ranges::SwappableWith<int&, double&>);
	CONCEPT_ASSERT(!ranges::SwappableWith<int(&)[4], bool(&)[4]>);
	CONCEPT_ASSERT(!ranges::Swappable<int[]>);
	CONCEPT_ASSERT(!ranges::Swappable<int[][4]>);

	CONCEPT_ASSERT(noexcept(ns::swap(ns::declval<int&>(), ns::declval<int&>())));
	CONCEPT_ASSERT(ns::is_nothrow_swappable<int&, int&>());
	CONCEPT_ASSERT(ns::is_nothrow_swappable<int(&)[42], int(&)[42]>());

#if VALIDATE_STL2
	// range-v3 doesn't support swapping multidimensional arrays
	CONCEPT_ASSERT(ranges::Swappable<int[3][4]>);
	CONCEPT_ASSERT(ranges::SwappableWith<int(&)[3][4], int(&)[3][4]>);
	CONCEPT_ASSERT(ranges::Swappable<int[3][4][1][2]>);
	CONCEPT_ASSERT(ranges::SwappableWith<int(&)[3][4][1][2], int(&)[3][4][1][2]>);
	CONCEPT_ASSERT(!ranges::SwappableWith<int(&)[3][4][1][2], int(&)[4][4][1][2]>);
	CONCEPT_ASSERT(ns::is_nothrow_swappable<int(&)[6][7], int(&)[6][7]>());

	struct unswappable : std::string { // Has std:: as an associated namespace
		unswappable() = default;
		unswappable(const unswappable&) = delete;
		unswappable(unswappable&&) = delete;
	};
	CONCEPT_ASSERT(!ranges::SwappableWith<unswappable&, unswappable&>);
	namespace __constrained_swappable {
		// Has a constrained swap findable via ADL:
		struct constrained_swappable {
			constrained_swappable() = default;
			constrained_swappable(const constrained_swappable&) = default;
			constrained_swappable(constrained_swappable&&) = default;
		};
		template<class T>
		STL2_CONCEPT ConstrainedSwappable =
			ns::Same<T, constrained_swappable>;
		template<ConstrainedSwappable T, ConstrainedSwappable U>
		void swap(T&, U&) {}
		template<ConstrainedSwappable T>
		void swap(T &, T &) {}
	}
	using __constrained_swappable::constrained_swappable;
	CONCEPT_ASSERT(ranges::SwappableWith<constrained_swappable&, constrained_swappable&>);
	CONCEPT_ASSERT(!ranges::SwappableWith<const volatile constrained_swappable&, const volatile constrained_swappable&>);
#endif

	namespace {
		struct A {
			A() = default;
			A(A&&) = delete;
			A& operator=(A&&) = delete;
			friend void swap(A&, A&) noexcept {}
		};

		CONCEPT_ASSERT(ranges::Swappable<A>);
		CONCEPT_ASSERT(noexcept(ns::swap(ns::declval<A&>(), ns::declval<A&>())));
		CONCEPT_ASSERT(ns::is_nothrow_swappable<A&, A&>());
	}

	namespace {
		struct B {
			friend void swap(B&, B&) {}
		};

		CONCEPT_ASSERT(ranges::Swappable<B>);
		CONCEPT_ASSERT(!noexcept(ns::swap(ns::declval<B&>(), ns::declval<B&>())));
		CONCEPT_ASSERT(!ns::is_nothrow_swappable<B&, B&>());
	}
} // namespace swappable_test

int main() {
#if VALIDATE_STL2
	{
		int a[2][2] = {{0, 1}, {2, 3}};
		int b[2][2] = {{4, 5}, {6, 7}};

		CONCEPT_ASSERT(ranges::SwappableWith<decltype((a)),decltype((b))>);
		ns::swap(a, b);
		CONCEPT_ASSERT(noexcept(ns::swap(a, b)));

		CHECK(a[0][0] == 4);
		CHECK(a[0][1] == 5);
		CHECK(a[1][0] == 6);
		CHECK(a[1][1] == 7);

		CHECK(b[0][0] == 0);
		CHECK(b[0][1] == 1);
		CHECK(b[1][0] == 2);
		CHECK(b[1][1] == 3);
	}
#endif

	return ::test_result();
}
