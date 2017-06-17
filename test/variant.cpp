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
#include <iostream>
#include <new>
#include <vector>
#include <stl2/type_traits.hpp>
#include <stl2/variant.hpp>
#include "simple_test.hpp"

using namespace __stl2;

namespace {
// variants of trivially {destructible,move constructible,copy constructible} types
// are trivially {destructible,move constructible,copy constructible}.
using VDI = variant<char, short, int, long, long long, float, double>;
static_assert(is_trivially_destructible<VDI>());
static_assert(is_trivially_move_constructible<VDI>());
static_assert(is_trivially_copy_constructible<VDI>());
static_assert(is_trivially_move_assignable<VDI>());
static_assert(is_trivially_copy_assignable<VDI>());

struct nontrivial {
	struct counts {
		unsigned create;
		unsigned move;
		unsigned copy;
		unsigned move_assign;
		unsigned copy_assign;
		unsigned destroy;
		unsigned swap;
	};
	static counts count;

	static void zero() {
		count = {};
	}

	~nontrivial() { ++count.destroy; }
	nontrivial() noexcept { ++count.create; }
	nontrivial(const nontrivial&) { ++count.copy; }
	nontrivial(nontrivial&&) noexcept { ++count.move; }
	nontrivial& operator=(nontrivial&&) & noexcept { ++count.move_assign; return *this; }
	nontrivial& operator=(const nontrivial&) & { ++count.copy_assign; return *this; }
	friend void swap(nontrivial&, nontrivial&) noexcept {
		++count.swap;
	}
};

nontrivial::counts nontrivial::count;

struct nontrivial_literal {
	int value;

	constexpr nontrivial_literal(int v = 0) : value{v} {}
	constexpr nontrivial_literal(nontrivial_literal&& that) : value{that.value} {}
	constexpr nontrivial_literal(const nontrivial_literal& that) : value{that.value} {}
	constexpr nontrivial_literal& operator=(nontrivial_literal&& that) & {
		value = that.value;
		return *this;
	}
	constexpr nontrivial_literal& operator=(const nontrivial_literal& that) & {
		value = that.value;
		return *this;
	}

	constexpr operator int() const {
		return value;
	}

