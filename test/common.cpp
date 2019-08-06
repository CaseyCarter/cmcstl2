// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/type_traits.hpp>

#include <array>
#include <functional>
#include <initializer_list>
#include <tuple>
#include <utility>

#include "simple_test.hpp"

using std::tuple;
using std::is_same;
using std::is_same_v;
using std::false_type;
using std::true_type;
using namespace __stl2;

static_assert(is_same_v<common_type_t<int, short&, int, char>, int>);
static_assert(!meta::is_trait<common_type<int, short, int, char*>>());

struct X {};
struct Y { explicit Y(X){} };

STL2_OPEN_NAMESPACE {
template<> struct common_type<X, Y> { typedef Y type; };
template<> struct common_type<Y, X> { typedef Y type; };
} STL2_CLOSE_NAMESPACE

static_assert(is_same_v<common_type_t<X, Y>, Y>);    // (A)
static_assert(is_same_v<common_type_t<X, Y, Y>, Y>); // (B)
static_assert(is_same_v<common_type_t<X, X, Y>, Y>); // (C)

struct AA {
	AA() = default;
	AA(AA &&) = delete;
	AA(AA const &) = delete;
};
struct BB : AA { };

static_assert(is_same_v<common_type_t<AA, BB>, AA>); // (C)

static_assert(is_same_v<common_reference_t<int &&, int const &, int volatile &>, int const volatile &>);
static_assert(is_same_v<common_reference_t<int &&, int const &, float &>, float>);
static_assert(!meta::is_trait<common_reference<int, short, int, char*>>());

STL2_OPEN_NAMESPACE {
template<class... T, class... U, template<class> class TQual, template<class> class UQual>
	requires (meta::Valid<common_reference_t, TQual<T>, UQual<U>> && ...)
struct basic_common_reference<tuple<T...>, tuple<U...>, TQual, UQual> {
	using type = tuple<common_reference_t<TQual<T>, UQual<U>>...>;
};
} STL2_CLOSE_NAMESPACE

static_assert(is_same_v<
	common_reference_t<const tuple<int, short> &, tuple<int&,short volatile&>>,
	tuple<const int&,const volatile short&>>);

static_assert(is_same_v<
	common_reference_t<volatile tuple<int, short> &, const tuple<int,short>&>,
	const volatile tuple<int, short>&>);

static_assert(!meta::is_trait<
	common_reference<volatile tuple<short> &, const tuple<int,short>&>>());

struct B {};
struct D : B {};

struct noncopyable
{
	noncopyable() = default;
	noncopyable(noncopyable const &) = delete;
	noncopyable(noncopyable &&) = default;
	noncopyable &operator=(noncopyable const &) = delete;
	noncopyable &operator=(noncopyable &&) = default;
};

struct noncopyable2 : noncopyable
{};

static_assert(is_same_v<common_reference_t<B &, D &>, B &>);
static_assert(is_same_v<common_reference_t<B &, D const &>, B const &>);
static_assert(is_same_v<common_reference_t<B &, D const &, D &>, B const &>);
static_assert(is_same_v<common_reference_t<B const &, D &>, B const &>);
static_assert(is_same_v<common_reference_t<B &, D &, B &, D &>, B &>);

static_assert(is_same_v<common_reference_t<B &&, D &&>, B &&>);
static_assert(is_same_v<common_reference_t<B const &&, D &&>, B const &&>);
static_assert(is_same_v<common_reference_t<B &&, D const &&>, B const &&>);

static_assert(is_same_v<common_reference_t<B &, D &&>, B const &>);
static_assert(is_same_v<common_reference_t<B &, D const &&>, B const &>);
static_assert(is_same_v<common_reference_t<B const &, D &&>, B const &>);

static_assert(is_same_v<common_reference_t<B &&, D &>, B const &>);
static_assert(is_same_v<common_reference_t<B &&, D const &>, B const &>);
static_assert(is_same_v<common_reference_t<B const &&, D &>, B const &>);

static_assert(is_same_v<common_reference_t<int, short>, int>);
static_assert(is_same_v<common_reference_t<int, short &>, int>);
static_assert(is_same_v<common_reference_t<int &, short &>, int>);
static_assert(is_same_v<common_reference_t<int &, short>, int>);

