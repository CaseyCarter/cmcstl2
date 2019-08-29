// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <set>
#include <unordered_set>
#include <vector>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include "../simple_test.hpp"

namespace ranges = std::experimental::ranges;

template<bool allow_nonconst, bool allow_const, bool allow_size>
struct arbitrary_range {
	int* begin() requires allow_nonconst;
	int* end() requires allow_nonconst;
	const int* begin() const requires allow_const;
	const int* end() const requires allow_const;

	unsigned char size() const requires allow_size;
};

template<class>
constexpr bool always_false = false;

using mutable_unsized_range = arbitrary_range<true, true, false>;
using mutable_only_no_size_range = arbitrary_range<true, false, false>;
using immutable_unsized_range = arbitrary_range<false, true, false>;
using mutable_sized_range = arbitrary_range<true, true, true>;
using mutable_only_sized_range = arbitrary_range<true, false, true>;
using immutable_sized_range = arbitrary_range<false, true, true>;

// size() launches the missiles.
template<class Base>
struct bad_sized_range : Base {
	[[noreturn]] int size() const {
		static_assert(always_false<Base>);
	}
	[[noreturn]] friend int size(const bad_sized_range&) {
		static_assert(always_false<Base>);
	}
};

using mutable_badsized_range = bad_sized_range<mutable_sized_range>;
using mutable_only_badsized_range = bad_sized_range<mutable_only_sized_range>;
using immutable_badsized_range = bad_sized_range<immutable_sized_range>;

namespace std::experimental::ranges {
	template<class Base>
	constexpr bool disable_sized_range<bad_sized_range<Base>> = true;
} // namespace std::experimental::ranges

struct strange_view
{
	std::vector<int>::iterator begin();
	std::vector<int>::const_iterator begin() const;

	std::vector<int>::iterator end();
	std::vector<int>::const_iterator end() const;
};

struct strange_view2 : strange_view, ranges::view_base {};
struct strange_view3 : strange_view2 {};

namespace std::experimental::ranges {
	template<>
	inline constexpr bool enable_view<strange_view> = true;
	template<>
	inline constexpr bool enable_view<strange_view3> = false;
} // namespace std::experimental::ranges