	friend std::ostream& operator<<(std::ostream& os, const nontrivial_literal& ntl) {
		return os << "nontrivial_literal{" << ntl.value << '}';
	}
};

struct moveonly {
	moveonly() = default;
	moveonly(moveonly&&) = default;
	moveonly& operator=(moveonly&&) & = default;
};

struct nonmovable {
	nonmovable() = default;
	nonmovable(nonmovable&&) = delete;
	nonmovable& operator=(nonmovable&&) & = delete;
};

void test_raw() {
	__variant::storage<int, double, nontrivial, reference_wrapper<int>> v;
	static_assert(sizeof(v) == sizeof(double));
	{
		int& i = __variant::st_access::raw_get(in_place_index<0>, v);
		{
			const int& i = __variant::st_access::raw_get(in_place_index<0>, v);
			(void)i;
		}
		{
			const int& i = __variant::st_access::raw_get(in_place_index<0>, const_cast<const decltype(v)&>(v));
			(void)i;
		}
		{
			int&& i = __variant::st_access::raw_get(in_place_index<0>, move(v));
			(void)i;
		}
		i = 42;
		CHECK(__variant::st_access::raw_get(in_place_index<0>, v) == 42);
	}

	{
		double& d = __variant::st_access::raw_get(in_place_index<1>, v);
		::new(&d) double;
		{
			const double& d = __variant::st_access::raw_get(in_place_index<1>, v);
			(void)d;
		}
		{
			const double& d = __variant::st_access::raw_get(in_place_index<1>, const_cast<const decltype(v)&>(v));
			(void)d;
		}
		{
			double&& d = __variant::st_access::raw_get(in_place_index<1>, move(v));
			(void)d;
		}
		d = 3.14;
		CHECK(__variant::st_access::raw_get(in_place_index<1>, v) == 3.14);
	}

	{
		nontrivial& n = __variant::st_access::raw_get(in_place_index<2>, v);
		::new(&n) nontrivial;
		{
			const nontrivial& n = __variant::st_access::raw_get(in_place_index<2>, v);
			(void)n;
		}
		{
			const nontrivial& n = __variant::st_access::raw_get(in_place_index<2>, const_cast<const decltype(v)&>(v));
			(void)n;
		}
		{
			nontrivial&& n = __variant::st_access::raw_get(in_place_index<2>, move(v));
			(void)n;
		}
		n.~nontrivial();
	}

	{
		int value = 42;
		reference_wrapper<int>& r = __variant::st_access::raw_get(in_place_index<3>, v);
		::new(&r) reference_wrapper<int>{value};
		int& i = __variant::cook<int&>(r);
		CHECK(i == 42);
		CHECK(&i == &value);
		reference_wrapper<int>&& rr = __variant::st_access::raw_get(in_place_index<3>, move(v));
		int&& ii = __variant::cook<int&&>(move(rr));
		CHECK(&ii == &value);
		r.~reference_wrapper();
	}
}

void test_in_place_index() {
	{
		using V = variant<int, int&, int&&, const int, const int&, const int&&>;
		int i = 42;
		const int& ci = i;

		{
			V v{in_place_index<0>, i};
			CHECK(v.index() == 0u);
		}
		{
			V v{in_place_index<0>, ci};
			CHECK(v.index() == 0u);
		}
		{
			V v{in_place_index<0>, move(i)};
			CHECK(v.index() == 0u);
		}
		{
			V v{in_place_index<0>, move(ci)};
			CHECK(v.index() == 0u);
		}

		{
			V v{in_place_index<1>, i};
			CHECK(v.index() == 1u);
		}
		{
			// V v{in_place_index<1>, ci}; // ill-formed
			static_assert(!models::Constructible<V, in_place_index_t<1>, const int&>);
		}
		{
			// V v{in_place_index<1>, move(i)}; // ill-formed
			static_assert(!models::Constructible<V, in_place_index_t<1>, int&&>);
		}
		{
			// V v{in_place_index<1>, move(ci)}; // ill-formed
			static_assert(!models::Constructible<V, in_place_index_t<1>, const int&&>);
		}

		{
			// V v{in_place_index<2>, i}; // ill-formed
			static_assert(!models::Constructible<V, in_place_index_t<2>, int&>);
		}
		{
			// V v{in_place_index<2>, ci}; // ill-formed
			static_assert(!models::Constructible<V, in_place_index_t<2>, const int&>);
		}
		{
			V v{in_place_index<2>, move(i)};
			CHECK(v.index() == 2u);
		}
		{
			// V v{in_place_index<2>, move(ci)}; // ill-formed
			static_assert(!models::Constructible<V, in_place_index_t<2>, const int&&>);
		}

		{
			V v{in_place_index<3>, i};
			CHECK(v.index() == 3u);
		}
		{
			V v{in_place_index<3>, ci};
			CHECK(v.index() == 3u);
		}
		{
			V v{in_place_index<3>, move(i)};
			CHECK(v.index() == 3u);
		}
		{
			V v{in_place_index<3>, move(ci)};
			CHECK(v.index() == 3u);
		}

		{
			V v{in_place_index<4>, i};
			CHECK(v.index() == 4u);
		}
		{
			V v{in_place_index<4>, ci};
			CHECK(v.index() == 4u);
		}
		{
			V v{in_place_index<4>, move(i)};
			CHECK(v.index() == 4u);
		}
		{
			V v{in_place_index<4>, move(ci)};
			CHECK(v.index() == 4u);
		}

		{
			// V v{in_place_index<5>, i}; // ill-formed
			static_assert(!models::Constructible<V, in_place_index_t<5>, int&>);
		}
		{
			// V v{in_place_index<5>, ci}; // ill-formed
			static_assert(!models::Constructible<V, in_place_index_t<5>, const int&>);
		}
		{
			V v{in_place_index<5>, move(i)};
			CHECK(v.index() == 5u);
		}
		{
			V v{in_place_index<5>, move(ci)};
			CHECK(v.index() == 5u);
		}
	}

	{
		nontrivial::zero();
		{
			using V = variant<std::vector<nontrivial>>;
			V v{in_place_index<0>, 4u};
			CHECK(v.index() == 0u);
			CHECK(get<0>(v).size() == 4u);
			CHECK(nontrivial::count.create == 4u);
			CHECK(nontrivial::count.move == 0u);
			CHECK(nontrivial::count.copy == 0u);
			CHECK(nontrivial::count.destroy == 0u);
			get<0>(v)[0];
		}
		CHECK(nontrivial::count.create == 4u);
		CHECK(nontrivial::count.move == 0u);
		CHECK(nontrivial::count.copy == 0u);
		CHECK(nontrivial::count.destroy == 4u);
	}

	{
		using V = variant<std::vector<int>>;
		V v{in_place_index<0>, {1,2,3,4}};
		CHECK(v.index() == 0u);
		CHECK(get<0>(v).size() == 4u);
		CHECK(get<0>(v)[2] == 3);
	}
}

void test_in_place_type() {
	{
		using V = variant<int, int&, int&&, const int, const int&, const int&&>;
		int i = 42;
		const int& ci = i;

		{
			V v{in_place_type<int>, i};
			CHECK(holds_alternative<int>(v));
		}
		{
			V v{in_place_type<int>, ci};
			CHECK(holds_alternative<int>(v));
		}
		{
			V v{in_place_type<int>, move(i)};
			CHECK(holds_alternative<int>(v));
		}
		{
			V v{in_place_type<int>, move(ci)};
			CHECK(holds_alternative<int>(v));
		}

		{
			V v{in_place_type<int&>, i};
			CHECK(holds_alternative<int&>(v));
		}
		{
			// V v{in_place_type<int&>, ci}; // ill-formed
			static_assert(!models::Constructible<V, in_place_type_t<int&>, const int&>);
		}
		{
			// V v{in_place_type<int&>, move(i)}; // ill-formed
			static_assert(!models::Constructible<V, in_place_type_t<int&>, int&&>);
		}
		{
			// V v{in_place_type<int&>, move(ci)}; // ill-formed
			static_assert(!models::Constructible<V, in_place_type_t<int&>, const int&&>);
		}

		{
			// V v{in_place_type<int&&>, i}; // ill-formed
			static_assert(!models::Constructible<V, in_place_type_t<int&&>, int&>);
		}
		{
			// V v{in_place_type<int&&>, ci}; // ill-formed
			static_assert(!models::Constructible<V, in_place_type_t<int&&>, const int&>);
		}
		{
			V v{in_place_type<int&&>, move(i)};
			CHECK(holds_alternative<int&&>(v));
		}
		{
			// V v{in_place_type<int&&>, move(ci)}; // ill-formed
			static_assert(!models::Constructible<V, in_place_type_t<int&&>, const int&&>);
		}

		{
			V v{in_place_type<const int>, i};
			CHECK(holds_alternative<const int>(v));
		}
		{
			V v{in_place_type<const int>, ci};
			CHECK(holds_alternative<const int>(v));
		}
		{
			V v{in_place_type<const int>, move(i)};
			CHECK(holds_alternative<const int>(v));
		}
		{
			V v{in_place_type<const int>, move(ci)};
			CHECK(holds_alternative<const int>(v));
		}

		{
			V v{in_place_type<const int&>, i};
			CHECK(holds_alternative<const int&>(v));
		}
		{
			V v{in_place_type<const int&>, ci};
			CHECK(holds_alternative<const int&>(v));
		}
		{
			V v{in_place_type<const int&>, move(i)};
			CHECK(holds_alternative<const int&>(v));
		}
		{
			V v{in_place_type<const int&>, move(ci)};
			CHECK(holds_alternative<const int&>(v));
		}

		{
			// V v{in_place_type<const int&&>, i}; // ill-formed
			static_assert(!models::Constructible<V, in_place_type_t<const int&&>, int&>);
		}
		{
			// V v{in_place_type<const int&&>, ci}; // ill-formed
			static_assert(!models::Constructible<V, in_place_type_t<const int&&>, const int&>);
		}
		{
			V v{in_place_type<const int&&>, move(i)};
			CHECK(holds_alternative<const int&&>(v));
		}
		{
			V v{in_place_type<const int&&>, move(ci)};
			CHECK(holds_alternative<const int&&>(v));
		}
	}

	{
		nontrivial::zero();
		{
			using V = variant<std::vector<nontrivial>>;
			V v{in_place_type<std::vector<nontrivial>>, 4u};
			CHECK(holds_alternative<std::vector<nontrivial>>(v));
			CHECK(get<0>(v).size() == 4u);
			CHECK(nontrivial::count.create == 4u);
			CHECK(nontrivial::count.move == 0u);
			CHECK(nontrivial::count.copy == 0u);
			CHECK(nontrivial::count.destroy == 0u);
			get<0>(v)[0];
		}
		CHECK(nontrivial::count.create == 4u);
		CHECK(nontrivial::count.move == 0u);
		CHECK(nontrivial::count.copy == 0u);
		CHECK(nontrivial::count.destroy == 4u);
	}

	{
		using V = variant<std::vector<int>>;
		V v{in_place_type<std::vector<int>>, {1,2,3,4}};
		CHECK(holds_alternative<std::vector<int>>(v));
		CHECK(get<0>(v).size() == 4u);
		CHECK(get<0>(v)[2] == 3);
	}

	{
		using V = variant<int, int>;
		// V v1{in_place_type<int>}; // ill-formed: ambiguous
		static_assert(!models::Constructible<V, in_place_type_t<int>>);
		// V v2{in_place_type<int>, 42}; // ill-formed: ambiguous
		static_assert(!models::Constructible<V, in_place_type_t<int>, int>);
	}
}

void test_construction() {
	static_assert(models::MoveConstructible<variant<int>>);
	static_assert(models::MoveConstructible<variant<const int>>);
	static_assert(models::CopyConstructible<variant<int>>);
	static_assert(models::CopyConstructible<variant<const int>>);
	static_assert(is_trivially_copy_constructible<variant<int>>());
	static_assert(is_trivially_copy_constructible<variant<const int>>());

	static_assert(models::Movable<variant<int>>);
	static_assert(!models::Movable<variant<const int>>);
	static_assert(models::Copyable<variant<int>>);
	static_assert(!models::Copyable<variant<const int>>);

	{
		nontrivial::zero();
		{
			variant<nontrivial> v;
			CHECK(v.index() == 0u);
			CHECK(holds_alternative<nontrivial>(v));
			CHECK(nontrivial::count.create == 1u);
		}
		CHECK(nontrivial::count.destroy == 1u);
	}
	{
		variant<nontrivial, int, double> vnid;
		CHECK(vnid.index() == 0u);
		CHECK(holds_alternative<nontrivial>(vnid));
		static_assert(!is_trivially_destructible<variant<int, double, nontrivial>>());
	}
	{
		nontrivial::zero();
		variant<nontrivial>{};
		CHECK(nontrivial::count.destroy == 1u);
		variant<nontrivial, int, double>{};
		CHECK(nontrivial::count.destroy == 2u);
	}
	{
		//variant<int, int> v{42}; // ill-formed: ambiguous.
		static_assert(!models::Constructible<variant<int,int>, int&&>);
		variant<int, int> v{in_place_index<1>, 42};
		CHECK(v.index() == 1u);
		//get<int>(v); // ill-formed: ambiguous.
		//CHECK(holds_alternative<int>(v)); // ill-formed: ambiguous.
	}
	{
		int i = 42;
		double d = 3.14;
		variant<int&, double&> vi{i};
		CHECK(holds_alternative<int&>(vi));
		variant<int&, double&> vd{d};
		CHECK(holds_alternative<double&>(vd));
		variant<int&&, double&&> vri{move(i)};
		CHECK(holds_alternative<int&&>(vri));
		variant<int&&, double&&> vrd{move(d)};
		CHECK(holds_alternative<double&&>(vrd));
	}
	{
		int i = 42;
		{
			variant<int> vi{i};
			CHECK(holds_alternative<int>(vi));
			variant<int&> vir{i};
			CHECK(holds_alternative<int&>(vir));
			variant<int&&> virr{move(i)};
			CHECK(holds_alternative<int&&>(virr));
		}
		// variant<int, int&> v1{i}; // ill-formed: ambiguous
		static_assert(!models::Constructible<variant<int,int&>, int&>);
		variant<int, int&> v2{move(i)};
		// variant<int, int&&> v3{move(i)}; // ill-formed: ambiguous
		static_assert(!models::Constructible<variant<int,int&&>, int&&>);
		variant<int, int&&> v4{i};
	}
	{
		moveonly m{};
		auto const& cm = m;
		// variant<moveonly, const moveonly&> v1{move(m)}; // ill-formed: ambiguous
		static_assert(!models::Constructible<variant<moveonly,const moveonly&>, moveonly&&>);
		variant<moveonly, moveonly&> v2{m};
		CHECK(holds_alternative<moveonly&>(v2));
		variant<moveonly, const moveonly&> v3{m};
		CHECK(holds_alternative<const moveonly&>(v3));
		variant<moveonly, const moveonly&> v4{cm};
		CHECK(holds_alternative<const moveonly&>(v4));
		variant<moveonly, moveonly&> v5{moveonly{}};
		CHECK(holds_alternative<moveonly>(v5));
	}

	{
		variant<int&&> v{in_place_index<0>, 42};
		CHECK(holds_alternative<int&&>(v));
		variant<int, int, int, int&&> v2{in_place_index<3>, 42};
		CHECK(holds_alternative<int&&>(v2));
		variant<int&&, int&&, int&&, int&&> v3{in_place_index<2>, 42};
		CHECK(v3.index() == 2u);
	}

	test_in_place_index();
	test_in_place_type();
}

void test_get() {
	{
		variant<int, double> v{42};
		CHECK(holds_alternative<int>(v));
		CHECK(get<0>(v) == 42);
		CHECK(get<int>(v) == 42);
		get<0>(v) = 13;
		CHECK(get<0>(v) == 13);
		CHECK(get<int>(v) == 13);
	}
	{
		variant<int, double> v{3.14};
		CHECK(holds_alternative<double>(v));
		CHECK(get<1>(v) == 3.14);
		CHECK(get<double>(v) == 3.14);
		get<double>(v) = 1.414;
		CHECK(get<1>(v) == 1.414);
		CHECK(get<double>(v) == 1.414);
	}
	{
		variant<int, int> v{in_place_index<1>, 42};
		CHECK(v.index() == 1u);
		CHECK(get<1>(v) == 42);
		//get<int>(v); // ill-formed: not unique.
	}
	{
		constexpr variant<int, double> v1{42};
		constexpr variant<int, double> v2{3.14};
		static_assert(get<0>(v1) == 42);
		static_assert(get<int>(v1) == 42);
		static_assert(get<1>(v2) == 3.14);
		static_assert(get<double>(v2) == 3.14);
	}
}

void test_move_assignment() {
	{
		// trivial
		using V = variant<int, double, int&>;
		static_assert(is_trivially_move_assignable<V>());
		V v;
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 0);
		v = V{42};
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 42);
		v = V{3.14};
		CHECK(holds_alternative<double>(v));
		CHECK(get<double>(v) == 3.14);
		v = V{42};
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 42);
		int i = 42;
		v = V{in_place_type<int&>, i};
		CHECK(holds_alternative<int&>(v));
		CHECK(get<int&>(v) == 42);
		CHECK(&get<int&>(v) == &i);
		int j = 13;
		v = V{in_place_type<int&>, j};
		CHECK(holds_alternative<int&>(v));
		CHECK(get<int&>(v) == 13);
		CHECK(&get<int&>(v) == &j);
	}
	{
		// nontrivial
		using V = variant<int, double, int&, nontrivial>;
		static_assert(!is_trivially_move_assignable<V>());
		V v;
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 0);
		v = V{42};
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 42);
		v = V{3.14};
		CHECK(holds_alternative<double>(v));
		CHECK(get<double>(v) == 3.14);
		v = V{42};
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 42);
		int i = 42;
		v = V{in_place_type<int&>, i};
		CHECK(holds_alternative<int&>(v));
		CHECK(get<int&>(v) == 42);
		CHECK(&get<int&>(v) == &i);
		int j = 13;
		v = V{in_place_type<int&>, j};
		CHECK(holds_alternative<int&>(v));
		CHECK(get<int&>(v) == 13);
		CHECK(&get<int&>(v) == &j);

		nontrivial::zero();
		v = V{in_place_type<nontrivial>};
		CHECK(holds_alternative<nontrivial>(v));
		CHECK(nontrivial::count.move == 1u);
		CHECK(nontrivial::count.move_assign == 0u);
		v = V{in_place_type<nontrivial>};
		CHECK(holds_alternative<nontrivial>(v));
		CHECK(nontrivial::count.move == 1u);
		CHECK(nontrivial::count.move_assign == 1u);
	}

	{
		// Really nontrivial
		using V = variant<int, double, std::vector<nontrivial>>;
		constexpr auto N = std::size_t{42};
		nontrivial::zero();
		V v{in_place_index<2>, N};
		CHECK(v.index() == 2u);
		CHECK(get<2>(v).size() == N);
		CHECK(nontrivial::count.create == N);
		V v2;
		CHECK(v2.index() == 0u);
		v2 = move(v);
		CHECK(v2.index() == 2u);
		CHECK(get<2>(v).empty());
		CHECK(get<2>(v2).size() == N);
		v = move(v2);
		CHECK(get<2>(v2).empty());
		CHECK(get<2>(v).size() == N);
	}
}

