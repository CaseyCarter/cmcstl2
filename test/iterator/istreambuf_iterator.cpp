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
#include <stl2/detail/iterator/istreambuf_iterator.hpp>
#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <sstream>
#include "../simple_test.hpp"

using namespace __stl2;

namespace {
	template<class charT, class traits = std::char_traits<charT>>
	void validate_one() {
		using C = __istreambuf_iterator::cursor<charT, traits>;
		static_assert(cursor::Cursor<C>);
		static_assert(same_as<typename traits::off_type, cursor::difference_type_t<C>>);
		static_assert(cursor::Next<C>);
		static_assert(same_as<charT, cursor::value_type_t<C>>);
		static_assert(cursor::readable<C>);
		static_assert(same_as<charT, cursor::reference_t<C>>);
		static_assert(cursor::Input<C>);
		static_assert(cursor::sentinel_for<C, C>);
		static_assert(cursor::sentinel_for<default_sentinel, C>);
		static_assert(!cursor::Forward<C>);
		static_assert(cursor::PostIncrement<C>);

		using I = istreambuf_iterator<charT, traits>;
		static_assert(weakly_incrementable<I>);
		static_assert(same_as<typename traits::off_type, iter_difference_t<I>>);
		static_assert(same_as<charT, iter_value_t<I>>);
		static_assert(readable<I>);
		static_assert(same_as<charT, iter_reference_t<I>>);
		static_assert(same_as<charT, iter_rvalue_reference_t<I>>);
		static_assert(input_or_output_iterator<I>);
		static_assert(same_as<input_iterator_tag, iterator_category_t<I>>);
		static_assert(input_iterator<I>);
		static_assert(!forward_iterator<I>);
		static_assert(sentinel_for<I, I>);
		static_assert(sentinel_for<default_sentinel, I>);
		static_assert(common_with<I, default_sentinel>);
		static_assert(same_as<I, common_type_t<I, default_sentinel>>);

		static_assert(same_as<iter_value_t<I>, typename I::value_type>);
		static_assert(same_as<iter_difference_t<I>, typename I::difference_type>);
		static_assert(same_as<input_iterator_tag, typename I::iterator_category>);
		static_assert(same_as<charT, typename I::reference>);
		static_assert(same_as<traits, typename I::traits_type>);
		static_assert(same_as<typename traits::int_type, typename I::int_type>);
		static_assert(same_as<std::basic_streambuf<charT, traits>, typename I::streambuf_type>);
		static_assert(same_as<std::basic_istream<charT, traits>, typename I::istream_type>);

		auto i = I{};
		auto ci = const_cast<const I&>(i);
		CHECK(ci.equal(i));
		CHECK(ci == i);
		CHECK(!(ci != i));
		CHECK(ci == default_sentinel{});
		CHECK(i == default_sentinel{});
		CHECK(!(ci != default_sentinel{}));
		CHECK(!(i != default_sentinel{}));

		static_assert(same_as<decltype(i.operator->()), typename C::pointer>);
#if 1 // FIXME: remove
		static_assert(!__stl2::detail::PostIncrementCursor<C>);
		static_assert(same_as<decltype(std::declval<C&>().post_increment()),
			typename C::__proxy>);
		static_assert(!same_as<decltype(i++), I>);
#endif
		static_assert(same_as<decltype(i++), typename C::__proxy>);

		static_assert(constructible_from<I, default_sentinel>);
		static_assert(convertible_to<default_sentinel, I>);
		static_assert(constructible_from<I, std::basic_istream<charT, traits>&>);
		static_assert(constructible_from<I, std::basic_streambuf<charT, traits>*>);
		static_assert(constructible_from<I, decltype(i++)>);
	}

	template<class... Cs>
	void validate() {
		(validate_one<Cs>(), ...);
	}
}

int main() {
	validate<char, wchar_t, char16_t, char32_t>();

	using I = istreambuf_iterator<char>;
	{
		static const char hw[] = "Hello, world!";
		std::istringstream is(hw);
		CHECK_EQUAL(subrange(I{is}, default_sentinel{}),
									subrange(hw + 0, hw + size(hw) - 1));
		CHECK(I{is} == default_sentinel{});
	}

	{
		// Test the postincrement proxy type.
		std::istringstream is("123");
		auto i = I{is};
		CHECK(*i++ == '1');
		auto j = i++;
		CHECK(*j == '2');
		auto k = I{j};
		CHECK(*k++ == '3');
		CHECK(k == I{});
		CHECK(k == default_sentinel{});
	}

	{
		// Test the operator-> proxy type.
		std::istringstream is("123");
		auto i = I{is};
		CHECK(*i.operator->().operator->() == '1');
		++i;
		CHECK(*i.operator->().operator->() == '2');
	}

	{
		// regression test for #149
		auto in = std::istringstream{};
		using I = std::istreambuf_iterator<char>;
		CHECK(in.rdbuf() != nullptr);
		CHECK(I{in} == I{});
	}

	return ::test_result();
}