// tricky volatile reference case
static_assert(is_same_v<common_reference_t<int &&, int volatile &>, int>);
static_assert(is_same_v<common_reference_t<int volatile &, int &&>, int>);
static_assert(is_same_v<common_reference_t<int const volatile &&, int volatile &&>, int const volatile &&>);
static_assert(is_same_v<common_reference_t<int &&, int const &, int volatile &>, int const volatile &>);

// Array types?? Yup!
static_assert(is_same_v<common_reference_t<int (&)[10], int (&&)[10]>, int const(&)[10]>);
static_assert(is_same_v<common_reference_t<int const (&)[10], int volatile (&)[10]>, int const volatile(&)[10]>);
static_assert(is_same_v<common_reference_t<int (&)[10], int (&)[11]>, int *>);

// Some tests with noncopyable types
static_assert(is_same_v<
	common_reference_t<noncopyable const &, noncopyable>,
	noncopyable>);

static_assert(is_same_v<
	common_reference_t<noncopyable2 const &, noncopyable>,
	noncopyable>);

static_assert(is_same_v<
	common_reference_t<noncopyable const &, noncopyable2>,
	noncopyable>);

struct X2 {};
struct Y2 {};
struct Z2 {
	explicit Z2(X2);
	explicit Z2(Y2);
};

STL2_OPEN_NAMESPACE {
template<>
struct common_type<X2, Y2>
{
	using type = Z2;
};
template<>
struct common_type<Y2, X2>
{
	using type = Z2;
};
} STL2_CLOSE_NAMESPACE

static_assert(is_same_v<common_type_t<X2 &, Y2 const &>, Z2>);
static_assert(is_same_v<common_reference_t<X2 &, Y2 const &>, Z2>);

static_assert(common_reference_with<void, void>);
static_assert(is_same_v<common_reference_t<void, void>, void>);
static_assert(common_with<void, void>);
static_assert(is_same_v<common_type_t<void, void>, void>);

static_assert(is_same_v<common_type_t<reference_wrapper<int>, int>, int>);
static_assert(is_same_v<common_type_t<std::reference_wrapper<int>, int>, int>);

// https://github.com/ericniebler/stl2/issues/338
struct MyIntRef {
  MyIntRef(int &);
};
static_assert(is_same_v<common_reference_t<int&, MyIntRef>, MyIntRef>);
static_assert(is_same_v<common_reference_t<int, int, int>, int>);

// Test cases taken from libc++
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
namespace libcpp_tests
{
	struct E {};

	template<class T>
	struct X { explicit X(T const&){} };

	template<class T>
	struct S { explicit S(T const&){} };
}
STL2_OPEN_NAMESPACE {
	template<typename T>
	struct common_type<T, libcpp_tests::S<T>>
	{
		typedef libcpp_tests::S<T> type;
	};
	template<typename T>
	struct common_type<libcpp_tests::S<T>, T>
	{
		typedef libcpp_tests::S<T> type;
	};
} STL2_CLOSE_NAMESPACE
namespace libcpp_tests
{
	template<class T, class U>
	struct no_common_type : true_type {};

	template<class T, class U>
	requires requires { typename common_type_t<T, U>; }
	struct no_common_type<T, U> : false_type {};

    static_assert((is_same<common_type_t<int>, int>::value), "");
    static_assert((is_same<common_type_t<char>, char>::value), "");
    static_assert((is_same<common_type_t<int>,   int>::value), "");
    static_assert((is_same<common_type_t<char>, char>::value), "");

    static_assert((is_same<common_type_t<               int>, int>::value), "");
    static_assert((is_same<common_type_t<const          int>, int>::value), "");
    static_assert((is_same<common_type_t<      volatile int>, int>::value), "");
    static_assert((is_same<common_type_t<const volatile int>, int>::value), "");

    static_assert((is_same<common_type_t<int,           int>, int>::value), "");
    static_assert((is_same<common_type_t<int,     const int>, int>::value), "");

    static_assert((is_same<common_type_t<long,       const int>, long>::value), "");
    static_assert((is_same<common_type_t<const long,       int>, long>::value), "");
    static_assert((is_same<common_type_t<long,    volatile int>, long>::value), "");
    static_assert((is_same<common_type_t<volatile long,    int>, long>::value), "");
    static_assert((is_same<common_type_t<const long, const int>, long>::value), "");

    static_assert((is_same<common_type_t<double, char>, double>::value), "");
    static_assert((is_same<common_type_t<short, char>, int>::value), "");
    static_assert((is_same<common_type_t<double, char>, double>::value), "");
    static_assert((is_same<common_type_t<short, char>, int>::value), "");