void test_copy_assignment() {
	{
		// trivial
		using V = variant<int, double, int&>;
		using CV = const V;
		static_assert(is_trivially_copy_assignable<V>());
		V v;
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 0);
		v = CV{42};
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 42);
		v = CV{3.14};
		CHECK(holds_alternative<double>(v));
		CHECK(get<double>(v) == 3.14);
		v = CV{42};
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 42);
		int i = 42;
		v = CV{in_place_type<int&>, i};
		CHECK(holds_alternative<int&>(v));
		CHECK(get<int&>(v) == 42);
		CHECK(&get<int&>(v) == &i);
		int j = 13;
		v = CV{in_place_type<int&>, j};
		CHECK(holds_alternative<int&>(v));
		CHECK(get<int&>(v) == 13);
		CHECK(&get<int&>(v) == &j);
	}
	{
		// nontrivial
		using V = variant<int, double, int&, nontrivial>;
		using CV = const V;
		static_assert(!is_trivially_move_assignable<V>());
		V v;
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 0);
		v = CV{42};
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 42);
		v = CV{3.14};
		CHECK(holds_alternative<double>(v));
		CHECK(get<double>(v) == 3.14);
		v = CV{42};
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 42);
		int i = 42;
		v = CV{in_place_type<int&>, i};
		CHECK(holds_alternative<int&>(v));
		CHECK(get<int&>(v) == 42);
		CHECK(&get<int&>(v) == &i);
		int j = 13;
		v = CV{in_place_type<int&>, j};
		CHECK(holds_alternative<int&>(v));
		CHECK(get<int&>(v) == 13);
		CHECK(&get<int&>(v) == &j);

		nontrivial::zero();
		v = CV{in_place_type<nontrivial>};
		CHECK(holds_alternative<nontrivial>(v));
		CHECK(nontrivial::count.copy == 1u);
		CHECK(nontrivial::count.copy_assign == 0u);
		v = CV{in_place_type<nontrivial>};
		CHECK(holds_alternative<nontrivial>(v));
		CHECK(nontrivial::count.copy == 1u);
		CHECK(nontrivial::count.copy_assign == 1u);
	}

	{
		// Really nontrivial
		using V = variant<int, double, std::vector<nontrivial>>;
		constexpr auto N = std::size_t{42};
		nontrivial::zero();
		V v{in_place_index<2>, N};
		CHECK(v.index() == 2u);
		CHECK(get<2>(v).size() == N);
		CHECK(nontrivial::count.create == N);
		V v2;
		CHECK(v2.index() == 0u);
		v2 = v;
		CHECK(v2.index() == 2u);
		CHECK(get<2>(v2).size() == N);
		CHECK(get<2>(v).size() == N);
		CHECK(nontrivial::count.copy == N);
		CHECK(nontrivial::count.copy_assign == 0u);
		v2 = v;
		CHECK(v2.index() == 2u);
		CHECK(get<2>(v2).size() == N);
		CHECK(get<2>(v).size() == N);
		CHECK(nontrivial::count.copy == N);
		CHECK(nontrivial::count.copy_assign == N);
	}

	{
		// Test the two-step: nontrivial copy-assignment creates a copy
		// before destroying the current state, and then moves.
		static int state = 0;
		struct target {
			~target() { CHECK(state == 3); ++state; }
			target() { CHECK(state == 0); ++state; }
			target(const target&) { CHECK(false); }
			target(target&&) { CHECK(false); }
			target& operator=(target&&) & { CHECK(false); return *this; }
			target& operator=(const target&) & { CHECK(false); return *this; }
		};
		struct source {
			~source() { CHECK(state >= 5); ++state; }
			source() { CHECK(state == 1); ++state; }
			source(const source&) { CHECK(state == 2); ++state; }
			source(source&&) { CHECK(state == 4); ++state; }
			source& operator=(source&&) & { CHECK(false); return *this; }
			source& operator=(const source&) & { CHECK(false); return *this; }
		};
		using V = variant<target, source>;
		{
			V v1{in_place_type<target>};
			V v2{in_place_type<source>};
			v1 = v2;
		}
		CHECK(state == 8);
	}
}