void ridiculously_exhaustive_range_property_test() {
	static_assert(!ranges::range<void>);
	static_assert(!ranges::sized_range<void>);
	static_assert(!ranges::_ContainerLike<void>);
	static_assert(!ranges::view<void>);

	using I = int*;
	using CI = const int*;

	static_assert(ranges::input_or_output_iterator<I>);
	static_assert(ranges::input_or_output_iterator<CI>);

	static_assert(ranges::same_as<ranges::iterator_t<int[2]>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<int[2]>, I>);
	static_assert(ranges::range<int[2]>);
	static_assert(ranges::sized_range<int[2]>);
	static_assert(ranges::_ContainerLike<int[2]>);
	static_assert(!ranges::view<int[2]>);

	static_assert(ranges::same_as<ranges::iterator_t<int(&)[2]>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<int(&)[2]>, I>);
	static_assert(ranges::range<int(&)[2]>);
	static_assert(ranges::sized_range<int(&)[2]>);
	static_assert(!ranges::_ContainerLike<int(&)[2]>);
	static_assert(!ranges::view<int(&)[2]>);

	static_assert(ranges::same_as<ranges::iterator_t<const int[2]>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const int[2]>, CI>);
	static_assert(ranges::range<const int[2]>);
	static_assert(ranges::sized_range<const int[2]>);
	static_assert(!ranges::_ContainerLike<const int[2]>);
	static_assert(!ranges::view<const int[2]>);

	static_assert(ranges::same_as<ranges::iterator_t<const int(&)[2]>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const int(&)[2]>, CI>);
	static_assert(ranges::range<const int(&)[2]>);
	static_assert(ranges::sized_range<const int(&)[2]>);
	static_assert(!ranges::_ContainerLike<const int(&)[2]>);
	static_assert(!ranges::view<const int(&)[2]>);

	static_assert(ranges::same_as<ranges::iterator_t<mutable_unsized_range>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<mutable_unsized_range>, I>);
	static_assert(ranges::range<mutable_unsized_range>);
	static_assert(ranges::sized_range<mutable_unsized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<mutable_unsized_range>())), std::ptrdiff_t>);
	static_assert(ranges::_ContainerLike<mutable_unsized_range>);
	static_assert(!ranges::view<mutable_unsized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<mutable_unsized_range&>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<mutable_unsized_range&>, I>);
	static_assert(ranges::range<mutable_unsized_range&>);
	static_assert(ranges::sized_range<mutable_unsized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<mutable_unsized_range&>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<mutable_unsized_range&>);
	static_assert(!ranges::view<mutable_unsized_range&>);

	static_assert(ranges::same_as<ranges::iterator_t<const mutable_unsized_range>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const mutable_unsized_range>, CI>);
	static_assert(ranges::range<const mutable_unsized_range>);
	static_assert(ranges::sized_range<const mutable_unsized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<const mutable_unsized_range>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<const mutable_unsized_range>);
	static_assert(!ranges::view<const mutable_unsized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<const mutable_unsized_range&>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const mutable_unsized_range&>, CI>);
	static_assert(ranges::range<const mutable_unsized_range&>);
	static_assert(ranges::sized_range<const mutable_unsized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<const mutable_unsized_range&>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<const mutable_unsized_range&>);
	static_assert(!ranges::view<const mutable_unsized_range&>);


	static_assert(ranges::same_as<ranges::iterator_t<mutable_only_no_size_range>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<mutable_only_no_size_range>, I>);
	static_assert(ranges::range<mutable_only_no_size_range>);
	static_assert(ranges::sized_range<mutable_only_no_size_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<mutable_only_no_size_range>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<mutable_only_no_size_range>);
	static_assert(ranges::view<mutable_only_no_size_range>);

	static_assert(ranges::same_as<ranges::iterator_t<mutable_only_no_size_range&>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<mutable_only_no_size_range&>, I>);
	static_assert(ranges::range<mutable_only_no_size_range&>);
	static_assert(ranges::sized_range<mutable_only_no_size_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<mutable_only_no_size_range&>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<mutable_only_no_size_range&>);
	static_assert(!ranges::view<mutable_only_no_size_range&>);

	static_assert(!ranges::range<const mutable_only_no_size_range>);
	static_assert(!ranges::sized_range<const mutable_only_no_size_range>);
	static_assert(!ranges::_ContainerLike<const mutable_only_no_size_range>);
	static_assert(!ranges::view<const mutable_only_no_size_range>);

	static_assert(!ranges::range<const mutable_only_no_size_range&>);
	static_assert(!ranges::sized_range<const mutable_only_no_size_range&>);
	static_assert(!ranges::_ContainerLike<const mutable_only_no_size_range&>);
	static_assert(!ranges::view<const mutable_only_no_size_range&>);


	static_assert(ranges::same_as<ranges::iterator_t<immutable_unsized_range>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<immutable_unsized_range>, CI>);
	static_assert(ranges::range<immutable_unsized_range>);
	static_assert(ranges::sized_range<immutable_unsized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<immutable_unsized_range>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<immutable_unsized_range>);
	static_assert(ranges::view<immutable_unsized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<immutable_unsized_range&>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<immutable_unsized_range&>, CI>);
	static_assert(ranges::range<immutable_unsized_range&>);
	static_assert(ranges::sized_range<immutable_unsized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<immutable_unsized_range&>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<immutable_unsized_range&>);
	static_assert(!ranges::view<immutable_unsized_range&>);

	static_assert(ranges::same_as<ranges::iterator_t<const immutable_unsized_range>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const immutable_unsized_range>, CI>);
	static_assert(ranges::range<const immutable_unsized_range>);
	static_assert(ranges::sized_range<const immutable_unsized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<const immutable_unsized_range>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<const immutable_unsized_range>);
	static_assert(!ranges::view<const immutable_unsized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<const immutable_unsized_range&>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const immutable_unsized_range&>, CI>);
	static_assert(ranges::range<const immutable_unsized_range&>);
	static_assert(ranges::sized_range<const immutable_unsized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<const immutable_unsized_range&>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<const immutable_unsized_range&>);
	static_assert(!ranges::view<const immutable_unsized_range&>);


	static_assert(ranges::same_as<ranges::iterator_t<mutable_sized_range>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<mutable_sized_range>, I>);
	static_assert(ranges::range<mutable_sized_range>);
	static_assert(ranges::sized_range<mutable_sized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<mutable_sized_range>())), unsigned char>);
	static_assert(ranges::_ContainerLike<mutable_sized_range>);
	static_assert(!ranges::view<mutable_sized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<mutable_sized_range&>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<mutable_sized_range&>, I>);
	static_assert(ranges::range<mutable_sized_range&>);
	static_assert(ranges::sized_range<mutable_sized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<mutable_sized_range&>())), unsigned char>);
	static_assert(!ranges::_ContainerLike<mutable_sized_range&>);
	static_assert(!ranges::view<mutable_sized_range&>);

	static_assert(ranges::same_as<ranges::iterator_t<const mutable_sized_range>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const mutable_sized_range>, CI>);
	static_assert(ranges::range<const mutable_sized_range>);
	static_assert(ranges::sized_range<const mutable_sized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<const mutable_sized_range>())), unsigned char>);
	static_assert(!ranges::_ContainerLike<const mutable_sized_range>);
	static_assert(!ranges::view<const mutable_sized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<const mutable_sized_range&>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const mutable_sized_range&>, CI>);
	static_assert(ranges::range<const mutable_sized_range&>);
	static_assert(ranges::sized_range<const mutable_sized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<const mutable_sized_range&>())), unsigned char>);
	static_assert(!ranges::_ContainerLike<const mutable_sized_range&>);
	static_assert(!ranges::view<const mutable_sized_range&>);


	static_assert(ranges::same_as<ranges::iterator_t<mutable_only_sized_range>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<mutable_only_sized_range>, I>);
	static_assert(ranges::range<mutable_only_sized_range>);
	static_assert(ranges::sized_range<mutable_only_sized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<mutable_only_sized_range>())), unsigned char>);
	static_assert(!ranges::_ContainerLike<mutable_only_sized_range>);
	static_assert(ranges::view<mutable_only_sized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<mutable_only_sized_range&>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<mutable_only_sized_range&>, I>);
	static_assert(ranges::range<mutable_only_sized_range&>);
	static_assert(ranges::sized_range<mutable_only_sized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<mutable_only_sized_range&>())), unsigned char>);
	static_assert(!ranges::_ContainerLike<mutable_only_sized_range&>);
	static_assert(!ranges::view<mutable_only_sized_range&>);

	static_assert(!ranges::range<const mutable_only_sized_range>);
	static_assert(!ranges::sized_range<const mutable_only_sized_range>);
	static_assert(!ranges::_ContainerLike<const mutable_only_sized_range>);
	static_assert(!ranges::view<const mutable_only_sized_range>);

	static_assert(!ranges::range<const mutable_only_sized_range&>);
	static_assert(!ranges::sized_range<const mutable_only_sized_range&>);
	static_assert(!ranges::_ContainerLike<const mutable_only_sized_range&>);
	static_assert(!ranges::view<const mutable_only_sized_range&>);


	static_assert(ranges::same_as<ranges::iterator_t<immutable_sized_range>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<immutable_sized_range>, CI>);
	static_assert(ranges::range<immutable_sized_range>);
	static_assert(ranges::sized_range<immutable_sized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<immutable_sized_range>())), unsigned char>);
	static_assert(!ranges::_ContainerLike<immutable_sized_range>);
	static_assert(ranges::view<immutable_sized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<immutable_sized_range&>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<immutable_sized_range&>, CI>);
	static_assert(ranges::range<immutable_sized_range&>);
	static_assert(ranges::sized_range<immutable_sized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<immutable_sized_range&>())), unsigned char>);
	static_assert(!ranges::_ContainerLike<immutable_sized_range&>);
	static_assert(!ranges::view<immutable_sized_range&>);

	static_assert(ranges::same_as<ranges::iterator_t<const immutable_sized_range>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const immutable_sized_range>, CI>);
	static_assert(ranges::range<const immutable_sized_range>);
	static_assert(ranges::sized_range<const immutable_sized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<const immutable_sized_range>())), unsigned char>);
	static_assert(!ranges::_ContainerLike<const immutable_sized_range>);
	static_assert(!ranges::view<const immutable_sized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<const immutable_sized_range&>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const immutable_sized_range&>, CI>);
	static_assert(ranges::range<const immutable_sized_range&>);
	static_assert(ranges::sized_range<const immutable_sized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<const immutable_sized_range&>())), unsigned char>);
	static_assert(!ranges::_ContainerLike<const immutable_sized_range&>);
	static_assert(!ranges::view<const immutable_sized_range&>);


	static_assert(ranges::same_as<ranges::iterator_t<mutable_badsized_range>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<mutable_badsized_range>, I>);
	static_assert(ranges::range<mutable_badsized_range>);
	static_assert(ranges::sized_range<mutable_badsized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<mutable_badsized_range>())), std::ptrdiff_t>);
	static_assert(ranges::_ContainerLike<mutable_badsized_range>);
	static_assert(!ranges::view<mutable_badsized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<mutable_badsized_range&>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<mutable_badsized_range&>, I>);
	static_assert(ranges::range<mutable_badsized_range&>);
	static_assert(ranges::sized_range<mutable_badsized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<mutable_badsized_range&>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<mutable_badsized_range&>);
	static_assert(!ranges::view<mutable_badsized_range&>);

	static_assert(ranges::same_as<ranges::iterator_t<const mutable_badsized_range>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const mutable_badsized_range>, CI>);
	static_assert(ranges::range<const mutable_badsized_range>);
	static_assert(ranges::sized_range<const mutable_badsized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<const mutable_badsized_range>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<const mutable_badsized_range>);
	static_assert(!ranges::view<const mutable_badsized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<const mutable_badsized_range&>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const mutable_badsized_range&>, CI>);
	static_assert(ranges::range<const mutable_badsized_range&>);
	static_assert(ranges::sized_range<const mutable_badsized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<const mutable_badsized_range&>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<const mutable_badsized_range&>);
	static_assert(!ranges::view<const mutable_badsized_range&>);


	static_assert(ranges::same_as<ranges::iterator_t<mutable_only_badsized_range>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<mutable_only_badsized_range>, I>);
	static_assert(ranges::range<mutable_only_badsized_range>);
	static_assert(ranges::sized_range<mutable_only_badsized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<mutable_only_badsized_range>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<mutable_only_badsized_range>);
	static_assert(ranges::view<mutable_only_badsized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<mutable_only_badsized_range&>, I>);
	static_assert(ranges::same_as<ranges::sentinel_t<mutable_only_badsized_range&>, I>);
	static_assert(ranges::range<mutable_only_badsized_range&>);
	static_assert(ranges::sized_range<mutable_only_badsized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<mutable_only_badsized_range&>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<mutable_only_badsized_range&>);
	static_assert(!ranges::view<mutable_only_badsized_range&>);

	static_assert(!ranges::range<const mutable_only_badsized_range>);
	static_assert(!ranges::sized_range<const mutable_only_badsized_range>);
	static_assert(!ranges::_ContainerLike<const mutable_only_badsized_range>);
	static_assert(!ranges::view<const mutable_only_badsized_range>);

	static_assert(!ranges::range<const mutable_only_badsized_range&>);
	static_assert(!ranges::sized_range<const mutable_only_badsized_range&>);
	static_assert(!ranges::_ContainerLike<const mutable_only_badsized_range&>);
	static_assert(!ranges::view<const mutable_only_badsized_range&>);


	static_assert(ranges::same_as<ranges::iterator_t<immutable_badsized_range>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<immutable_badsized_range>, CI>);
	static_assert(ranges::range<immutable_badsized_range>);
	static_assert(ranges::sized_range<immutable_badsized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<immutable_badsized_range>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<immutable_badsized_range>);
	static_assert(ranges::view<immutable_badsized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<immutable_badsized_range&>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<immutable_badsized_range&>, CI>);
	static_assert(ranges::range<immutable_badsized_range&>);
	static_assert(ranges::sized_range<immutable_badsized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<immutable_badsized_range&>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<immutable_badsized_range&>);
	static_assert(!ranges::view<immutable_badsized_range&>);

	static_assert(ranges::same_as<ranges::iterator_t<const immutable_badsized_range>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const immutable_badsized_range>, CI>);
	static_assert(ranges::range<const immutable_badsized_range>);
	static_assert(ranges::sized_range<const immutable_badsized_range>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<const immutable_badsized_range>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<const immutable_badsized_range>);
	static_assert(!ranges::view<const immutable_badsized_range>);

	static_assert(ranges::same_as<ranges::iterator_t<const immutable_badsized_range&>, CI>);
	static_assert(ranges::same_as<ranges::sentinel_t<const immutable_badsized_range&>, CI>);
	static_assert(ranges::range<const immutable_badsized_range&>);
	static_assert(ranges::sized_range<const immutable_badsized_range&>);
	static_assert(ranges::same_as<decltype(ranges::size(std::declval<const immutable_badsized_range&>())), std::ptrdiff_t>);
	static_assert(!ranges::_ContainerLike<const immutable_badsized_range&>);
	static_assert(!ranges::view<const immutable_badsized_range&>);


	static_assert(ranges::same_as<ranges::iterator_t<std::vector<int>>, std::vector<int>::iterator>);
	static_assert(ranges::same_as<ranges::sentinel_t<std::vector<int>>, std::vector<int>::iterator>);
	static_assert(ranges::range<std::vector<int>>);
	static_assert(ranges::sized_range<std::vector<int>>);
	static_assert(ranges::_ContainerLike<std::vector<int>>);
	static_assert(!ranges::view<std::vector<int>>);


	static_assert(ranges::range<strange_view>);
	static_assert(ranges::range<strange_view&>);
	static_assert(ranges::view<strange_view>);
	static_assert(!ranges::view<strange_view&>);
	static_assert(!ranges::view<const strange_view>);

	static_assert(ranges::range<strange_view2>);
	static_assert(ranges::range<strange_view2&>);
	static_assert(ranges::view<strange_view2>);
	static_assert(!ranges::view<strange_view2&>);
	static_assert(!ranges::view<const strange_view2>);

	static_assert(ranges::range<strange_view3>);
	static_assert(ranges::range<strange_view3&>);
	static_assert(!ranges::view<strange_view3>);
	static_assert(!ranges::view<strange_view3&>);
	static_assert(!ranges::view<const strange_view3>);
}

