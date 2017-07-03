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
#include <stl2/optional.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/utility.hpp>
#include "simple_test.hpp"

namespace ranges = ::std::experimental::ranges;
namespace models = ranges::models;

#if ASSEMBLY // Only for assembly inspection.
using S = short;
using T = int;

T test_dereference(const ranges::optional<T>& o)
noexcept(std::is_nothrow_copy_constructible<T>::value )
{
	return *o;
}

int test_value(const ranges::optional<T>& o) {
	return o.value();
}

ranges::optional<T> test_copy(const ranges::optional<T>& o)
noexcept(std::is_nothrow_copy_constructible<ranges::optional<T>>::value)
{
	return o;
}

bool test_eq(const ranges::optional<T>& l, const ranges::optional<S>& r) {
	return l == r;
}

auto test_convert(ranges::optional<S>&& o) {
	return ranges::optional<T>{std::move(o)};
}

auto test_convert(const ranges::optional<S>& o) {
	return ranges::optional<T>{o};
}

ranges::optional<T> test_pass_and_return_by_value(ranges::optional<T> o) {
	return o;
}

#endif

namespace X {
	struct B;
	struct A {
		int i;
		A(int i = 42) : i{i} {}
		A(B const& b);
	};

	struct B {
		int i;
		B(int i = 13) : i{i} {}
		B(A const& a) : i{a.i} {}
	};

	inline A::A(B const& b) : i{b.i} {}

	struct C {
		int i ;
		C(int i = 1729) : i{i} {}
		C(A const& a) : i{a.i} {}
		C(B const& b) : i{b.i} {}
	};

	int count = 0;

	void swap(auto& x, auto& y) noexcept {
		++count;
		ranges::swap(x.i, y.i);
	}
}


namespace std::experimental::ranges {
	template <>
	struct common_type<::X::A, ::X::B> { using type = ::X::C; };
	template <>
	struct common_type<::X::B, ::X::A> { using type = ::X::C; };
}

static_assert(ranges::models::Common<::X::A, ::X::B>);
static_assert(ranges::models::Swappable<::X::A>);
static_assert(ranges::models::Swappable<::X::B>);
static_assert(ranges::models::Swappable<::X::C>);
static_assert(ranges::models::SwappableWith<::X::A&, ::X::B&>);

void test_cross_type_swap() {
	ranges::optional<::X::A> oa{ranges::in_place};
	ranges::optional<::X::B> ob{ranges::in_place};

	CHECK(oa->i == 42);
	CHECK(ob->i == 13);
	CHECK(X::count == 0);

	ranges::swap(oa, ob);
	CHECK(oa->i == 13);
	CHECK(ob->i == 42);
	CHECK(X::count == 1);

	ranges::swap(ob, oa);
	CHECK(oa->i == 42);
	CHECK(ob->i == 13);
	CHECK(X::count == 2);
}

