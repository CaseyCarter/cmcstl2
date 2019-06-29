// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/numeric/adjacent_difference.hpp>

#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include <array>
#include <cassert>
#include <complex>
#include <forward_list>
#include <stl2/functional.hpp>
#include <stl2/view/iota.hpp>
#include <stl2/view/reverse.hpp>
#include <stl2/view/transform.hpp>
#include <stl2/view/subrange.hpp>
#include <vector>

namespace ranges {
	using namespace __stl2;
	using namespace __stl2::ext;
} // namespace ranges

namespace view = ranges::view;

using ranges::adjacent_difference;

template<class I, class S1, class O, class S2, class E>
void test_iterator(I first, S1 last, O result, S2 result_last, E const& expected_output) {
	auto output = ranges::subrange{result, result_last};
	{
		auto actual_result = adjacent_difference(first, last, result, result_last);
		CHECK(actual_result.in == last);
		CHECK(actual_result.out == result_last);
		CHECK_EQUAL(output, expected_output);
	}
	{
		auto actual_result = adjacent_difference(ranges::subrange{first, last}, output);
		CHECK(actual_result.in == last);
		CHECK(actual_result.out == result_last);
		CHECK_EQUAL(output, expected_output);
	}
}

template<ranges::Range R, ranges::Range O, ranges::Range E>
void test_iterator(R&& r, O&& result, E const& expected_output) {
	test_iterator(input_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), expected_output);
	test_iterator(forward_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), expected_output);
	test_iterator(bidirectional_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), expected_output);
	test_iterator(random_access_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), expected_output);

	test_iterator(forward_iterator(begin(r)), forward_iterator(end(r)),
		begin(result), end(result), expected_output);
	test_iterator(bidirectional_iterator(begin(r)), bidirectional_iterator(end(r)),
		begin(result), end(result), expected_output);
	test_iterator(random_access_iterator(begin(r)), random_access_iterator(end(r)),
		begin(result), end(result), expected_output);
}

template<class I, class S1, class O, class S2, class BOp, class E>
void test_binary_op(I first, S1 last, O result, S2 result_last, BOp const& bop,
	E const& expected_output) {
	auto output = ranges::subrange{result, result_last};
	{
		auto actual_result = adjacent_difference(first, last, result, result_last, bop);
		CHECK(actual_result.in == last);
		CHECK(actual_result.out == result_last);
		CHECK_EQUAL(output, expected_output);
	}
	{
		auto actual_result = adjacent_difference(ranges::subrange{first, last}, output, bop);
		CHECK(actual_result.in == last);
		CHECK(actual_result.out == result_last);
		CHECK_EQUAL(output, expected_output);
	}
}

template<ranges::Range R, ranges::Range O, class BOp, ranges::Range E>
void test_binary_op(R&& r, O&& result, BOp const& bop, E const& expected_output) {
	test_binary_op(input_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, expected_output);
	test_binary_op(forward_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, expected_output);
	test_binary_op(bidirectional_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, expected_output);
	test_binary_op(random_access_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, expected_output);

	test_binary_op(forward_iterator(begin(r)), forward_iterator(end(r)),
		begin(result), end(result), bop, expected_output);
	test_binary_op(bidirectional_iterator(begin(r)), bidirectional_iterator(end(r)),
		begin(result), end(result), bop, expected_output);
	test_binary_op(random_access_iterator(begin(r)), random_access_iterator(end(r)),
		begin(result), end(result), bop, expected_output);
}

template<class I, class S1, class O, class S2, class BOp, class Proj, class E>
void test_projection(I first, S1 last, O result, S2 result_last, BOp const& bop, Proj const& proj,
	E const& expected_output) {
	auto output = ranges::subrange{result, result_last};
	{
		auto actual_result = adjacent_difference(first, last, result, result_last, bop, proj);
		CHECK(actual_result.in == last);
		CHECK(actual_result.out == result_last);
		CHECK_EQUAL(output, expected_output);
	}
	{
		auto actual_result = adjacent_difference(ranges::subrange{first, last}, output, bop, proj);
		CHECK(actual_result.in == last);
		CHECK(actual_result.out == result_last);
		CHECK_EQUAL(output, expected_output);
	}
}

template<ranges::Range R, ranges::Range O, class BOp, class Proj, ranges::Range E>
void test_projection(R&& r, O&& result, BOp const& bop, Proj const& proj, E const& expected_output)
{
	test_projection(input_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, proj, expected_output);
	test_projection(forward_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, proj, expected_output);
	test_projection(bidirectional_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, proj, expected_output);
	test_projection(random_access_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, proj, expected_output);

	test_projection(forward_iterator(begin(r)), forward_iterator(end(r)),
		begin(result), end(result), bop, proj, expected_output);
	test_projection(bidirectional_iterator(begin(r)), bidirectional_iterator(end(r)),
		begin(result), end(result), bop, proj, expected_output);
	test_projection(random_access_iterator(begin(r)), random_access_iterator(end(r)),
		begin(result), end(result), bop, proj, expected_output);
}

int main()
{
	{
		auto ints = view::iota(1, 100);
		auto result = std::vector(ranges::distance(ints), 0);
		auto const expected_output = std::vector(ranges::distance(ints), 1);

		test_iterator(ints, result, expected_output);
		test_binary_op(ints, result, ranges::minus{}, expected_output);
		test_projection(ints, result, ranges::minus{}, ranges::identity{}, expected_output);
	}
	{
		auto ints  = view::iota(0, 10);
		auto result = std::vector(ranges::distance(ints), 0);
		auto const expected_output = std::array{0, 1, 3, 5, 7, 9, 11, 13, 15, 17};

		test_binary_op(ints, result, ranges::plus{}, expected_output);
		test_projection(ints, result, ranges::plus{}, ranges::identity{}, expected_output);
	}
	{
		using namespace std::literals;
		auto input = std::array{
			0.0 + 1.0i,
			1.0 + 2.0i,
			2.0 + 3.0i,
			3.0 + 4.0i,
			4.0 + 5.0i,
			5.0 + 6.0i,
			6.0 + 7.0i,
			7.0 + 8.0i,
			8.0 + 9.0i,
			9.0 + 0.0i
		};

		{
			auto imaginary_result = std::forward_list<std::complex<double>>(input.size());
			constexpr auto expected_imaginary_output = std::array{
				1.0i, 3.0i, 5.0i, 7.0i, 9.0i, 11.0i, 13.0i, 15.0i, 17.0i, 9.0i
			};
			test_projection(input, imaginary_result, ranges::plus{}, [](auto const x) {
				return std::imag(x) * 1.0i; }, expected_imaginary_output);
		}
		{
			auto output = std::ostringstream{};
			ranges::adjacent_difference(ranges::begin(input), ranges::end(input),
				ranges::ostream_iterator<double>{output, ", "}, ranges::unreachable{},
				ranges::product{}, [](auto const x) { return std::real(x); });

			constexpr auto expected_real_output = "0, 0, 2, 6, 12, 20, 30, 42, 56, 72, "sv;
			CHECK_EQUAL(output.str(), expected_real_output);
		}
	}
	return ::test_result();
}
