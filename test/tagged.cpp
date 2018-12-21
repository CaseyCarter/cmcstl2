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
#include <array>
#include <stl2/functional.hpp>
#include <stl2/tuple.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/utility.hpp>
#include "simple_test.hpp"

using TP = __stl2::tagged_pair<__stl2::tag::in(int), __stl2::tag::out(double)>;
using TT = __stl2::tagged_tuple<__stl2::tag::in(int), __stl2::tag::out(double)>;

constexpr auto tp(int i, double d) {
	return TP{i, d};
}

constexpr auto tt(int i, double d) {
	return TT{i, d};
}

void test_pair() {
	auto t = tp(42, 3.14);
	static_assert(__stl2::Same<decltype(t), TP>);
	CHECK(t.first == 42);
	CHECK(std::get<0>(t) == 42);
	CHECK(t.in() == 42);
	CHECK(t.second == 3.14);
	CHECK(std::get<1>(t) == 3.14);
	CHECK(t.out() == 3.14);
}

void test_constexpr_pair() {
	constexpr auto t = tp(42, 3.14);
	static_assert(__stl2::Same<decltype(t), const TP>);
	constexpr int f = t.first;
	CHECK(f == 42);
	constexpr int fg = std::get<0>(t);
	CHECK(fg == 42);
	constexpr int ff = t.in();
	CHECK(ff == 42);
	constexpr double s = t.second;
	CHECK(s == 3.14);
	constexpr double sg = std::get<1>(t);
	CHECK(sg == 3.14);
	constexpr double sf = t.out();
	CHECK(sf == 3.14);
}

void test_tuple() {
	auto t = tt(42, 3.14);
	static_assert(__stl2::Same<decltype(t), TT>);
	CHECK(std::get<0>(t) == 42);
	CHECK(t.in() == 42);
	CHECK(std::get<1>(t) == 3.14);
	CHECK(t.out() == 3.14);
}

void test_constexpr_tuple() {
	constexpr auto t = tt(42, 3.14);
	static_assert(__stl2::Same<decltype(t), const TT>);
	constexpr int fg = std::get<0>(t);
	CHECK(fg == 42);
	constexpr int ff = t.in();
	CHECK(ff == 42);
	constexpr double sg = std::get<1>(t);
	CHECK(sg == 3.14);
	constexpr double sf = t.out();
	CHECK(sf == 3.14);
}

void test_conversions() {
	auto t = tt(42, 3.14);
	CHECK(t == tt(42, 3.14));
	t = tt(13, 1.414);
	CHECK(t == tt(13, 1.414));
	t = tp(42, 3.14);
	CHECK(t == tt(42, 3.14));
	t = std::make_tuple(13, 1.414);
	CHECK(t == tt(13, 1.414));
	t = std::make_pair(42, 3.14);
	CHECK(t == tt(42, 3.14));
}

void test_swap() {
	auto t1 = tt(42, 3.14);
	decltype(t1) t2{};

	std::pair<int, double> stdp;
	__stl2::swap(t1, t2);

	CHECK(t1.in() == 0);
	CHECK(t1.out() == 0);
	CHECK(t2.in() == 42);
	CHECK(t2.out() == 3.14);
}

template<class T> T& lv();

struct nothrow_pair {
	int first;
	int second;

	nothrow_pair() = default;
	constexpr nothrow_pair(int f, int s) noexcept :
		first{f}, second{s} {}
};

namespace std {
	template<>
	struct tuple_size<nothrow_pair> : meta::size_t<2> {};
	template<std::size_t N>
		requires N < 2
	struct tuple_element<N, nothrow_pair> : meta::id<int> {};
}

void test_noexcept() {
	using T = __stl2::tagged<nothrow_pair, __stl2::tag::in1, __stl2::tag::in2>;
	CHECK(std::is_nothrow_default_constructible<T>());
	CHECK(std::is_nothrow_move_constructible<T>());
	CHECK(std::is_nothrow_copy_constructible<T>());
	CHECK(std::is_nothrow_move_assignable<T>());
	CHECK(std::is_nothrow_copy_assignable<T>());
	CHECK(noexcept(T{1, 2}));
	// Force to rvalue to workaround
	// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67185
	CHECK(bool(__stl2::ext::is_nothrow_swappable_v<T&, T&>));
}

// Why? Because we can.
void test_array() {
	using T = __stl2::tagged<std::array<int, 3>, __stl2::tag::in, __stl2::tag::in1, __stl2::tag::in2>;
	T t;
	t = std::array<int, 3>{{0, 1, 2}};
	CHECK(t.in() == 0);
	CHECK(t.in1() == 1);
	CHECK(t.in2() == 2);
}