void test_void() {
	{
		using V = variant<int, void, double>;
		static_assert(is_trivially_destructible<V>());
		static_assert(is_trivially_move_constructible<V>());
		static_assert(is_trivially_copy_constructible<V>());
		static_assert(is_trivially_move_assignable<V>());
		static_assert(is_trivially_copy_assignable<V>());
		V v1{42};
		V{3.14};
		V{in_place_index<0>};
		V{in_place_index<2>};
		// V{in_place_type<void>}; // ill-formed
		static_assert(!models::Constructible<V, in_place_type_t<void>>);
		// V{in_place_index<1>}; // ill-formed
		static_assert(!models::Constructible<V, in_place_index_t<1>>);

		static_assert(sizeof(variant<char>) == sizeof(variant<char, void>));
	}
}

struct print_fn {
	reference_wrapper<std::ostream> os_;

	std::ostream& os() const noexcept {
		return os_.get();
	}

	template <bool B>
	int print(meta::bool_<B>) const {
		os() << ")\n";
		return 0;
	}

	int print(false_type, const auto& first, const auto&...rest) const {
		os() << first;
		return 1 + print(true_type{}, rest...);
	}

	int print(true_type, const auto&...args) const
		requires sizeof...(args) > 0
	{
		os() << ", ";
		return print(false_type{}, args...);
	}

	int operator()(const auto&...args) const {
		os() << '(';
		return print(false_type{}, args...);
	}
};