    static_assert((is_same<common_type_t<double, char, long long>, double>::value), "");
    static_assert((is_same<common_type_t<unsigned, char, long long>, long long>::value), "");
    static_assert((is_same<common_type_t<double, char, long long>, double>::value), "");
    static_assert((is_same<common_type_t<unsigned, char, long long>, long long>::value), "");

    static_assert((is_same<common_type_t<               void>, void>::value), "");
    static_assert((is_same<common_type_t<const          void>, void>::value), "");
    static_assert((is_same<common_type_t<      volatile void>, void>::value), "");
    static_assert((is_same<common_type_t<const volatile void>, void>::value), "");

    static_assert((is_same<common_type_t<void,       const void>, void>::value), "");
    static_assert((is_same<common_type_t<const void,       void>, void>::value), "");
    static_assert((is_same<common_type_t<void,    volatile void>, void>::value), "");
    static_assert((is_same<common_type_t<volatile void,    void>, void>::value), "");
    static_assert((is_same<common_type_t<const void, const void>, void>::value), "");

    static_assert((no_common_type<void, int>::value), "");
    static_assert((no_common_type<int, void>::value), "");
    static_assert((no_common_type<int, E>::value), "");
    static_assert((no_common_type<int, X<int> >::value), "");

    static_assert((is_same<common_type_t<int, S<int> >, S<int> >::value), "");
    static_assert((is_same<common_type_t<int, S<int>, S<int> >, S<int> >::value), "");
    static_assert((is_same<common_type_t<int, int, S<int> >, S<int> >::value), "");
}

// libstdc++ tests
//
// Copyright (C) 2009-2016 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.
STL2_OPEN_NAMESPACE {
	typedef int test_type1;
	typedef int& test_type2;
	typedef double test_type3;
	typedef float test_type4;
	typedef void test_type5;
	typedef const void test_type6;

	template struct common_type<test_type1>;
	template struct common_type<test_type1, test_type2>;
	template struct common_type<test_type1, test_type2, test_type3>;
	template struct common_type<test_type1, test_type2, test_type3, test_type4>;

	template struct common_type<test_type5>;
	template struct common_type<test_type5, test_type6>;
} STL2_CLOSE_NAMESPACE

namespace libstdcpp_tests
{
	template<typename T, typename Expected>
	struct is_type : std::false_type {};

	template<meta::Trait T, typename Expected>
		requires same_as<meta::_t<T>, Expected>
	struct is_type<T, Expected> : std::true_type {};

	// Inspection types:

	struct S {};

	struct B {};
	struct D : B {};

	struct F1 { operator void*(); };
	struct F2 { operator void*(); };

	struct G1 { operator const void*(); };
	struct G2 { operator volatile void*(); };

	template<typename T>
	struct ImplicitTo
	{
		operator T();
	};

	template<typename T>
	struct ExplicitTo
	{
		explicit operator T();
	};

	template<typename T>
	struct PrivateImplicitTo
	{
	private:
		operator T();
	};

	auto lmd1 = [](int, double) {};
	auto lmd2 = [](int, double) {};

	struct Abstract
	{
		virtual ~Abstract() = 0;
	};

	enum class ScEn;

	enum UnscEn : int;

	struct Ukn;

	union U
	{
		int i;
	};

	union U2
	{
		long i;
	};

	union UConv1
	{
		operator Abstract*();
	};

	union UConv2
	{
		operator Abstract*();
	};

#if 0 // All test cases involving these types are ill-formed NDR since they
      // specialize common_type<X, Y> differently from common_type<Y, X>
	struct X1 {};
	struct X2 {};
	struct RX12 {};
	struct RX21 {};
	struct Y1 {};
	struct Y2 {};
	struct Y3 {};
	struct Y4 {};
#endif // IFNDR test cases
}

#if 0 // These are all ill-formed NDR
STL2_OPEN_NAMESPACE {
	template<>
	struct common_type<libstdcpp_tests::X1, libstdcpp_tests::X2>
	{
		typedef libstdcpp_tests::RX12 type;
	};

	template<>
	struct common_type<libstdcpp_tests::X2, libstdcpp_tests::X1>
	{
		typedef libstdcpp_tests::RX12 type;
	};

	template<>
	struct common_type<libstdcpp_tests::RX12, libstdcpp_tests::X1>
	{
		typedef libstdcpp_tests::Y1 type;
	};

	template<>
	struct common_type<libstdcpp_tests::X1, libstdcpp_tests::RX12>
	{
		typedef libstdcpp_tests::Y1 type;
	};

	template<>
	struct common_type<libstdcpp_tests::RX21, libstdcpp_tests::X1>
	{
		typedef libstdcpp_tests::Y3 type;
	};

	template<>
	struct common_type<libstdcpp_tests::X1, libstdcpp_tests::RX21>
	{
		typedef libstdcpp_tests::Y3 type;
	};
} STL2_CLOSE_NAMESPACE
#endif // IFNDR test cases

