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