template<ranges::input_iterator I, ranges::sentinel_for<I> S>
I complicated_algorithm(I i, S s) {
	static constexpr bool output = false;
	if (output) std::cout << '{';
	if (i != s) {
		if (output) std::cout << *i;
		while (++i != s) {
			if (output) std::cout << ", " << *i;
		}
	}
	if (output) std::cout << "}\n";
	return i;
}

template<ranges::input_range R>
ranges::iterator_t<R> complicated_algorithm(R&& r) {
	return complicated_algorithm(ranges::begin(r), ranges::end(r));
}

template<class T>
struct array_view {
	T* first_;
	std::size_t n_;

	array_view() = default;
	template<std::size_t N>
	array_view(T (&a)[N]) : first_{a}, n_{N} {}

	auto begin() const { return first_; }
	auto end() const { return first_ + n_; }
	auto size() const { return n_; }
};

void complicated_algorithm_test() {
	static int some_ints[] = {2, 3, 5, 7};
	static_assert(ranges::range<decltype(some_ints)>);
	static_assert(ranges::sized_range<decltype(some_ints)>);
	static_assert(ranges::_ContainerLike<decltype(some_ints)>);
	static_assert(!ranges::view<decltype(some_ints)>);
	CHECK(complicated_algorithm(some_ints) == ranges::end(some_ints));
	static_assert(ranges::range<array_view<int>>);
	static_assert(ranges::sized_range<array_view<int>>);
	static_assert(!ranges::_ContainerLike<array_view<int>>);
	static_assert(ranges::view<array_view<int>>);
	CHECK(complicated_algorithm(array_view<int>{some_ints}) == ranges::end(some_ints));
}

int main() {
	ridiculously_exhaustive_range_property_test();
	complicated_algorithm_test();

	{
		using T = int[2];
		static_assert(ranges::common_range<T>);
		static_assert(ranges::output_range<T, int>);
		static_assert(ranges::output_range<T, const int&>);
		static_assert(ranges::input_range<T>);
		static_assert(ranges::forward_range<T>);
		static_assert(ranges::bidirectional_range<T>);
		static_assert(ranges::random_access_range<T>);
		static_assert(ranges::contiguous_range<T>);
	}

	static_assert(!ranges::view<std::vector<int>>);
	static_assert(!ranges::view<std::set<int>>);
	static_assert(!ranges::view<std::multiset<int>>);
	static_assert(!ranges::view<std::unordered_set<int>>);
	static_assert(!ranges::view<std::unordered_multiset<int>>);

	return ::test_result();
}
