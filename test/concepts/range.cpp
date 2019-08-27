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
#include "validate.hpp"

#if VALIDATE_RANGES
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ns {
	template<class R>
	using iterator_t = ranges::range_iterator_t<R>;

	template<class R>
	using sentinel_t = ranges::range_sentinel_t<R>;

	using ranges::size;
	using ranges::view_base;
	using ranges::begin;
	using ranges::end;
}

template<bool allow_nonconst, bool allow_const, bool allow_size>
struct arbitrary_range {
	CONCEPT_REQUIRES(allow_nonconst)
	int* begin();
	CONCEPT_REQUIRES(allow_nonconst)
	int* end();
	CONCEPT_REQUIRES(allow_const)
	const int* begin() const;
	CONCEPT_REQUIRES(allow_const)
	const int* end() const;

	CONCEPT_REQUIRES(allow_size)
	unsigned char size() const;
};

#elif VALIDATE_STL2
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>

namespace ns = ::__stl2;

template<bool allow_nonconst, bool allow_const, bool allow_size>
struct arbitrary_range {
	int* begin() requires allow_nonconst;
	int* end() requires allow_nonconst;
	const int* begin() const requires allow_const;
	const int* end() const requires allow_const;

	unsigned char size() const requires allow_size;
};
#endif

#include "../simple_test.hpp"
#include <set>
#include <unordered_set>
#include <vector>

template<class>
constexpr bool always_false = false;

using mutable_unsized_range =
	arbitrary_range<true, true, false>;

using mutable_only_no_size_range =
	arbitrary_range<true, false, false>;

using immutable_unsized_range =
	arbitrary_range<false, true, false>;

using mutable_sized_range =
	arbitrary_range<true, true, true>;

using mutable_only_sized_range =
	arbitrary_range<true, false, true>;

using immutable_sized_range =
	arbitrary_range<false, true, true>;

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

#if VALIDATE_STL2
STL2_OPEN_NAMESPACE {
template<class Base>
constexpr bool disable_sized_range<bad_sized_range<Base>> = true;
} STL2_CLOSE_NAMESPACE

#elif VALIDATE_RANGES
namespace ranges {
template<class Base>
struct is_sized_range<bad_sized_range<Base>> :
	std::false_type {};
}
#endif

struct strange_view
{
	std::vector<int>::iterator begin();
	std::vector<int>::const_iterator begin() const;

	std::vector<int>::iterator end();
	std::vector<int>::const_iterator end() const;
};

struct strange_view2 : strange_view, ns::view_base {};
struct strange_view3 : strange_view2 {};

#if VALIDATE_STL2
STL2_OPEN_NAMESPACE {
	template<>
	inline constexpr bool enable_view<strange_view> = true;
	template<>
	inline constexpr bool enable_view<strange_view3> = false;
} STL2_CLOSE_NAMESPACE

#elif VALIDATE_RANGES
namespace ranges {
	template<>
	struct is_view<strange_view> : std::true_type {};
	template<>
	struct is_view<strange_view3> : std::false_type {};
}
#endif

