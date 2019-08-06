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
#include <stl2/detail/iterator/ostream_iterator.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <sstream>
#include "../simple_test.hpp"

using namespace __stl2;

namespace {
	template<class I, class O>
	struct result {
		I in;
		O out;
	};

	template<InputIterator I, Sentinel<I> S, OutputIterator<iter_reference_t<I>> O>
	constexpr result<I, O> copy(I first, S last, O out) {
		for (; first != last; ++first, void(), ++out) {
			*out = *first;
		}
		return {first, out};
	}
}

int main() {
	std::stringstream ss;
	static constexpr int some_ints[] = {0, 7, 1, 6, 2, 5, 3, 4};

	using I = ostream_iterator<int>;
	static_assert(WeaklyIncrementable<I>);
	static_assert(same_as<iter_difference_t<I>, std::ptrdiff_t>);
	static_assert(Iterator<I>);
	static_assert(same_as<iter_reference_t<I>, I&>);
	static_assert(OutputIterator<I, const int&>);
	static_assert(!InputIterator<I>);

	I i{ss, " "};
	static_assert(same_as<I::difference_type, std::ptrdiff_t>);
	static_assert(same_as<I::char_type, char>);
	static_assert(same_as<I::traits_type, std::char_traits<char>>);
	static_assert(same_as<I::ostream_type, std::ostream>);

	static_assert(same_as<I&, decltype(*i)>);
	static_assert(same_as<I&, decltype(*i = 42)>);
	static_assert(same_as<I&, decltype(++i)>);
	static_assert(same_as<I&, decltype(i++)>);

	static_assert(noexcept(I{}));
#if defined(__GNUC__) && __GNUC__ < 8 || (__GNUC__ == 7 && __GNUC_MINOR__ >= 2)
	// workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80633
	static_assert(noexcept(I{std::declval<decltype((ss))>()}));
	static_assert(noexcept(I{std::declval<decltype((ss))>(), "some text"}));
#else
	static_assert(noexcept(I{ss}));
	static_assert(noexcept(I{ss, "some text"}));
#endif
	static_assert(noexcept(I{I{}}));
	static_assert(noexcept(I{i}));

	::copy(begin(some_ints), end(some_ints), i);
	CHECK(ss.str() == "0 7 1 6 2 5 3 4 ");
	CHECK_EQUAL(
		subrange(istream_iterator<int>{ss}, default_sentinel{}),
			some_ints);

	static_assert(same_as<
		ostream_iterator<int>&,
		decltype(*std::declval<ostream_iterator<int>&>())>);

	ostream_iterator<std::string>{std::cout} = "Hello, World!\n";

	return ::test_result();
}