int main() {
	{
		// Ensure that {} cannot convert to nullopt_t
		struct S {
			static int foo(ranges::nullopt_t);
			static void foo(int);
		};
		static_assert(std::is_same<void, decltype(S::foo({}))>());
	}
	{
		ranges::optional<int> o;
		CHECK(!o);
		o = 42;
		CHECK(o);
		CHECK(*o == 42);
		CHECK(o == 42);
		CHECK(o < 43);
		CHECK(o <= 43);
		CHECK(o > 41);
		CHECK(o >= 41);
		CHECK(!(o < o));
		CHECK(!(o > o));
		CHECK(o <= o);
		CHECK(o >= o);

		CHECK(!(ranges::nullopt == o));
		CHECK(ranges::nullopt != o);
		CHECK(ranges::nullopt < o);
		CHECK(!(ranges::nullopt > o));
		CHECK(ranges::nullopt <= o);
		CHECK(!(ranges::nullopt >= o));

		CHECK(!(o == ranges::nullopt));
		CHECK(o != ranges::nullopt);
		CHECK(!(o < ranges::nullopt));
		CHECK(o > ranges::nullopt);
		CHECK(!(o <= ranges::nullopt));
		CHECK(o >= ranges::nullopt);

		CHECK(o == ranges::optional<int>{42});
		CHECK(o == ranges::optional<double>{42.0});
		CHECK(o != ranges::optional<int>{13});
		CHECK(o != ranges::optional<double>{13.0});
		CHECK(o.value() == 42);
		o = ranges::nullopt;
		CHECK(!o);
		CHECK(o.value_or(42) == 42);
		o = 3;
		CHECK(o);
		CHECK(o == 3);
		o = {};
		CHECK(!o);
		{
			auto oi = ranges::make_optional<int>(42);
			static_assert(models::Swappable<ranges::optional<int>>);
			ranges::swap(o, oi);
			CHECK(!oi);
			CHECK(o);
			CHECK(*o == 42);
			oi = 13;
			ranges::swap(o, oi);
			CHECK(*o == 13);
			CHECK(*oi == 42);
		}
	}

	{
		constexpr auto o = ranges::make_optional<int>(42);
		static_assert(o);
		static_assert(*o == 42);
		static_assert(o == 42);
		static_assert(o < 43);
		static_assert(o.value() == 42);
		static_assert(o.value_or(13) == 42);
		static_assert(o == ranges::optional<int>{42});
		static_assert(o == ranges::optional<double>{42.});
		static_assert(o != ranges::optional<int>{13});
		static_assert(o != ranges::optional<double>{3.14});
	}
	{
		constexpr ranges::optional<int> o;
		static_assert(!o);
		static_assert(o.value_or(42) == 42);
		static_assert(o != ranges::optional<int>{13});
		static_assert(o != ranges::optional<double>{3.14});
	}

	{
		using OI = ranges::optional<int>;
		using OL = ranges::optional<long>;

		static_assert(models::StrictTotallyOrdered<int>);
		static_assert(models::StrictTotallyOrdered<OI>);
		static_assert(models::StrictTotallyOrderedWith<int, OI>);

		static_assert(models::StrictTotallyOrdered<long>);
		static_assert(models::StrictTotallyOrdered<OL>);
		static_assert(models::StrictTotallyOrderedWith<long, OL>);

		static_assert(models::StrictTotallyOrderedWith<int, long>);
		static_assert(models::StrictTotallyOrderedWith<OI, OL>);

		static_assert(models::Constructible<long, int>);
		static_assert(models::ConvertibleTo<int, long>);
		static_assert(models::Constructible<OL, OI>);
		static_assert(models::ConvertibleTo<OI, OL>);

		static_assert(models::ConvertibleTo<OL, OI>);
		static_assert(models::Constructible<OI, OL>);

		static_assert(models::ConvertibleTo<long, int>);
		static_assert(models::Constructible<int, long>);

		static_assert(!models::SwappableWith<OI&, OL&>);
	}

	{
		using OI = ranges::optional<int>;
		constexpr OI four{4};
		constexpr OI empty;
		static_assert(!(four == empty));
		static_assert(four != empty);
		static_assert(!(four < empty));
		static_assert(four > empty);
		static_assert(!(four <= empty));
		static_assert(four >= empty);

		static_assert(!(empty == four));
		static_assert(empty != four);
		static_assert(empty < four);
		static_assert(!(empty > four));
		static_assert(empty <= four);
		static_assert(!(empty >= four));
	}

	{
		using OI = ranges::optional<int>;
		using OL = ranges::optional<long>;
		constexpr OI four{4};
		constexpr OL three{3};
		static_assert(!(four == three));
		static_assert(four != three);
		static_assert(!(four < three));
		static_assert(four > three);
		static_assert(!(four <= three));
		static_assert(four >= three);

		static_assert(!(three == four));
		static_assert(three != four);
		static_assert(three < four);
		static_assert(!(three > four));
		static_assert(three <= four);
		static_assert(!(three >= four));
	}

	test_cross_type_swap();

	return test_result();
}