namespace libstdcpp_tests
{
	static_assert(is_type<common_type<int, int>, int>(), "");
	static_assert(is_type<common_type<ScEn, ScEn>, ScEn>(), "");
	static_assert(is_type<common_type<UnscEn, UnscEn>, UnscEn>(), "");
	static_assert(is_type<common_type<UnscEn, int>, int>(), "");
	static_assert(is_type<common_type<int, int, int>, int>(), "");
	static_assert(is_type<common_type<int, int, int, int>, int>(), "");
	static_assert(is_type<common_type<int, int, int, int, int>, int>(), "");
	static_assert(is_type<common_type<S, S>, S>(), "");
	static_assert(is_type<common_type<const S, const S>, S>(), "");
	static_assert(is_type<common_type<std::initializer_list<int>,
			std::initializer_list<int>>, std::initializer_list<int>>(), "");
	static_assert(is_type<common_type<B, D>, B>(), "");
	static_assert(is_type<common_type<D, B>, B>(), "");
	static_assert(is_type<common_type<F1, F2>, void*>(), "");
	static_assert(is_type<common_type<F2, F1>, void*>(), "");
	static_assert(is_type<common_type<G1, G2>, const volatile void*>(), "");
	static_assert(is_type<common_type<G2, G1>, const volatile void*>(), "");
	static_assert(is_type<common_type<int*, const volatile int*>,
			const volatile int*>(), "");
	static_assert(is_type<common_type<void*, const volatile int*>,
			const volatile void*>(), "");
	static_assert(is_type<common_type<void, void>, void>(), "");
	static_assert(is_type<common_type<const void, const void>, void>(), "");
	static_assert(is_type<common_type<int&, int&&>, int>(), "");
	static_assert(is_type<common_type<int&, int&>, int>(), "");
	static_assert(is_type<common_type<int&&, int&&>, int>(), "");
	static_assert(is_type<common_type<int&&, const int&&>, int>(), "");
	static_assert(is_type<common_type<U&, const U&&>, U>(), "");
	static_assert(is_type<common_type<U&, U&>, U>(), "");
	static_assert(is_type<common_type<U&&, U&&>, U>(), "");
	static_assert(is_type<common_type<int B::*, int D::*>, int D::*>(), "");
	static_assert(is_type<common_type<int D::*, int B::*>, int D::*>(), "");
	static_assert(is_type<common_type<const int B::*, volatile int D::*>,
			const volatile int D::*>(), "");
	static_assert(is_type<common_type<int (B::*)(), int (D::*)()>,
			int (D::*)()>(), "");
	static_assert(is_type<common_type<int (B::*)() const, int (D::*)() const>,
			int (D::*)() const>(), "");
	static_assert(is_type<common_type<int[3], int[3]>, int*>(), "");
	static_assert(is_type<common_type<int[1], const int[3]>,
			const int*>(), "");
	static_assert(is_type<common_type<void(), void()>, void(*)()>(), "");
	static_assert(is_type<common_type<void(&)(), void(&)()>, void(*)()>(), "");
	static_assert(is_type<common_type<void(&)(), void(&&)()>,
			void(*)()>(), "");
	static_assert(is_type<common_type<void(&&)(), void(&)()>,
			void(*)()>(), "");
	static_assert(is_type<common_type<void(&&)(), void(&&)()>,
			void(*)()>(), "");
	static_assert(is_type<common_type<ImplicitTo<int>, int>, int>{}, "");
	static_assert(is_type<common_type<ImplicitTo<int>, ImplicitTo<int>>,
			ImplicitTo<int>>{}, "");
	static_assert(is_type<common_type<ImplicitTo<int>, int,
			ImplicitTo<int>>, int>{}, "");
	static_assert(is_type<common_type<ExplicitTo<int>, ExplicitTo<int>>,
			ExplicitTo<int>>{}, "");
	static_assert(is_type<common_type<decltype(lmd1), decltype(lmd1)>,
			decltype(lmd1)>(), "");
	static_assert(is_type<common_type<decltype(lmd1)&, decltype(lmd1)&>,
			decltype(lmd1)>(), "");
	static_assert(is_type<common_type<decltype(lmd1)&, decltype(lmd2)&>,
			void(*)(int, double)>(), "");
	static_assert(is_type<common_type<decltype(nullptr), void*>, void*>(), "");
	static_assert(is_type<common_type<decltype(nullptr), int*>, int*>(), "");
	static_assert(is_type<common_type<const decltype(nullptr)&, int*>,
			int*>(), "");
	static_assert(is_type<common_type<decltype(nullptr), const volatile int*>,
			const volatile int*>(), "");
	static_assert(is_type<common_type<decltype(nullptr), int (B::*)()>,
			int (B::*)()>(), "");
	static_assert(is_type<common_type<decltype(nullptr), int (B::*)() const>,
			int (B::*)() const>(), "");
	static_assert(is_type<common_type<decltype(nullptr), const int B::*>,
			const int B::*>(), "");
	static_assert(is_type<common_type<Abstract&, Abstract&>, Abstract>(), "");
	static_assert(is_type<common_type<Ukn&, Ukn&>, Ukn>(), "");
	static_assert(is_type<common_type<ImplicitTo<B&>, B&>, B>{}, "");
	static_assert(is_type<common_type<ImplicitTo<B&>&, B&&>, B>{}, "");
	static_assert(is_type<common_type<UConv1, const Abstract*&>,
			const Abstract*>(), "");
	static_assert(is_type<common_type<UConv1, UConv2>, Abstract*>{}, "");
	static_assert(is_type<common_type<UConv1&, UConv2&>, Abstract*>{}, "");

