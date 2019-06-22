// Range v3 library
//
//  Copyright Casey Carter
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
#include <stl2/detail/numeric/accumulate.hpp>

#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include <array>
#include <stl2/detail/functional/arithmetic/product.hpp>
#include <stl2/detail/functional/arithmetic/quotient.hpp>
#include <stl2/view/iota.hpp>
#include <stl2/view/subrange.hpp>
#include <string>
#include <string_view>

namespace ranges {
	using namespace __stl2;
	using namespace __stl2::ext;
} // namespace ranges

namespace view = ranges::view;

template<class I, class S, class T, class Expected>
void test_iterator(I first, S last, T const& init, Expected const& expected_value)
{
	using ranges::ext::accumulate;
	{
		auto actual_result = accumulate(first, last, init);
		CHECK(actual_result.in == last);
		CHECK(actual_result.value == expected_value);
	}
	{
		auto actual_result = accumulate(ranges::subrange{first, last}, init);
		CHECK(actual_result.in == last);
		CHECK(actual_result.value == expected_value);
	}
}

template<class R, class T, class Expected>
void test_iterator(R&& r, T init, Expected const& expected_value)
{
	test_iterator(input_iterator(begin(r)), sentinel(end(r)), init, expected_value);
	test_iterator(forward_iterator(begin(r)), sentinel(end(r)), init, expected_value);
	test_iterator(random_access_iterator(begin(r)), sentinel(end(r)), init, expected_value);
	test_iterator(bidirectional_iterator(begin(r)), sentinel(end(r)), init, expected_value);

	test_iterator(forward_iterator(begin(r)), forward_iterator(end(r)), init, expected_value);
	test_iterator(bidirectional_iterator(begin(r)), bidirectional_iterator(end(r)), init,
		expected_value);
	test_iterator(random_access_iterator(begin(r)), random_access_iterator(end(r)), init,
		expected_value);
}

template<class I, class S, class T, class BOp, class Expected>
void test_binary_op(I first, S last, T const& init, BOp const& bop, Expected const& expected_value)
{
	using ranges::ext::accumulate;
	{
		auto actual_result = accumulate(first, last, init, bop);
		CHECK(actual_result.in == last);
		CHECK(actual_result.value == expected_value);
	}
	{
		auto actual_result = accumulate(ranges::subrange{first, last}, init, bop);
		CHECK(actual_result.in == last);
		CHECK(actual_result.value == expected_value);
	}
}

template<class R, class T, class BOp, class Expected>
void test_binary_op(R&& r, T const& init, BOp const& bop, Expected const& expected_value)
{
	test_binary_op(input_iterator(begin(r)), sentinel(end(r)), init, bop, expected_value);
	test_binary_op(forward_iterator(begin(r)), sentinel(end(r)), init, bop, expected_value);
	test_binary_op(random_access_iterator(begin(r)), sentinel(end(r)), init, bop, expected_value);
	test_binary_op(bidirectional_iterator(begin(r)), sentinel(end(r)), init, bop, expected_value);

	test_binary_op(forward_iterator(begin(r)), forward_iterator(end(r)), init, bop, expected_value);
	test_binary_op(bidirectional_iterator(begin(r)), bidirectional_iterator(end(r)), init, bop,
		expected_value);
	test_binary_op(random_access_iterator(begin(r)), random_access_iterator(end(r)), init, bop,
		expected_value);
}

template<class I, class S, class T, class BOp, class Proj, class Expected>
void test_projection(I first, S last, T const& init, BOp const& bop, Proj const& proj,
	Expected const& expected_value)
{
	using ranges::ext::accumulate;
	{
		auto actual_result = accumulate(first, last, init, bop, proj);
		CHECK(actual_result.in == last);
		CHECK(actual_result.value == expected_value);
	}
	{
		auto actual_result = accumulate(ranges::subrange{first, last}, init, bop, proj);
		CHECK(actual_result.in == last);
		CHECK(actual_result.value == expected_value);
	}
}

template<class R, class T, class BOp, class Proj, class Expected>
void test_projection(R&& r, T const& init, BOp const& bop, Proj const& proj,
	Expected const& expected_value)
{
	test_projection(input_iterator(begin(r)), sentinel(end(r)), init, bop, proj, expected_value);
	test_projection(forward_iterator(begin(r)), sentinel(end(r)), init, bop, proj, expected_value);
	test_projection(random_access_iterator(begin(r)), sentinel(end(r)), init, bop, proj,
		expected_value);
	test_projection(bidirectional_iterator(begin(r)), sentinel(end(r)), init, bop, proj,
		expected_value);

	test_projection(forward_iterator(begin(r)), forward_iterator(end(r)), init, bop, proj,
		expected_value);
	test_projection(bidirectional_iterator(begin(r)), bidirectional_iterator(end(r)), init, bop,
		proj, expected_value);
	test_projection(random_access_iterator(begin(r)), random_access_iterator(end(r)), init, bop,
		proj, expected_value);
}

int main()
{
	using ranges::accumulate;
	using ranges::begin, ranges::end;

	{
		auto ints = view::iota(0, 10);
		test_iterator(ints, 0, 45);
		test_iterator(ints, 0.0, 45);

		using namespace std::string_literals;
		using namespace std::string_view_literals;
		auto const lyrics = std::array{"once a jolly swagman, "s, "camped"s, " by a "s, "billabong"s};
		test_iterator(lyrics, ""s, "once a jolly swagman, camped by a billabong"sv);
	}
	{
		auto doubles = view::iota(1, 6);
		test_binary_op(doubles, 1, ranges::ext::product{}, 120);
		test_binary_op(doubles, 1.0, ranges::ext::product{}, 120);
	}
	{
		struct entry {
			int id;
			double time;
		};

		constexpr auto applicants = std::array{
			entry{0, 126.84L},
			entry{1, 3.96375L},
			entry{2, 2.0L},
			entry{3, 4.0L},
			entry{4, 0.5L},
			entry{5, 8.0L},
		};

		test_projection(applicants, 16088.3856L, ranges::ext::quotient{}, &entry::time, 1.0L);
	}

	return ::test_result();
}
