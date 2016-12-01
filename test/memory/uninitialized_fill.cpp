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
#include <stl2/detail/memory/uninitialized_fill.hpp>
#include <cstdint>
#include <stl2/concepts.hpp>
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <vector>
#include "../simple_test.hpp"
#include "Book.hpp"
#include "raw_buffer.hpp"

namespace ranges = __stl2;

namespace {
	constexpr auto test_size{1 << 10};

	template <typename T>
	requires ranges::CopyConstructible<T>() && ranges::EqualityComparable<T>()
	void uninitialized_fill_test(const T& x)
	{
		const auto independent = make_buffer<T>(test_size);
		auto test = [&independent, &x](const auto& p){
			CHECK(p == independent.end());
			CHECK(ranges::find_if(independent.begin(), p, [&x](const T& i){ return i != x; }) == p);
			ranges::destroy(independent.begin(), p);
		};

		test(ranges::uninitialized_fill(independent.begin(), independent.end(), x));
		test(ranges::uninitialized_fill(independent.cbegin(), independent.cend(), x));
		test(ranges::uninitialized_fill(independent, x));
		test(ranges::uninitialized_fill_n(independent.begin(), independent.size(), x));
		test(ranges::uninitialized_fill_n(independent.cbegin(), independent.size(), x));
	}
}

int main()
{
	uninitialized_fill_test(0);
	uninitialized_fill_test(0.0);
	uninitialized_fill_test('a');
	uninitialized_fill_test(std::vector<int>{});
	uninitialized_fill_test(std::vector<int>(1 << 10, 0));
	uninitialized_fill_test(Book{});

	return ::test_result();
}