	static_assert(is_type<common_type<Abstract&&, Abstract&&>,
			Abstract>(), "");
	static_assert(is_type<common_type<const Abstract&&,
						const Abstract&&>, Abstract>(), "");
	static_assert(is_type<common_type<volatile Abstract&&,
						volatile Abstract&&>, Abstract>(), "");
	static_assert(is_type<common_type<Ukn&&, Ukn&&>, Ukn>(), "");
	static_assert(is_type<common_type<const Ukn&&, const Ukn&&>,
			Ukn>(), "");
	static_assert(is_type<common_type<volatile Ukn&&, volatile Ukn&&>,
			Ukn>(), "");

#if 0 // IFNDR test cases
	static_assert(is_type<common_type<X1, X2>, RX12>(), "");
	static_assert(is_type<common_type<X2, X1>, RX21>(), "");

	static_assert(is_type<common_type<X1, X2, X1>, Y1>(), "");
	static_assert(is_type<common_type<X2, X1, X1>, Y3>(), "");

	static_assert(is_type<common_type<X1, X1, X2>, RX12>(), "");
	static_assert(is_type<common_type<X1, X1, X2, X1>, Y1>(), "");
#endif // IFNDR test cases

	static_assert(!meta::is_trait<common_type<>>(), "");
	static_assert(!meta::is_trait<common_type<int, S>>(), "");
	static_assert(!meta::is_trait<common_type<U, S>>(), "");
	static_assert(!meta::is_trait<common_type<U, U2>>(), "");
	// See https://wg21.link/lwg2763 and P0435
	static_assert(is_same_v<common_type_t<const ImplicitTo<int>, int>, int>);
#if !defined(__GNUC__) || defined(__clang__) // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67225
	static_assert(!meta::is_trait<common_type<PrivateImplicitTo<int>, int>>{}, "");
	static_assert(!meta::is_trait<common_type<const PrivateImplicitTo<int>, int>>(), "");
#endif
	static_assert(!meta::is_trait<common_type<int, Ukn>>(), "");
	static_assert(!meta::is_trait<common_type<int, Abstract>>(), "");
	static_assert(!meta::is_trait<common_type<Ukn, Abstract>>(), "");
	static_assert(!meta::is_trait<common_type<int, void>>(), "");
	static_assert(!meta::is_trait<common_type<int, const volatile void>>(), "");
	static_assert(!meta::is_trait<common_type<Abstract, void>>(), "");
	static_assert(!meta::is_trait<common_type<Ukn, void>>(), "");
	static_assert(!meta::is_trait<common_type<int[4], void>>(), "");
	static_assert(!meta::is_trait<common_type<ScEn, void>>(), "");
	static_assert(!meta::is_trait<common_type<UnscEn, void>>(), "");
	static_assert(!meta::is_trait<common_type<U, void>>(), "");
	static_assert(!meta::is_trait<common_type<std::initializer_list<int>,
			void>>(), "");
	static_assert(!meta::is_trait<common_type<int, int, int, S>>(), "");
	static_assert(!meta::is_trait<common_type<int, int, S, int>>(), "");
	static_assert(!meta::is_trait<common_type<int, S, int, int>>(), "");
	static_assert(!meta::is_trait<common_type<S, int, int, int>>(), "");
	static_assert(!meta::is_trait<common_type<int, int, void, int, int>>(), "");
	static_assert(!meta::is_trait<common_type<B, S>>(), "");
	static_assert(!meta::is_trait<common_type<int, B, S>>(), "");
	static_assert(!meta::is_trait<common_type<B, int, S>>(), "");
	static_assert(!meta::is_trait<common_type<B, S, int>>(), "");
	static_assert(!meta::is_trait<common_type<int*, double*>>(), "");
	static_assert(!meta::is_trait<common_type<void*, void(*)(...)>>(), "");
	static_assert(!meta::is_trait<common_type<void(*)(), void(*)(...)>>(), "");
	static_assert(!meta::is_trait<common_type<void(*)(), void(S::*)()>>(), "");
	static_assert(!meta::is_trait<common_type<void(S::*)() const,
			void(S::*)()>>(), "");
	static_assert(!meta::is_trait<common_type<int S::*, long S::*>>(), "");
	static_assert(!meta::is_trait<common_type<int S::*, void(S::*)()>>(), "");
	static_assert(!meta::is_trait<common_type<int (B::*)(),
			int (D::*)() const>>(), "");
	static_assert(!meta::is_trait<common_type<int (B::*)() const,
			int (D::*)()>>(), "");
	static_assert(!meta::is_trait<common_type<int, ExplicitTo<int>>>{}, "");
	static_assert(!meta::is_trait<common_type<ImplicitTo<int>,
						ExplicitTo<int>>>{}, "");
	static_assert(!meta::is_trait<common_type<ScEn, int>>(), "");
	static_assert(!meta::is_trait<common_type<ScEn, UnscEn>>(), "");
	static_assert(!meta::is_trait<common_type<U, S, Abstract, void, D,
			int (B::*)(), int[5]>>(), "");
	static_assert(!meta::is_trait<common_type<UConv1, Abstract&&>>(), "");
	static_assert(!meta::is_trait<common_type<std::initializer_list<int>,
						std::initializer_list<long>>>(), "");