void test_visit() {
	{
		using V = variant<int, double, void, nontrivial_literal>;
		auto print = print_fn{std::cout};

		CHECK(visit(print, V{42}) == 1);
		CHECK(visit(print, V{3.14}) == 1);
		CHECK(visit(print, V{nontrivial_literal{13}}) == 1);

		CHECK(visit(print, V{42}, V{13}) == 2);
		CHECK(visit(print, V{42}, V{3.14}) == 2);
		CHECK(visit(print, V{42}, V{nontrivial_literal{42}}) == 2);
		CHECK(visit(print, V{3.14}, V{1.414}) == 2);
		CHECK(visit(print, V{3.14}, V{42}) == 2);
		CHECK(visit(print, V{3.14}, V{nontrivial_literal{42}}) == 2);
		CHECK(visit(print, V{nontrivial_literal{42}}, V{13}) == 2);
		CHECK(visit(print, V{nontrivial_literal{42}}, V{3.14}) == 2);

		CHECK(visit(print, V{42}, V{3.14}, V{nontrivial_literal{42}}) == 3);

#if 0
		CHECK(visit(print, V{0}) == 1);
		CHECK(visit(print, V{0}, V{1}) == 2);
		CHECK(visit(print, V{0}, V{1}, V{2}) == 3);
		CHECK(visit(print, V{0}, V{1}, V{2}, V{3}) == 4);
		CHECK(visit(print, V{0}, V{1}, V{2}, V{3}, V{4}) == 5);
		CHECK(visit(print, V{0}, V{1}, V{2}, V{3}, V{4}, V{5}) == 6);
		CHECK(visit(print, V{0}, V{1}, V{2}, V{3}, V{4}, V{5}, V{6}) == 7);
		CHECK(visit(print, V{0}, V{1}, V{2}, V{3}, V{4}, V{5}, V{6}, V{7}) == 8);
		CHECK(visit(print, V{0}, V{1}, V{2}, V{3}, V{4}, V{5}, V{6}, V{7}, V{8}) == 9);
#endif
	}

	{
		using V = variant<int, unsigned, long, unsigned long,
			long long, unsigned long long>;
		auto f = plus<>{};
		static_assert(visit(f, V{42}, V{42}) == 84ull);
		static_assert(visit(f, V{42}, V{42u}) == 84ull);
		static_assert(visit(f, V{42}, V{42l}) == 84ull);
		static_assert(visit(f, V{42}, V{42ul}) == 84ull);
		static_assert(visit(f, V{42}, V{42ll}) == 84ull);
		static_assert(visit(f, V{42}, V{42ull}) == 84ull);

		static_assert(visit(f, V{42u}, V{42}) == 84ull);
		static_assert(visit(f, V{42u}, V{42u}) == 84ull);
		static_assert(visit(f, V{42u}, V{42l}) == 84ull);
		static_assert(visit(f, V{42u}, V{42ul}) == 84ull);
		static_assert(visit(f, V{42u}, V{42ll}) == 84ull);
		static_assert(visit(f, V{42u}, V{42ull}) == 84ull);

		static_assert(visit(f, V{42l}, V{42}) == 84ull);
		static_assert(visit(f, V{42l}, V{42u}) == 84ull);
		static_assert(visit(f, V{42l}, V{42l}) == 84ull);
		static_assert(visit(f, V{42l}, V{42ul}) == 84ull);
		static_assert(visit(f, V{42l}, V{42ll}) == 84ull);
		static_assert(visit(f, V{42l}, V{42ull}) == 84ull);

		static_assert(visit(f, V{42ul}, V{42}) == 84ull);
		static_assert(visit(f, V{42ul}, V{42u}) == 84ull);
		static_assert(visit(f, V{42ul}, V{42l}) == 84ull);
		static_assert(visit(f, V{42ul}, V{42ul}) == 84ull);
		static_assert(visit(f, V{42ul}, V{42ll}) == 84ull);
		static_assert(visit(f, V{42ul}, V{42ull}) == 84ull);

		static_assert(visit(f, V{42ll}, V{42}) == 84ull);
		static_assert(visit(f, V{42ll}, V{42u}) == 84ull);
		static_assert(visit(f, V{42ll}, V{42l}) == 84ull);
		static_assert(visit(f, V{42ll}, V{42ul}) == 84ull);
		static_assert(visit(f, V{42ll}, V{42ll}) == 84ull);
		static_assert(visit(f, V{42ll}, V{42ull}) == 84ull);

		static_assert(visit(f, V{42ull}, V{42}) == 84ull);
		static_assert(visit(f, V{42ull}, V{42u}) == 84ull);
		static_assert(visit(f, V{42ull}, V{42l}) == 84ull);
		static_assert(visit(f, V{42ull}, V{42ul}) == 84ull);
		static_assert(visit(f, V{42ull}, V{42ll}) == 84ull);
		static_assert(visit(f, V{42ull}, V{42ull}) == 84ull);
	}
}

void test_tagged() {
	using V = tagged_variant<tag::in(int), tag::out(double)>;
	static_assert(is_trivially_destructible<V>());
	static_assert(is_trivially_move_constructible<V>());
	static_assert(is_trivially_copy_constructible<V>());
	V v1{42};
	CHECK(get<0>(v1) == 42);
	CHECK(v1.in() == 42);
	V v2{3.14};
	CHECK(get<1>(v2) == 3.14);
	CHECK(v2.out() == 3.14);
}

void test_pointer_get() {
	using V = variant<char, int, double>;
	{
		V v{'c'};
		char* p = __stl2::get<0>(&v);
		CHECK(p == __stl2::get<char>(&v));
		CHECK(p);
		if (p) { CHECK(*p == 'c'); }
		CHECK(!__stl2::get<1>(&v));
		CHECK(!__stl2::get<int>(&v));
		CHECK(!__stl2::get<2>(&v));
		CHECK(!__stl2::get<double>(&v));

		auto const& cv = v;
		auto cp = __stl2::get<0>(&cv);
		CHECK(is_same<decltype(cp), const char*>());
		CHECK(p == cp);
	}
	{
		V v{42};
		CHECK(!__stl2::get<0>(&v));
		CHECK(!__stl2::get<char>(&v));
		int* p = __stl2::get<1>(&v);
		CHECK(p);
		if (p) { CHECK(*p == 42); }
		CHECK(p == __stl2::get<int>(&v));
		CHECK(!__stl2::get<2>(&v));
		CHECK(!__stl2::get<double>(&v));

		auto const& cv = v;
		auto cp = __stl2::get<1>(&cv);
		CHECK(is_same<decltype(cp), const int*>());
		CHECK(p == cp);
	}
	{
		V v{3.14};
		CHECK(!__stl2::get<0>(&v));
		CHECK(!__stl2::get<char>(&v));
		CHECK(!__stl2::get<1>(&v));
		CHECK(!__stl2::get<int>(&v));
		double* p = __stl2::get<2>(&v);
		CHECK(p);
		if (p) { CHECK(*p == 3.14); }
		CHECK(p == __stl2::get<double>(&v));

		auto const& cv = v;
		auto cp = __stl2::get<2>(&cv);
		CHECK(is_same<decltype(cp), const double*>());
		CHECK(p == cp);
	}
}