void ridiculously_exhaustive_range_property_test() {
	CONCEPT_ASSERT(!ranges::range<void>);
	CONCEPT_ASSERT(!ranges::sized_range<void>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<void>);
	CONCEPT_ASSERT(!ranges::view<void>);

	using I = int*;
	using CI = const int*;

	CONCEPT_ASSERT(ranges::input_or_output_iterator<I>);
	CONCEPT_ASSERT(ranges::input_or_output_iterator<CI>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<int[2]>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<int[2]>, I>);
	CONCEPT_ASSERT(ranges::range<int[2]>);
	CONCEPT_ASSERT(ranges::sized_range<int[2]>);
	CONCEPT_ASSERT(ranges::_ContainerLike<int[2]>);
	CONCEPT_ASSERT(!ranges::view<int[2]>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<int(&)[2]>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<int(&)[2]>, I>);
	CONCEPT_ASSERT(ranges::range<int(&)[2]>);
	CONCEPT_ASSERT(ranges::sized_range<int(&)[2]>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<int(&)[2]>);
	CONCEPT_ASSERT(!ranges::view<int(&)[2]>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const int[2]>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const int[2]>, CI>);
	CONCEPT_ASSERT(ranges::range<const int[2]>);
	CONCEPT_ASSERT(ranges::sized_range<const int[2]>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const int[2]>);
	CONCEPT_ASSERT(!ranges::view<const int[2]>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const int(&)[2]>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const int(&)[2]>, CI>);
	CONCEPT_ASSERT(ranges::range<const int(&)[2]>);
	CONCEPT_ASSERT(ranges::sized_range<const int(&)[2]>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const int(&)[2]>);
	CONCEPT_ASSERT(!ranges::view<const int(&)[2]>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<mutable_unsized_range>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<mutable_unsized_range>, I>);
	CONCEPT_ASSERT(ranges::range<mutable_unsized_range>);
	CONCEPT_ASSERT(ranges::sized_range<mutable_unsized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<mutable_unsized_range>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(ranges::_ContainerLike<mutable_unsized_range>);
	CONCEPT_ASSERT(!ranges::view<mutable_unsized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<mutable_unsized_range&>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<mutable_unsized_range&>, I>);
	CONCEPT_ASSERT(ranges::range<mutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<mutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<mutable_unsized_range&>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_unsized_range&>);
	CONCEPT_ASSERT(!ranges::view<mutable_unsized_range&>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const mutable_unsized_range>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const mutable_unsized_range>, CI>);
	CONCEPT_ASSERT(ranges::range<const mutable_unsized_range>);
	CONCEPT_ASSERT(ranges::sized_range<const mutable_unsized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<const mutable_unsized_range>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_unsized_range>);
	CONCEPT_ASSERT(!ranges::view<const mutable_unsized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const mutable_unsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const mutable_unsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::range<const mutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<const mutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<const mutable_unsized_range&>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_unsized_range&>);
	CONCEPT_ASSERT(!ranges::view<const mutable_unsized_range&>);


	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<mutable_only_no_size_range>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<mutable_only_no_size_range>, I>);
	CONCEPT_ASSERT(ranges::range<mutable_only_no_size_range>);
	CONCEPT_ASSERT(ranges::sized_range<mutable_only_no_size_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<mutable_only_no_size_range>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_only_no_size_range>);
	CONCEPT_ASSERT(ranges::view<mutable_only_no_size_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<mutable_only_no_size_range&>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<mutable_only_no_size_range&>, I>);
	CONCEPT_ASSERT(ranges::range<mutable_only_no_size_range&>);
	CONCEPT_ASSERT(ranges::sized_range<mutable_only_no_size_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<mutable_only_no_size_range&>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_only_no_size_range&>);
	CONCEPT_ASSERT(!ranges::view<mutable_only_no_size_range&>);

	CONCEPT_ASSERT(!ranges::range<const mutable_only_no_size_range>);
	CONCEPT_ASSERT(!ranges::sized_range<const mutable_only_no_size_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_only_no_size_range>);
	CONCEPT_ASSERT(!ranges::view<const mutable_only_no_size_range>);

	CONCEPT_ASSERT(!ranges::range<const mutable_only_no_size_range&>);
	CONCEPT_ASSERT(!ranges::sized_range<const mutable_only_no_size_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_only_no_size_range&>);
	CONCEPT_ASSERT(!ranges::view<const mutable_only_no_size_range&>);


	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<immutable_unsized_range>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<immutable_unsized_range>, CI>);
	CONCEPT_ASSERT(ranges::range<immutable_unsized_range>);
	CONCEPT_ASSERT(ranges::sized_range<immutable_unsized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<immutable_unsized_range>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<immutable_unsized_range>);
	CONCEPT_ASSERT(ranges::view<immutable_unsized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<immutable_unsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<immutable_unsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::range<immutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<immutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<immutable_unsized_range&>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<immutable_unsized_range&>);
	CONCEPT_ASSERT(!ranges::view<immutable_unsized_range&>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const immutable_unsized_range>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const immutable_unsized_range>, CI>);
	CONCEPT_ASSERT(ranges::range<const immutable_unsized_range>);
	CONCEPT_ASSERT(ranges::sized_range<const immutable_unsized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<const immutable_unsized_range>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const immutable_unsized_range>);
	CONCEPT_ASSERT(!ranges::view<const immutable_unsized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const immutable_unsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const immutable_unsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::range<const immutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<const immutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<const immutable_unsized_range&>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const immutable_unsized_range&>);
	CONCEPT_ASSERT(!ranges::view<const immutable_unsized_range&>);


	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<mutable_sized_range>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<mutable_sized_range>, I>);
	CONCEPT_ASSERT(ranges::range<mutable_sized_range>);
	CONCEPT_ASSERT(ranges::sized_range<mutable_sized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<mutable_sized_range>())), unsigned char>);
	CONCEPT_ASSERT(ranges::_ContainerLike<mutable_sized_range>);
	CONCEPT_ASSERT(!ranges::view<mutable_sized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<mutable_sized_range&>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<mutable_sized_range&>, I>);
	CONCEPT_ASSERT(ranges::range<mutable_sized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<mutable_sized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<mutable_sized_range&>())), unsigned char>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_sized_range&>);
	CONCEPT_ASSERT(!ranges::view<mutable_sized_range&>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const mutable_sized_range>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const mutable_sized_range>, CI>);
	CONCEPT_ASSERT(ranges::range<const mutable_sized_range>);
	CONCEPT_ASSERT(ranges::sized_range<const mutable_sized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<const mutable_sized_range>())), unsigned char>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_sized_range>);
	CONCEPT_ASSERT(!ranges::view<const mutable_sized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const mutable_sized_range&>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const mutable_sized_range&>, CI>);
	CONCEPT_ASSERT(ranges::range<const mutable_sized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<const mutable_sized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<const mutable_sized_range&>())), unsigned char>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_sized_range&>);
	CONCEPT_ASSERT(!ranges::view<const mutable_sized_range&>);


	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<mutable_only_sized_range>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<mutable_only_sized_range>, I>);
	CONCEPT_ASSERT(ranges::range<mutable_only_sized_range>);
	CONCEPT_ASSERT(ranges::sized_range<mutable_only_sized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<mutable_only_sized_range>())), unsigned char>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_only_sized_range>);
	CONCEPT_ASSERT(ranges::view<mutable_only_sized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<mutable_only_sized_range&>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<mutable_only_sized_range&>, I>);
	CONCEPT_ASSERT(ranges::range<mutable_only_sized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<mutable_only_sized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<mutable_only_sized_range&>())), unsigned char>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_only_sized_range&>);
	CONCEPT_ASSERT(!ranges::view<mutable_only_sized_range&>);

	CONCEPT_ASSERT(!ranges::range<const mutable_only_sized_range>);
	CONCEPT_ASSERT(!ranges::sized_range<const mutable_only_sized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_only_sized_range>);
	CONCEPT_ASSERT(!ranges::view<const mutable_only_sized_range>);

	CONCEPT_ASSERT(!ranges::range<const mutable_only_sized_range&>);
	CONCEPT_ASSERT(!ranges::sized_range<const mutable_only_sized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_only_sized_range&>);
	CONCEPT_ASSERT(!ranges::view<const mutable_only_sized_range&>);


	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<immutable_sized_range>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<immutable_sized_range>, CI>);
	CONCEPT_ASSERT(ranges::range<immutable_sized_range>);
	CONCEPT_ASSERT(ranges::sized_range<immutable_sized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<immutable_sized_range>())), unsigned char>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<immutable_sized_range>);
	CONCEPT_ASSERT(ranges::view<immutable_sized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<immutable_sized_range&>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<immutable_sized_range&>, CI>);
	CONCEPT_ASSERT(ranges::range<immutable_sized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<immutable_sized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<immutable_sized_range&>())), unsigned char>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<immutable_sized_range&>);
	CONCEPT_ASSERT(!ranges::view<immutable_sized_range&>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const immutable_sized_range>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const immutable_sized_range>, CI>);
	CONCEPT_ASSERT(ranges::range<const immutable_sized_range>);
	CONCEPT_ASSERT(ranges::sized_range<const immutable_sized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<const immutable_sized_range>())), unsigned char>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const immutable_sized_range>);
	CONCEPT_ASSERT(!ranges::view<const immutable_sized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const immutable_sized_range&>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const immutable_sized_range&>, CI>);
	CONCEPT_ASSERT(ranges::range<const immutable_sized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<const immutable_sized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<const immutable_sized_range&>())), unsigned char>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const immutable_sized_range&>);
	CONCEPT_ASSERT(!ranges::view<const immutable_sized_range&>);


	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<mutable_badsized_range>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<mutable_badsized_range>, I>);
	CONCEPT_ASSERT(ranges::range<mutable_badsized_range>);
	CONCEPT_ASSERT(ranges::sized_range<mutable_badsized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<mutable_badsized_range>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(ranges::_ContainerLike<mutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::view<mutable_badsized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<mutable_badsized_range&>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<mutable_badsized_range&>, I>);
	CONCEPT_ASSERT(ranges::range<mutable_badsized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<mutable_badsized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<mutable_badsized_range&>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::view<mutable_badsized_range&>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const mutable_badsized_range>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const mutable_badsized_range>, CI>);
	CONCEPT_ASSERT(ranges::range<const mutable_badsized_range>);
	CONCEPT_ASSERT(ranges::sized_range<const mutable_badsized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<const mutable_badsized_range>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::view<const mutable_badsized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const mutable_badsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const mutable_badsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::range<const mutable_badsized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<const mutable_badsized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<const mutable_badsized_range&>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::view<const mutable_badsized_range&>);


	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<mutable_only_badsized_range>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<mutable_only_badsized_range>, I>);
	CONCEPT_ASSERT(ranges::range<mutable_only_badsized_range>);
	CONCEPT_ASSERT(ranges::sized_range<mutable_only_badsized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<mutable_only_badsized_range>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_only_badsized_range>);
	CONCEPT_ASSERT(ranges::view<mutable_only_badsized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<mutable_only_badsized_range&>, I>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<mutable_only_badsized_range&>, I>);
	CONCEPT_ASSERT(ranges::range<mutable_only_badsized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<mutable_only_badsized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<mutable_only_badsized_range&>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_only_badsized_range&>);
	CONCEPT_ASSERT(!ranges::view<mutable_only_badsized_range&>);

	CONCEPT_ASSERT(!ranges::range<const mutable_only_badsized_range>);
	CONCEPT_ASSERT(!ranges::sized_range<const mutable_only_badsized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_only_badsized_range>);
	CONCEPT_ASSERT(!ranges::view<const mutable_only_badsized_range>);

	CONCEPT_ASSERT(!ranges::range<const mutable_only_badsized_range&>);
	CONCEPT_ASSERT(!ranges::sized_range<const mutable_only_badsized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_only_badsized_range&>);
	CONCEPT_ASSERT(!ranges::view<const mutable_only_badsized_range&>);


	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<immutable_badsized_range>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<immutable_badsized_range>, CI>);
	CONCEPT_ASSERT(ranges::range<immutable_badsized_range>);
	CONCEPT_ASSERT(ranges::sized_range<immutable_badsized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<immutable_badsized_range>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<immutable_badsized_range>);
	CONCEPT_ASSERT(ranges::view<immutable_badsized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<immutable_badsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<immutable_badsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::range<immutable_badsized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<immutable_badsized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<immutable_badsized_range&>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<immutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::view<immutable_badsized_range&>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const immutable_badsized_range>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const immutable_badsized_range>, CI>);
	CONCEPT_ASSERT(ranges::range<const immutable_badsized_range>);
	CONCEPT_ASSERT(ranges::sized_range<const immutable_badsized_range>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<const immutable_badsized_range>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const immutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::view<const immutable_badsized_range>);

	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<const immutable_badsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<const immutable_badsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::range<const immutable_badsized_range&>);
	CONCEPT_ASSERT(ranges::sized_range<const immutable_badsized_range&>);
	CONCEPT_ASSERT(ranges::same_as<decltype(ranges::size(std::declval<const immutable_badsized_range&>())), std::ptrdiff_t>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const immutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::view<const immutable_badsized_range&>);


	CONCEPT_ASSERT(ranges::same_as<ns::iterator_t<std::vector<int>>, std::vector<int>::iterator>);
	CONCEPT_ASSERT(ranges::same_as<ns::sentinel_t<std::vector<int>>, std::vector<int>::iterator>);
	CONCEPT_ASSERT(ranges::range<std::vector<int>>);
	CONCEPT_ASSERT(ranges::sized_range<std::vector<int>>);
	CONCEPT_ASSERT(ranges::_ContainerLike<std::vector<int>>);
	CONCEPT_ASSERT(!ranges::view<std::vector<int>>);


	CONCEPT_ASSERT(ranges::range<strange_view>);
	CONCEPT_ASSERT(ranges::range<strange_view&>);
	CONCEPT_ASSERT(ranges::view<strange_view>);
	CONCEPT_ASSERT(!ranges::view<strange_view&>);
	CONCEPT_ASSERT(!ranges::view<const strange_view>);

	CONCEPT_ASSERT(ranges::range<strange_view2>);
	CONCEPT_ASSERT(ranges::range<strange_view2&>);
	CONCEPT_ASSERT(ranges::view<strange_view2>);
	CONCEPT_ASSERT(!ranges::view<strange_view2&>);
	CONCEPT_ASSERT(!ranges::view<const strange_view2>);

	CONCEPT_ASSERT(ranges::range<strange_view3>);
	CONCEPT_ASSERT(ranges::range<strange_view3&>);
	CONCEPT_ASSERT(!ranges::view<strange_view3>);
	CONCEPT_ASSERT(!ranges::view<strange_view3&>);
	CONCEPT_ASSERT(!ranges::view<const strange_view3>);
}