	void test(int i)
	{
		auto local_lmd1 = [=](int, double) { return i + i; };
		auto local_lmd2 = [=](int, double) { return i - i; };

		static_assert(is_type<common_type<decltype(local_lmd1),
						decltype(local_lmd1)>, decltype(local_lmd1)>(), "");
		static_assert(is_type<common_type<decltype(local_lmd1)&,
						decltype(local_lmd1)>, decltype(local_lmd1)>(), "");
		static_assert(is_type<common_type<decltype(local_lmd1)&,
					decltype(local_lmd1)&>, decltype(local_lmd1)>(), "");

		static_assert(!meta::is_trait<common_type<decltype(local_lmd1),
				decltype(lmd1)>>(), "");
		static_assert(!meta::is_trait<common_type<decltype(local_lmd1)&,
				decltype(lmd1)&>>(), "");
		static_assert(!meta::is_trait<common_type<decltype(local_lmd1),
				decltype(local_lmd2)>>(), "");
		static_assert(!meta::is_trait<common_type<decltype(local_lmd1)&,
				decltype(local_lmd2)&>>(), "");
	}

	template<typename... Args>
	constexpr
	std::array<typename common_type<Args...>::type,
		sizeof...(Args)>
	make_array(Args&&... args)
	{
		typedef typename common_type<Args...>::type CT;
		return std::array<CT, sizeof...(Args)>{static_cast<CT>
			(std::forward<Args>(args))...};
	}

