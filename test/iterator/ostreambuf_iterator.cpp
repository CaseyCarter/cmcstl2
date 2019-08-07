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
#include <stl2/detail/iterator/ostreambuf_iterator.hpp>
#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <sstream>
#include "../simple_test.hpp"

using namespace __stl2;

namespace {
	template<class I, class O>
	struct result {
		I in;
		O out;
	};

	template<input_iterator I, sentinel_for<I> S, output_iterator<iter_reference_t<I>> O>
	constexpr result<I, O> copy(I first, S last, O out) {
		for (; first != last; ++first, void(), ++out) {
			*out = *first;
		}
		return {first, out};
	}

	template<input_range R, output_iterator<iter_reference_t<iterator_t<R>>> O>
	constexpr result<safe_iterator_t<R>, O>
	copy(R&& range, O out) {
		return ::copy(begin(range), end(range), std::move(out));
	}
}

int main() {
	using I = ostreambuf_iterator<char>;
	static_assert(output_iterator<I, const char&>);
	static_assert(sentinel_for<default_sentinel, I>);
	static_assert(common_with<I, default_sentinel>);
	static_assert(std::is_same<I, common_type_t<I, default_sentinel>>());

	{
		static const char hw[] = "Hello, world!";
		auto hw_range = subrange(begin(hw), end(hw) - 1);
		std::ostringstream os;
		auto r = ::copy(hw_range, I{os});
		CHECK(r.out != default_sentinel{});
		CHECK_EQUAL(os.str(), hw_range);
	}

	return ::test_result();
}