template <class T, std::size_t I, class...Args>
concept bool EmplaceableIndex =
	requires(T& t, Args&&...args) {
		t.template emplace<I>((Args&&)args...);
	};

template <class, std::size_t, class...>
constexpr bool emplaceable() { return false; }
EmplaceableIndex{T, I, ...Args}
constexpr bool emplaceable() { return true; }

template <class T, class U, class...Args>
concept bool EmplaceableType =
	requires(T& t, Args&&...args) {
		t.template emplace<U>((Args&&)args...);
	};

template <class, class, class...>
constexpr bool emplaceable() { return false; }
template <class T, class U, class...Args>
	requires EmplaceableType<T, U, Args...>
constexpr bool emplaceable() { return true; }

void test_emplace() {
	{
		using V = variant<int, int&, int&&, const int, const int&, const int&&>;
		int i = 42;
		const int& ci = i;

		{
			V v;
			v.emplace<0>(i);
			CHECK(v.index() == 0u);
		}
		{
			V v;
			v.emplace<0>(ci);
			CHECK(v.index() == 0u);
		}
		{
			V v;
			v.emplace<0>(move(i));
			CHECK(v.index() == 0u);
		}
		{
			V v;
			v.emplace<0>(move(ci));
			CHECK(v.index() == 0u);
		}

		{
			V v;
			v.emplace<1>(i);
			CHECK(v.index() == 1u);
		}
		{
			//V v; v.emplace<1>(ci); // ill-formed
			static_assert(!emplaceable<V, 1u, decltype((ci))>());
		}
		{
			//V v; v.emplace<1>(move(i)); // ill-formed
			static_assert(!emplaceable<V, 1u, decltype(move(i))>());
		}
		{
			//V v; v.emplace<1>(move(ci)); // ill-formed
			static_assert(!emplaceable<V, 1u, decltype(move(ci))>());
		}

		{
			//V v; v.emplace<2>(i); // ill-formed
			static_assert(!emplaceable<V, 2u, decltype((i))>());
		}
		{
			//V v; v.emplace<2>(ci); // ill-formed
			static_assert(!emplaceable<V, 2u, decltype((ci))>());
		}
		{
			V v;
			v.emplace<2>(move(i));
			CHECK(v.index() == 2u);
		}
		{
			//V v; v.emplace<2>(move(ci)); // ill-formed
			static_assert(!emplaceable<V, 2u, decltype(move(ci))>());
		}

		{
			V v;
			v.emplace<3>(i);
			CHECK(v.index() == 3u);
		}
		{
			V v;
			v.emplace<3>(ci);
			CHECK(v.index() == 3u);
		}
		{
			V v;
			v.emplace<3>(move(i));
			CHECK(v.index() == 3u);
		}
		{
			V v;
			v.emplace<3>(move(ci));
			CHECK(v.index() == 3u);
		}

		{
			V v;
			v.emplace<4>(i);
			CHECK(v.index() == 4u);
		}
		{
			V v;
			v.emplace<4>(ci);
			CHECK(v.index() == 4u);
		}
		{
			V v;
			v.emplace<4>(move(i));
			CHECK(v.index() == 4u);
		}
		{
			V v;
			v.emplace<4>(move(ci));
			CHECK(v.index() == 4u);
		}

		{
			//V v; v.emplace<5>(i); // ill-formed
			static_assert(!emplaceable<V, 5u, decltype((i))>());
		}
		{
			//V v; v.emplace<5>(ci); // ill-formed
			static_assert(!emplaceable<V, 5u, decltype((ci))>());
		}
		{
			V v;
			v.emplace<5>(move(i));
			CHECK(v.index() == 5u);
		}
		{
			V v;
			v.emplace<5>(move(ci));
			CHECK(v.index() == 5u);
		}
	}

	{
		nontrivial::zero();
		{
			using V = variant<std::vector<nontrivial>>;
			V v;
			v.emplace<0>(std::size_t{4});
			CHECK(v.index() == 0u);
			CHECK(get<0>(v).size() == 4u);
			CHECK(nontrivial::count.create == 4u);
			CHECK(nontrivial::count.move == 0u);
			CHECK(nontrivial::count.copy == 0u);
			CHECK(nontrivial::count.destroy == 0u);
			get<0>(v)[0];
		}
		CHECK(nontrivial::count.create == 4u);
		CHECK(nontrivial::count.move == 0u);
		CHECK(nontrivial::count.copy == 0u);
		CHECK(nontrivial::count.destroy == 4u);
	}

	{
		using V = variant<std::vector<int>>;
		V v;
		v.emplace<0>({1,2,3,4});
		CHECK(v.index() == 0u);
		CHECK(get<0>(v).size() == 4u);
		CHECK(get<0>(v)[2] == 3);
	}

	{
		using V = variant<int, int&, int&&, const int, const int&, const int&&>;
		int i = 42;
		const int& ci = i;

		{
			V v; v.emplace<int>(i);
			CHECK(holds_alternative<int>(v));
		}
		{
			V v; v.emplace<int>(ci);
			CHECK(holds_alternative<int>(v));
		}
		{
			V v; v.emplace<int>(move(i));
			CHECK(holds_alternative<int>(v));
		}
		{
			V v; v.emplace<int>(move(ci));
			CHECK(holds_alternative<int>(v));
		}

		{
			V v; v.emplace<int&>(i);
			CHECK(holds_alternative<int&>(v));
		}
		{
			//V v; v.emplace<int&>(ci); // ill-formed
			static_assert(!emplaceable<V, int&, decltype((ci))>());
		}
		{
			//V v; v.emplace<int&>(move(i)); // ill-formed
			static_assert(!emplaceable<V, int&, decltype(move(i))>());
		}
		{
			//V v; v.emplace<int&>(move(ci)); // ill-formed
			static_assert(!emplaceable<V, int&, decltype(move(ci))>());
		}

		{
			//V v; v.emplace<int&&>(i); // ill-formed
			static_assert(!emplaceable<V, int&&, decltype((i))>());
		}
		{
			//V v; v.emplace<int&&>(ci); // ill-formed
			static_assert(!emplaceable<V, int&&, decltype((ci))>());
		}
		{
			V v; v.emplace<int&&>(move(i));
			CHECK(holds_alternative<int&&>(v));
		}
		{
			//V v; v.emplace<int&&>(move(ci)); // ill-formed
			static_assert(!emplaceable<V, int&&, decltype(move(ci))>());
		}

		{
			V v; v.emplace<const int>(i);
			CHECK(holds_alternative<const int>(v));
		}
		{
			V v; v.emplace<const int>(ci);
			CHECK(holds_alternative<const int>(v));
		}
		{
			V v; v.emplace<const int>(move(i));
			CHECK(holds_alternative<const int>(v));
		}
		{
			V v; v.emplace<const int>(move(ci));
			CHECK(holds_alternative<const int>(v));
		}

		{
			V v; v.emplace<const int&>(i);
			CHECK(holds_alternative<const int&>(v));
		}
		{
			V v; v.emplace<const int&>(ci);
			CHECK(holds_alternative<const int&>(v));
		}
		{
			V v; v.emplace<const int&>(move(i));
			CHECK(holds_alternative<const int&>(v));
		}
		{
			V v; v.emplace<const int&>(move(ci));
			CHECK(holds_alternative<const int&>(v));
		}

		{
			//V v; v.emplace<const int&&>(i); // ill-formed
			static_assert(!emplaceable<V, const int&&, decltype((i))>());
		}
		{
			//V v; v.emplace<const int&&>(ci); // ill-formed
			static_assert(!emplaceable<V, const int&&, decltype((ci))>());
		}
		{
			V v; v.emplace<const int&&>(move(i));
			CHECK(holds_alternative<const int&&>(v));
		}
		{
			V v; v.emplace<const int&&>(move(ci));
			CHECK(holds_alternative<const int&&>(v));
		}
	}

	{
		nontrivial::zero();
		{
			using V = variant<std::vector<nontrivial>>;
			V v; v.emplace<std::vector<nontrivial>>(4u);
			CHECK(holds_alternative<std::vector<nontrivial>>(v));
			CHECK(get<0>(v).size() == 4u);
			CHECK(nontrivial::count.create == 4u);
			CHECK(nontrivial::count.move == 0u);
			CHECK(nontrivial::count.copy == 0u);
			CHECK(nontrivial::count.destroy == 0u);
			get<0>(v)[0];
		}
		CHECK(nontrivial::count.create == 4u);
		CHECK(nontrivial::count.move == 0u);
		CHECK(nontrivial::count.copy == 0u);
		CHECK(nontrivial::count.destroy == 4u);
	}

	{
		using V = variant<std::vector<int>>;
		V v; v.emplace<std::vector<int>>({1,2,3,4});
		CHECK(holds_alternative<std::vector<int>>(v));
		CHECK(get<0>(v).size() == 4u);
		CHECK(get<0>(v)[2] == 3);
	}

	{
		using V = variant<int, int>;
		//V v1; v1.emplace<int>(); // ill-formed: ambiguous
		static_assert(!emplaceable<V, int>());
		//V v2; v2.emplace<int>(42); // ill-formed: ambiguous
		static_assert(!emplaceable<V, int, int>());
	}
}