	void test01()
	{
		constexpr auto a1 = make_array(0);
		constexpr auto a2 = make_array(0, 1.2);
		constexpr auto a3 = make_array(5, true, 3.1415f, 'c');

		int i{};
		double d{1.2};
		float f{3.1415f};

		auto b1 = make_array(i);
		auto b2 = make_array(i, 1.2);
		auto b3 = make_array(i, d);
		auto b4 = make_array(0, d);
		auto b5 = make_array(i, true, f, 'c');

		static_assert(is_same<decltype(a1), const std::array<int, 1>>(), "");
		static_assert(is_same<decltype(a2), const std::array<double, 2>>(), "");
		static_assert(is_same<decltype(a3), const std::array<float, 4>>(), "");

		static_assert(is_same<decltype(b1), std::array<int, 1>>(), "");
		static_assert(is_same<decltype(b2), std::array<double, 2>>(), "");
		static_assert(is_same<decltype(b3), std::array<double, 2>>(), "");
		static_assert(is_same<decltype(b4), std::array<double, 2>>(), "");
		static_assert(is_same<decltype(b5), std::array<float, 4>>(), "");
	}

	#define JOIN( X, Y ) DO_JOIN( X, Y )
	#define DO_JOIN( X, Y ) DO_JOIN2(X,Y)
	#define DO_JOIN2( X, Y ) X##Y

	#define COMMON_TYPE_TEST_1(type1, uid) \
		typedef common_type<type1>::type JOIN(test_t,uid); \
		CHECK( (is_same<JOIN(test_t,uid), JOIN(test_t,uid)>::value) ); \
		typedef common_type<const type1>::type JOIN(test_t,JOIN(uid,c)); \
		CHECK( (is_same<JOIN(test_t,JOIN(uid,c)), \
						JOIN(test_t,JOIN(uid,c))>::value) ); \
		typedef common_type<volatile type1>::type JOIN(test_t,JOIN(uid,v)); \
		CHECK( (is_same<JOIN(test_t,JOIN(uid,v)), \
						JOIN(test_t,JOIN(uid,v))>::value) ); \
		typedef common_type<const volatile type1>::type JOIN(test_t,JOIN(uid,cv)); \
		CHECK( (is_same<JOIN(test_t,JOIN(uid,cv)), \
						JOIN(test_t,JOIN(uid,cv))>::value) ); \
		typedef common_type<type1 &>::type JOIN(test_t,JOIN(uid,l)); \
		CHECK( (is_same<JOIN(test_t,JOIN(uid,l)), \
						JOIN(test_t,JOIN(uid,l))>::value) ); \
		typedef common_type<const type1 &>::type JOIN(test_t,JOIN(uid,lc)); \
		CHECK( (is_same<JOIN(test_t,JOIN(uid,lc)), \
						JOIN(test_t,JOIN(uid,lc))>::value) ); \
		typedef common_type<volatile type1 &>::type JOIN(test_t,JOIN(uid,lv)); \
		CHECK( (is_same<JOIN(test_t,JOIN(uid,lv)), \
						JOIN(test_t,JOIN(uid,lv))>::value) ); \
		typedef common_type<const volatile type1 &>::type JOIN(test_t,JOIN(uid,lcv)); \
		CHECK( (is_same<JOIN(test_t,JOIN(uid,lcv)), \
						JOIN(test_t,JOIN(uid,lcv))>::value) ); \
		typedef common_type<type1 &&>::type JOIN(test_t,JOIN(uid,r)); \
		CHECK( (is_same<JOIN(test_t,JOIN(uid,r)), \
						JOIN(test_t,JOIN(uid,r))>::value) ); \
		typedef common_type<const type1 &&>::type JOIN(test_t,JOIN(uid,rc)); \
		CHECK( (is_same<JOIN(test_t,JOIN(uid,rc)), \
						JOIN(test_t,JOIN(uid,rc))>::value) ); \
		typedef common_type<volatile type1 &&>::type JOIN(test_t,JOIN(uid,rv)); \
		CHECK( (is_same<JOIN(test_t,JOIN(uid,rv)), \
						JOIN(test_t,JOIN(uid,rv))>::value) ); \
		typedef common_type<const volatile type1 &&>::type JOIN(test_t,JOIN(uid,rcv)); \
		CHECK( (is_same<JOIN(test_t,JOIN(uid,rcv)), \
						JOIN(test_t,JOIN(uid,rcv))>::value) )

	struct AA { };
	struct BB : AA { };

	void typedefs_test01()
	{
		// Positive tests.
		COMMON_TYPE_TEST_1(int, 1);
		COMMON_TYPE_TEST_1(double, 2);
		COMMON_TYPE_TEST_1(AA, 3);
		COMMON_TYPE_TEST_1(BB, 4);
	}