#if VALIDATE_RANGES
template<class I, class S,
	CONCEPT_REQUIRES_(ranges::input_iterator<I> && ranges::sentinel_for<S, I>)>
#elif VALIDATE_STL2
template<ns::input_iterator I, ns::sentinel_for<I> S>
#endif
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

#if VALIDATE_RANGES
template<class R, CONCEPT_REQUIRES_(ranges::range<R>)>
#elif VALIDATE_STL2
template<ns::input_range R>
#endif
ns::iterator_t<R> complicated_algorithm(R&& r) {
	return complicated_algorithm(ns::begin(r), ns::end(r));
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
	CONCEPT_ASSERT(ranges::range<decltype(some_ints)>);
	CONCEPT_ASSERT(ranges::sized_range<decltype(some_ints)>);
	CONCEPT_ASSERT(ranges::_ContainerLike<decltype(some_ints)>);
	CONCEPT_ASSERT(!ranges::view<decltype(some_ints)>);
	CHECK(complicated_algorithm(some_ints) == ns::end(some_ints));
	CONCEPT_ASSERT(ranges::range<array_view<int>>);
	CONCEPT_ASSERT(ranges::sized_range<array_view<int>>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<array_view<int>>);
	CONCEPT_ASSERT(ranges::view<array_view<int>>);
	CHECK(complicated_algorithm(array_view<int>{some_ints}) == ns::end(some_ints));
}

int main() {
	ridiculously_exhaustive_range_property_test();
	complicated_algorithm_test();

	{
		using T = int[2];
		CONCEPT_ASSERT(ranges::common_range<T>);
		CONCEPT_ASSERT(ranges::output_range<T, int>);
		CONCEPT_ASSERT(ranges::output_range<T, const int&>);
		CONCEPT_ASSERT(ranges::input_range<T>);
		CONCEPT_ASSERT(ranges::forward_range<T>);
		CONCEPT_ASSERT(ranges::bidirectional_range<T>);
		CONCEPT_ASSERT(ranges::random_access_range<T>);
#if VALIDATE_STL2
		CONCEPT_ASSERT(ranges::contiguous_range<T>);
#endif
	}

	CONCEPT_ASSERT(!ranges::view<std::vector<int>>);
	CONCEPT_ASSERT(!ranges::view<std::set<int>>);
	CONCEPT_ASSERT(!ranges::view<std::multiset<int>>);
	CONCEPT_ASSERT(!ranges::view<std::unordered_set<int>>);
	CONCEPT_ASSERT(!ranges::view<std::unordered_multiset<int>>);

	return ::test_result();
}