void test_make_pair() {
#define MAKE(a, b) __stl2::make_tagged_pair<__stl2::tag::in, __stl2::tag::out>(a, b)
	static_assert(__stl2::Same<TP, decltype(MAKE(42, 3.14))>);
	CHECK(tp(42, 3.14) == MAKE(42, 3.14));
	CHECK(MAKE(42, 3.14).in() == 42);
	CHECK(MAKE(42, 3.14).out() == 3.14);
	CHECK(std::pair<int, double>(42, 3.14) == MAKE(42, 3.14));
#undef MAKE
}

void test_make_tuple() {
	using namespace __stl2::tag;
	auto t = __stl2::make_tagged_tuple<in, out, max>(1,2,3);
	static_assert(__stl2::Same<__stl2::tagged_tuple<in(int), out(int), max(int)>, decltype(t)>);
	CHECK(t.in() == 1);
	CHECK(t.out() == 2);
	CHECK(t.max() == 3);
	CHECK(t == std::make_tuple(1,2,3));
}

void test_tagged_pairs_para_2_example() {
	using namespace __stl2;
	tagged_pair<tag::min(int), tag::max(int)> p{0, 1};
	CHECK(&p.min() == &p.first);
	CHECK(&p.max() == &p.second);
}

void test_tagged_pairs_creation_para_2_example() {
	using namespace __stl2;
	const auto a = make_tagged_pair<tag::min, tag::max>(5, 3.1415926);
	const auto b = tagged_pair<tag::min(int), tag::max(double)>{5, 3.1415926};
	static_assert(Same<decltype(a), decltype(b)>);
	CHECK(a == b);
}

void test_tagged_tuple_para_3_example() {
	using namespace __stl2;
	tagged_tuple<tag::in(char*), tag::out(char*)> t{0, 0};
	CHECK(&t.in() == &std::get<0>(t));
	CHECK(&t.out() == &std::get<1>(t));
}

void test_tagged_tuple_creation_para_4_example() {
	using namespace __stl2;
	int i;
	float j;
	auto t = make_tagged_tuple<tag::in1, tag::in2, tag::out>(1, ref(i), cref(j));
	static_assert(Same<decltype(t), tagged_tuple<tag::in1(int), tag::in2(int&), tag::out(const float&)>>);
	CHECK(t.in1() == 1);
	CHECK(&t.in2() == &i);
	CHECK(&t.out() == &j);
}

STL2_DEFINE_GETTER(foo)

void test_tag_extension() {
	auto p = __stl2::make_tagged_pair<__stl2::tag::in, foo>(42, 13);
	static_assert(std::is_same<int&, decltype(p.foo())>());
	const auto& cp = p;
	static_assert(std::is_same<const int&, decltype(cp.foo())>());
	static_assert(std::is_same<int&&, decltype(std::move(p).foo())>());
	CHECK(p.in() == 42);
	CHECK(p.foo() == 13);
}

template<class T>
struct im_a_tuple {
	T i = 42;

	void out();
};

template<int I, class T> requires (I == 0) constexpr auto&
get(im_a_tuple<T>& f) { return f.i; }

template<int I, class T> requires (I == 0) constexpr auto&
get(const im_a_tuple<T>& f) { return f.i; }

template<int I, class T> requires (I == 0) constexpr auto&&
get(im_a_tuple<T>&& f) { return std::move(f).i; }

template<int I, class T> requires (I == 0) constexpr auto&&
get(const im_a_tuple<T>&& f) { return std::move(f).i; }

namespace std {
	template<class T>
	struct tuple_size<::im_a_tuple<T>> : meta::size_t<1> {};
	template<class T>
	struct tuple_element<0, ::im_a_tuple<T>> { using type = T; };
}

void test_chain_inheritance() {
	using T = __stl2::tagged<im_a_tuple<int>, __stl2::tag::out>;
	T t{};
	CHECK(t.out(), 42);
}

constexpr bool test_constexpr() {
	using T = __stl2::tagged<im_a_tuple<int>, __stl2::tag::in>;
	T t1, t2;
	if (t1.in() != 42) return false;
	if (t2.in() != 42) return false;
	t2.in() = 13;
	if (t2.in() != 13) return false;
	__stl2::swap(t1, t2);
	if (t1.in() != 13) return false;
	if (t2.in() != 42) return false;
	t1.swap(t2);
	if (t1.in() != 42) return false;
	if (t2.in() != 13) return false;
	return true;
}
static_assert(test_constexpr());

int main() {
	test_pair();
	test_tuple();
	test_constexpr_pair();
	test_constexpr_tuple();
	test_conversions();
	test_swap();
	test_noexcept();
	test_array();
	test_make_pair();
	test_make_tuple();
	test_tagged_pairs_para_2_example();
	test_tagged_pairs_creation_para_2_example();
	test_tagged_tuple_para_3_example();
	test_tagged_tuple_creation_para_4_example();
	test_tag_extension();
	test_chain_inheritance();
	return ::test_result();
}