	#define COMMON_TYPE_TEST_2_IMPL(type1, type2, type3, uid) \
		typedef common_type<type1, type2>::type JOIN(JOIN(test, uid),_t1); \
		typedef common_type<type2, type1>::type JOIN(JOIN(test, uid),_t2); \
		CHECK( (is_same<JOIN(JOIN(test, uid),_t1), type3>::value) ); \
		CHECK( (is_same<JOIN(JOIN(test, uid),_t2), type3>::value) )

	#define NO_CV

	#define COMMON_TYPE_TEST_2(cv_qual, type1, type2, type3, uid) \
		COMMON_TYPE_TEST_2_IMPL(cv_qual type1, type2, type3, uid); \
		COMMON_TYPE_TEST_2_IMPL(cv_qual type1 &, type2, type3, JOIN(uid,l)); \
		COMMON_TYPE_TEST_2_IMPL(cv_qual type1 &&, type2, type3, JOIN(uid,r))

	#define COMMON_TYPE_TEST_ALL_2(type1, type2, type3, uid) \
		COMMON_TYPE_TEST_2(NO_CV, type1, type2, type3, uid); \
		COMMON_TYPE_TEST_2(const, type1, type2, type3, uid); \
		COMMON_TYPE_TEST_2(volatile, type1, type2, type3, uid); \
		COMMON_TYPE_TEST_2(const volatile, type1, type2, type3, uid)

	void typedefs_test02()
	{
		COMMON_TYPE_TEST_ALL_2(int, int, int, 1);
		COMMON_TYPE_TEST_ALL_2(int, double, double, 2);
		COMMON_TYPE_TEST_2(NO_CV, AA, AA, AA, 3);
		COMMON_TYPE_TEST_2(const, AA, AA, AA, 4);
		COMMON_TYPE_TEST_2(NO_CV, BB, AA, AA, 5);
	}

	void typedefs_1()
	{
		typedefs_test01();
		typedefs_test02();
	}

	// 2009-11-12  Paolo Carlini  <paolo.carlini@oracle.com>
	// DR 1255.
	static_assert( is_same<common_type<void>::type, void>(),
					"common_type<void>" );
	static_assert( is_same<common_type<const void>::type, void>(),
					"common_type<const void>" );
	static_assert( is_same<common_type<volatile void>::type, void>(),
					"common_type<volatile void>" );
	static_assert( is_same<common_type<const volatile void>::type, void>(),
					"common_type<const volatile void>" );

	static_assert( is_same<common_type<void, void>::type, void>(),
					"common_type<void, void>" );
	static_assert( is_same<common_type<void, const void>::type, void>(),
					"common_type<void, const void>" );
	static_assert( is_same<common_type<void, volatile void>::type, void>(),
					"common_type<void, volatile void>" );
	static_assert( is_same<common_type<void, const volatile void>::type, void>(),
					"common_type<void, const volatile void>" );
	static_assert( is_same<common_type<const void, void>::type, void>(),
					"common_type<const void, void>" );
	static_assert( is_same<common_type<const void, const void>::type, void>(),
					"common_type<const void, const void>" );
	static_assert( is_same<common_type<const void, volatile void>::type, void>(),
					"common_type<const void, volatile void>" );
	static_assert( is_same<common_type<const void, const volatile void>::type,
				void>(), "common_type<const void, const volatile void>" );
	static_assert( is_same<common_type<volatile void, void>::type, void>(),
					"common_type<volatile void, void>" );
	static_assert( is_same<common_type<volatile void, volatile void>::type,
				void>(), "common_type<volatile void, volatile void>" );
	static_assert( is_same<common_type<volatile void, const void>::type,
				void>(), "common_type<volatile void, const void>" );
	static_assert( is_same<common_type<volatile void, const volatile void>::type,
				void>(), "common_type<volatile void, const volatile void>" );
	static_assert( is_same<common_type<const volatile void, void>::type, void>(),
				"common_type<const volatile void, const volatile void>" );
	static_assert( is_same<common_type<const volatile void, const void>::type,
				void>(), "common_type<const volatile void, const void>" );
	static_assert( is_same<common_type<const volatile void, volatile void>::type,
				void>(), "common_type<const volatile void, volatile void>" );
	static_assert( is_same<common_type<const volatile void, const volatile void>::type,
				void>(),
				"common_type<const volatile void, const volatile void>" );
}

int main() {
	::libstdcpp_tests::typedefs_1();
	return ::test_result();
}