void test_conversion_assign() {
	variant<int, double, nontrivial_literal> v{0};
	{
		// rvalue assignment
		CHECK(holds_alternative<int>(v));
		v = 42;
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 42);
		v = 3.14;
		CHECK(holds_alternative<double>(v));
		CHECK(get<double>(v) == 3.14);
		v = 1.414;
		CHECK(holds_alternative<double>(v));
		CHECK(get<double>(v) == 1.414);
		v = nontrivial_literal{13};
		CHECK(holds_alternative<nontrivial_literal>(v));
		CHECK(get<nontrivial_literal>(v) == 13);
		v = nontrivial_literal{42};
		CHECK(holds_alternative<nontrivial_literal>(v));
		CHECK(get<nontrivial_literal>(v) == 42);
		v = 13;
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 13);
	}
	{
		// lvalue assignment
		int i1 = 42;
		int i2 = 13;
		double d1 = 3.14;
		double d2 = 1.414;
		nontrivial_literal nt1{13};
		nontrivial_literal nt2{42};

		v = i1;
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 42);
		v = d1;
		CHECK(holds_alternative<double>(v));
		CHECK(get<double>(v) == 3.14);
		v = d2;
		CHECK(holds_alternative<double>(v));
		CHECK(get<double>(v) == 1.414);
		v = nt1;
		CHECK(holds_alternative<nontrivial_literal>(v));
		CHECK(get<nontrivial_literal>(v) == 13);
		v = nt2;
		CHECK(holds_alternative<nontrivial_literal>(v));
		CHECK(get<nontrivial_literal>(v) == 42);
		v = i2;
		CHECK(holds_alternative<int>(v));
		CHECK(get<int>(v) == 13);
	}
	{
		using V = variant<int, int>;
		// V v; v = 42; // ill-formed: ambiguous.
		static_assert(!models::Assignable<V&, int>);
		static_assert(!models::Assignable<V&, int&>);
		static_assert(!models::Assignable<V&, int&&>);
		static_assert(!models::Assignable<V&, const int&>);
		static_assert(!models::Assignable<V&, const int&&>);
	}
	{
		using V = variant<int&>;
		// int i = 42; V v{i}; int j = 13; v = j; // ill-formed
		static_assert(!models::Assignable<V&, int&>);
	}
}

void test_swap() {
	{
		using V = variant<int, float>;
		static_assert(models::Swappable<V>);
		V v1{42};
		V v2{3.14f};
		__stl2::swap(v1, v2);
		CHECK(holds_alternative<float>(v1));
		CHECK(get<float>(v1) == 3.14f);
		CHECK(holds_alternative<int>(v2));
		CHECK(get<int>(v2) == 42);
	}

	static_assert(!models::Swappable<variant<const int>>);

	{
		using V = variant<int&>;
		static_assert(models::Swappable<V>);
		int i = 42;
		int j = 13;
		V v1{i};
		V v2{j};
		__stl2::swap(v1, v2);
		CHECK(&get<0>(v1) == &j);
		CHECK(&get<0>(v2) == &i);
	}

	{
		using V = variant<int,nontrivial>;
		V v1{in_place_type<nontrivial>};
		V v2{v1};
		nontrivial::zero();
		__stl2::swap(v1, v2);
		CHECK(nontrivial::count.swap == 1u);
		v1 = 42;
		nontrivial::zero();
		__stl2::swap(v1, v2);
		CHECK(holds_alternative<nontrivial>(v1));
		CHECK(holds_alternative<int>(v2));
		CHECK(get<int>(v2) == 42);
		CHECK(nontrivial::count.swap == 0u);
		CHECK((nontrivial::count.move >= 1u && nontrivial::count.move <= 2u));
	}
}

void test_comparisons() {
	{
		static_assert(models::EqualityComparable<variant<int, double>>);
		static_assert(models::StrictTotallyOrdered<variant<int, double>>);

		static_assert(models::EqualityComparable<variant<int, void, double>>);
		static_assert(models::StrictTotallyOrdered<variant<int, void, double>>);

		struct bar {
			bool operator==(const bar&) const;
			bool operator!=(const bar&) const;
		};
		static_assert(models::EqualityComparable<variant<int, bar>>);
		static_assert(!models::StrictTotallyOrdered<variant<int, bar>>);

		struct foo {};
		static_assert(!models::EqualityComparable<variant<int, foo>>);
		static_assert(!models::StrictTotallyOrdered<variant<int, foo>>);

		static_assert(models::EqualityComparable<variant<int&, int&&>>);
		static_assert(models::StrictTotallyOrdered<variant<int&, int&&>>);
	}

	{
		using V = variant<int, void, double>;
		CHECK(V{42} == V{42});
		CHECK(V{42} != V{3.14});
		CHECK(V{3.14} != V{42});
		CHECK(V{3.14} == V{3.14});
		CHECK(V{0} != V{0.0});
		CHECK(V{42} <= V{42});
		CHECK(V{42} < V{3.14});
		CHECK(V{3.14} >= V{3.14});
	}

	{
		using V = variant<int, int>;
		CHECK(V{in_place_index<0>, 42} != V{in_place_index<1>, 42});
	}

	{
		int i = 42;
		int j = 42;
		using V = variant<int&>;
		V v1{i}, v2{j};
		CHECK(v1 == v2);
		j = 13;
		CHECK(v1 != v2);
	}

	{
		static_assert(!models::EqualityComparableWith<variant<int>, variant<long>>);
	}
}

