// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/memory/uninitialized_value_construct.hpp>
#include <cstdint>
#include <deque>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <stl2/concepts.hpp>
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include "../simple_test.hpp"
#include "raw_buffer.hpp"

namespace ranges = __stl2;

namespace {
	constexpr auto N = 1 << 10;

	template <typename T>
	requires
		ranges::DefaultConstructible<T>() &&
		ranges::EqualityComparable<T>()
	void uninitialized_value_construct_test()
	{
		auto independent = make_buffer<T>(N);
		auto test = [&independent](const auto& p) {
			auto t = T{};
			CHECK(p == independent.end());
			CHECK(ranges::find_if(independent.begin(), p, [&t](const T& i){ return i != t; }) == p);
			ranges::destroy(independent.begin(), p);
		};

		test(ranges::uninitialized_value_construct(independent.begin(), independent.end()));
		test(ranges::uninitialized_value_construct(independent.cbegin(), independent.cend()));
		test(ranges::uninitialized_value_construct(independent));
		test(ranges::uninitialized_value_construct_n(independent.begin(), independent.size()));
		test(ranges::uninitialized_value_construct_n(independent.cbegin(), independent.size()));
	}
}

int main()
{
	using namespace std;

	uninitialized_value_construct_test<char>();
	uninitialized_value_construct_test<short>();
	uninitialized_value_construct_test<int>();
	uninitialized_value_construct_test<float>();
	uninitialized_value_construct_test<long>();
	uninitialized_value_construct_test<double>();
	uninitialized_value_construct_test<long long>();
	uninitialized_value_construct_test<vector<char>>();
	uninitialized_value_construct_test<string>();
	uninitialized_value_construct_test<deque<double>>();
	uninitialized_value_construct_test<list<vector<deque<double>>>>();
	uninitialized_value_construct_test<unique_ptr<string>>();

	return ::test_result();
}