void test_hash() {
	static_assert(__variant::HashableVariant<int, void, double>);
	static_assert(!__variant::HashableVariant<int, void, double&>);
	using V = variant<monostate, int, void, double>;
	std::hash<V> h;
	h(V{42});
	h(V{3.14});
	h(V{});
}

void test_n4542_examples() {
	{
		variant<int, float> v, w;
		v = 12;
		int i = get<int>(v);
		w = get<int>(v);
		w = get<0>(v); // same effect as the previous line
		w = v; // same effect as the previous line
		// get<double>(v); // ill formed
		// get<3>(v); // ill formed
		try {
			get<float>(w); // will throw.
			CHECK(false);
		} catch (bad_variant_access&) {}
		(void)i;
	}
}
} // unnamed namespace

int main() {
	test_raw();
	test_construction();
	test_get();
	test_move_assignment();
	test_copy_assignment();

	{
		// variant<>{}; // ill-formed
		static_assert(!models::DefaultConstructible<variant<>>);
		static_assert(!models::DefaultConstructible<variant<int&>>);
		static_assert(models::MoveConstructible<variant<int, double, nontrivial>>);
		static_assert(models::MoveConstructible<variant<int, double, moveonly>>);
		// variant<nonmovable>{variant<nonmovable>{}}; // ill-formed
		static_assert(!models::MoveConstructible<variant<nonmovable>>);
		static_assert(models::CopyConstructible<variant<int, double, nontrivial>>);
		// { variant<moveonly> v; auto c = v; } // ill-formed
		static_assert(!models::CopyConstructible<variant<moveonly>>);
		// { variant<nonmovable> v; auto c = v; } // ill-formed
		static_assert(!models::CopyConstructible<variant<nonmovable>>);
	}

	{
		int i = 42;
		variant<int&> vir{in_place_index<0>, i};
		CHECK(vir.index() == 0u);
		CHECK(holds_alternative<int&>(vir));
		CHECK(&get<0>(vir) == &i);
		CHECK(&get<int&>(vir) == &i);
		static_assert(is_trivially_destructible<variant<int&>>());
	}

	{
		constexpr variant<int> vi{in_place_index<0>, 42};
		static_assert(vi.index() == 0u);
		static_assert(holds_alternative<int>(vi));
		CHECK(get<0>(vi) == 42);
		CHECK(get<int>(vi) == 42);
		CHECK(&get<0>(vi) == &get<int>(vi));
	}
	{
		constexpr variant<double, int> vdi{in_place_index<1>, 42};
		static_assert(vdi.index() == 1);
		static_assert(holds_alternative<int>(vdi));
		static_assert(is_trivially_destructible<variant<int, double>>());
	}

	{
		static_assert(is_trivially_copyable<variant<int, double>>());
		static_assert(is_trivially_move_constructible<variant<int, double>>());
		variant<int, double>{variant<int, double>{}};
	}

	{
		static_assert(!is_trivially_copyable<variant<int, double, nontrivial>>());
		static_assert(is_copy_constructible<variant<int, double, nontrivial>>());
		static_assert(!is_trivially_move_constructible<variant<int, double, nontrivial>>());
		nontrivial::count.copy = 0;
		variant<int, double, nontrivial> v{};
		CHECK(v.index() == 0u);
		CHECK(holds_alternative<int>(v));
		auto copy1 = v;
		CHECK(copy1.index() == 0u);
		CHECK(holds_alternative<int>(copy1));
		auto copy2 = copy1;
		CHECK(copy2.index() == 0u);
		CHECK(holds_alternative<int>(copy2));
	}

	{
		constexpr variant<int> v{in_place_index<0>, 42};
		CHECK(v.index() == 0u);
		CHECK(holds_alternative<int>(v));
		constexpr variant<int> c = v;
		CHECK(c.index() == 0u);
		CHECK(holds_alternative<int>(c));
	}

	{
		nontrivial::count.move = 0u;
		nontrivial::count.copy = 0u;
		variant<nontrivial, int, double> v;
		auto move1 = __stl2::move(v);
		auto move2 = __stl2::move(move1);
		CHECK(nontrivial::count.move == 2u);
		CHECK(nontrivial::count.copy == 0u);
	}

	{
		using V = variant<nontrivial_literal>;
		constexpr V v1{nontrivial_literal{42}};
		// constexpr V v2 = move(v1); // This seems to be unimplementable.
		// constexpr V v3 = v2; // This seems to be unimplementable.
	}

	test_void();
	test_visit();
	test_tagged();
	test_pointer_get();
	test_emplace();
	test_conversion_assign();
	test_swap();
	test_comparisons();
	test_hash();
	test_n4542_examples();

	return ::test_result();
}

#if 0
void f(auto...args);

// The triviality goal ensures that trivial variants are passed by value in
// registers on x64. This function optimizes to two register moves:
VDI v_test(VDI source) {
	return source;
}

// This is two stores:
void v_assign(VDI& target, VDI source) {
	target = move(source);
}

int test_get(variant<int, double> v) {
	return __stl2::get<0>(v);
}

int test_guarded_get(variant<int, double> v) {
	return v.index() == 0 ? __stl2::get<0>(v) : 0;
}

using VV = variant<char, void, short, int>;

void test_foo_u(VV a) {
	visit([](auto&& t){ f(t); }, a);
}

void test_foo_b(VV a, VV b) {
	visit([](auto&& t, auto&& u){ f(t, u); }, a, b);
}

void test_bar() {
	test_foo_b(VV{42}, VV{'a'});
}

extern "C" void test_destroy(variant<char,int,void,nontrivial>& v) {
	v.~variant();
}

using VVV = variant<int, void, const int, void, void, void, char, void, double, long long, float>;
void test_void_visit(VVV v) {
	auto fn = [](auto&& t) -> auto&& {
		f(t);
		return __stl2::forward<decltype(t)>(t);
	};
	visit(fn, v);
}

void test_simple_assign(variant<char, int>& v) {
	// TODO: figure out how to eliminate the unnecessary branch generated here.
	v = 'a';
}

void test_known() {
	test_void_visit(VVV{42LL});
}
#endif
