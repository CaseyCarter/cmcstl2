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
	int size() const;
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

	int size() const requires allow_size;
};
#endif

#include "../simple_test.hpp"
#include <set>
#include <unordered_set>
#include <vector>

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
struct mutable_badsized_range :
	mutable_sized_range {};

struct mutable_only_badsized_range :
	mutable_only_sized_range {};

struct immutable_badsized_range :
	immutable_sized_range {};

#if VALIDATE_STL2
STL2_OPEN_NAMESPACE {
template<>
constexpr bool disable_sized_range<mutable_badsized_range> = true;
template<>
constexpr bool disable_sized_range<mutable_only_badsized_range> = true;
template<>
constexpr bool disable_sized_range<immutable_badsized_range> = true;
} STL2_CLOSE_NAMESPACE

#elif VALIDATE_RANGES
namespace ranges {
template<>
struct is_sized_range<mutable_badsized_range> :
	std::false_type {};
template<>
struct is_sized_range<mutable_only_badsized_range> :
	std::false_type {};
template<>
struct is_sized_range<immutable_badsized_range> :
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
	CONCEPT_ASSERT(!ranges::Range<void>);
	CONCEPT_ASSERT(!ranges::SizedRange<void>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<void>);
	CONCEPT_ASSERT(!ranges::View<void>);

	using I = int*;
	using CI = const int*;

	CONCEPT_ASSERT(ranges::Iterator<I>);
	CONCEPT_ASSERT(ranges::Iterator<CI>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<int[2]>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<int[2]>, I>);
	CONCEPT_ASSERT(ranges::Range<int[2]>);
	CONCEPT_ASSERT(ranges::SizedRange<int[2]>);
	CONCEPT_ASSERT(ranges::_ContainerLike<int[2]>);
	CONCEPT_ASSERT(!ranges::View<int[2]>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<int(&)[2]>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<int(&)[2]>, I>);
	CONCEPT_ASSERT(ranges::Range<int(&)[2]>);
	CONCEPT_ASSERT(ranges::SizedRange<int(&)[2]>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<int(&)[2]>);
	CONCEPT_ASSERT(!ranges::View<int(&)[2]>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const int[2]>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const int[2]>, CI>);
	CONCEPT_ASSERT(ranges::Range<const int[2]>);
	CONCEPT_ASSERT(ranges::SizedRange<const int[2]>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const int[2]>);
	CONCEPT_ASSERT(!ranges::View<const int[2]>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const int(&)[2]>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const int(&)[2]>, CI>);
	CONCEPT_ASSERT(ranges::Range<const int(&)[2]>);
	CONCEPT_ASSERT(ranges::SizedRange<const int(&)[2]>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const int(&)[2]>);
	CONCEPT_ASSERT(!ranges::View<const int(&)[2]>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<mutable_unsized_range>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<mutable_unsized_range>, I>);
	CONCEPT_ASSERT(ranges::Range<mutable_unsized_range>);
	CONCEPT_ASSERT(ranges::SizedRange<mutable_unsized_range>);
	CONCEPT_ASSERT(ranges::_ContainerLike<mutable_unsized_range>);
	CONCEPT_ASSERT(!ranges::View<mutable_unsized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<mutable_unsized_range&>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<mutable_unsized_range&>, I>);
	CONCEPT_ASSERT(ranges::Range<mutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::SizedRange<mutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::_ContainerLike<mutable_unsized_range>);
	CONCEPT_ASSERT(!ranges::View<mutable_unsized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const mutable_unsized_range>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const mutable_unsized_range>, CI>);
	CONCEPT_ASSERT(ranges::Range<const mutable_unsized_range>);
	CONCEPT_ASSERT(ranges::SizedRange<const mutable_unsized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_unsized_range>);
	CONCEPT_ASSERT(!ranges::View<const mutable_unsized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const mutable_unsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const mutable_unsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Range<const mutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::SizedRange<const mutable_unsized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_unsized_range&>);
	CONCEPT_ASSERT(!ranges::View<const mutable_unsized_range&>);


	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<mutable_only_no_size_range&>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<mutable_only_no_size_range&>, I>);
	CONCEPT_ASSERT(ranges::Range<mutable_only_no_size_range>);
	CONCEPT_ASSERT(ranges::SizedRange<mutable_only_no_size_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_only_no_size_range>);
	CONCEPT_ASSERT(ranges::View<mutable_only_no_size_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<mutable_only_no_size_range&>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<mutable_only_no_size_range&>, I>);
	CONCEPT_ASSERT(ranges::Range<mutable_only_no_size_range&>);
	CONCEPT_ASSERT(ranges::SizedRange<mutable_only_no_size_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_only_no_size_range&>);
	CONCEPT_ASSERT(!ranges::View<mutable_only_no_size_range&>);

	CONCEPT_ASSERT(!ranges::Range<const mutable_only_no_size_range>);
	CONCEPT_ASSERT(!ranges::SizedRange<const mutable_only_no_size_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_only_no_size_range>);
	CONCEPT_ASSERT(!ranges::View<const mutable_only_no_size_range>);

	CONCEPT_ASSERT(!ranges::Range<const mutable_only_no_size_range&>);
	CONCEPT_ASSERT(!ranges::SizedRange<const mutable_only_no_size_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_only_no_size_range&>);
	CONCEPT_ASSERT(!ranges::View<const mutable_only_no_size_range&>);


	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<immutable_unsized_range>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<immutable_unsized_range>, CI>);
	CONCEPT_ASSERT(ranges::Range<immutable_unsized_range>);
	CONCEPT_ASSERT(ranges::SizedRange<immutable_unsized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<immutable_unsized_range>);
	CONCEPT_ASSERT(ranges::View<immutable_unsized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<immutable_unsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<immutable_unsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Range<immutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::SizedRange<immutable_unsized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<immutable_unsized_range&>);
	CONCEPT_ASSERT(!ranges::View<immutable_unsized_range&>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const immutable_unsized_range>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const immutable_unsized_range>, CI>);
	CONCEPT_ASSERT(ranges::Range<const immutable_unsized_range>);
	CONCEPT_ASSERT(ranges::SizedRange<const immutable_unsized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const immutable_unsized_range>);
	CONCEPT_ASSERT(!ranges::View<const immutable_unsized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const immutable_unsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const immutable_unsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Range<const immutable_unsized_range&>);
	CONCEPT_ASSERT(ranges::SizedRange<const immutable_unsized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const immutable_unsized_range&>);
	CONCEPT_ASSERT(!ranges::View<const immutable_unsized_range&>);


	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<mutable_sized_range>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<mutable_sized_range>, I>);
	CONCEPT_ASSERT(ranges::Range<mutable_sized_range>);
	CONCEPT_ASSERT(ranges::SizedRange<mutable_sized_range>);
	CONCEPT_ASSERT(ranges::_ContainerLike<mutable_sized_range>);
	CONCEPT_ASSERT(!ranges::View<mutable_sized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<mutable_sized_range&>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<mutable_sized_range&>, I>);
	CONCEPT_ASSERT(ranges::Range<mutable_sized_range&>);
	CONCEPT_ASSERT(ranges::SizedRange<mutable_sized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_sized_range&>);
	CONCEPT_ASSERT(!ranges::View<mutable_sized_range&>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const mutable_sized_range>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const mutable_sized_range>, CI>);
	CONCEPT_ASSERT(ranges::Range<const mutable_sized_range>);
	CONCEPT_ASSERT(ranges::SizedRange<const mutable_sized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_sized_range>);
	CONCEPT_ASSERT(!ranges::View<const mutable_sized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const mutable_sized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const mutable_sized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Range<const mutable_sized_range&>);
	CONCEPT_ASSERT(ranges::SizedRange<const mutable_sized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_sized_range&>);
	CONCEPT_ASSERT(!ranges::View<const mutable_sized_range&>);


	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<mutable_only_sized_range>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<mutable_only_sized_range>, I>);
	CONCEPT_ASSERT(ranges::Range<mutable_only_sized_range>);
	CONCEPT_ASSERT(ranges::SizedRange<mutable_only_sized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_only_sized_range>);
	CONCEPT_ASSERT(ranges::View<mutable_only_sized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<mutable_only_sized_range&>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<mutable_only_sized_range&>, I>);
	CONCEPT_ASSERT(ranges::Range<mutable_only_sized_range&>);
	CONCEPT_ASSERT(ranges::SizedRange<mutable_only_sized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_only_sized_range&>);
	CONCEPT_ASSERT(!ranges::View<mutable_only_sized_range&>);

	CONCEPT_ASSERT(!ranges::Range<const mutable_only_sized_range>);
	CONCEPT_ASSERT(!ranges::SizedRange<const mutable_only_sized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_only_sized_range>);
	CONCEPT_ASSERT(!ranges::View<const mutable_only_sized_range>);

	CONCEPT_ASSERT(!ranges::Range<const mutable_only_sized_range&>);
	CONCEPT_ASSERT(!ranges::SizedRange<const mutable_only_sized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_only_sized_range&>);
	CONCEPT_ASSERT(!ranges::View<const mutable_only_sized_range&>);


	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<immutable_sized_range>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<immutable_sized_range>, CI>);
	CONCEPT_ASSERT(ranges::Range<immutable_sized_range>);
	CONCEPT_ASSERT(ranges::SizedRange<immutable_sized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<immutable_sized_range>);
	CONCEPT_ASSERT(ranges::View<immutable_sized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<immutable_sized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<immutable_sized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Range<immutable_sized_range&>);
	CONCEPT_ASSERT(ranges::SizedRange<immutable_sized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<immutable_sized_range&>);
	CONCEPT_ASSERT(!ranges::View<immutable_sized_range&>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const immutable_sized_range>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const immutable_sized_range>, CI>);
	CONCEPT_ASSERT(ranges::Range<const immutable_sized_range>);
	CONCEPT_ASSERT(ranges::SizedRange<const immutable_sized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const immutable_sized_range>);
	CONCEPT_ASSERT(!ranges::View<const immutable_sized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const immutable_sized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const immutable_sized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Range<const immutable_sized_range&>);
	CONCEPT_ASSERT(ranges::SizedRange<const immutable_sized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const immutable_sized_range&>);
	CONCEPT_ASSERT(!ranges::View<const immutable_sized_range&>);


	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<mutable_badsized_range>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<mutable_badsized_range>, I>);
	CONCEPT_ASSERT(ranges::Range<mutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::SizedRange<mutable_badsized_range>);
	CONCEPT_ASSERT(ranges::_ContainerLike<mutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::View<mutable_badsized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<mutable_badsized_range&>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<mutable_badsized_range&>, I>);
	CONCEPT_ASSERT(ranges::Range<mutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::SizedRange<mutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::View<mutable_badsized_range&>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const mutable_badsized_range>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const mutable_badsized_range>, CI>);
	CONCEPT_ASSERT(ranges::Range<const mutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::SizedRange<const mutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::View<const mutable_badsized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const mutable_badsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const mutable_badsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Range<const mutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::SizedRange<const mutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::View<const mutable_badsized_range&>);


	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<mutable_only_badsized_range>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<mutable_only_badsized_range>, I>);
	CONCEPT_ASSERT(ranges::Range<mutable_only_badsized_range>);
	CONCEPT_ASSERT(!ranges::SizedRange<mutable_only_badsized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_only_badsized_range>);
	CONCEPT_ASSERT(ranges::View<mutable_only_badsized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<mutable_only_badsized_range&>, I>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<mutable_only_badsized_range&>, I>);
	CONCEPT_ASSERT(ranges::Range<mutable_only_badsized_range&>);
	CONCEPT_ASSERT(!ranges::SizedRange<mutable_only_badsized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<mutable_only_badsized_range&>);
	CONCEPT_ASSERT(!ranges::View<mutable_only_badsized_range&>);

	CONCEPT_ASSERT(!ranges::Range<const mutable_only_badsized_range>);
	CONCEPT_ASSERT(!ranges::SizedRange<const mutable_only_badsized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_only_badsized_range>);
	CONCEPT_ASSERT(!ranges::View<const mutable_only_badsized_range>);

	CONCEPT_ASSERT(!ranges::Range<const mutable_only_badsized_range&>);
	CONCEPT_ASSERT(!ranges::SizedRange<const mutable_only_badsized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const mutable_only_badsized_range&>);
	CONCEPT_ASSERT(!ranges::View<const mutable_only_badsized_range&>);


	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<immutable_badsized_range>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<immutable_badsized_range>, CI>);
	CONCEPT_ASSERT(ranges::Range<immutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::SizedRange<immutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<immutable_badsized_range>);
	CONCEPT_ASSERT(ranges::View<immutable_badsized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<immutable_badsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<immutable_badsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Range<immutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::SizedRange<immutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<immutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::View<immutable_badsized_range&>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const immutable_badsized_range>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const immutable_badsized_range>, CI>);
	CONCEPT_ASSERT(ranges::Range<const immutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::SizedRange<const immutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const immutable_badsized_range>);
	CONCEPT_ASSERT(!ranges::View<const immutable_badsized_range>);

	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<const immutable_badsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<const immutable_badsized_range&>, CI>);
	CONCEPT_ASSERT(ranges::Range<const immutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::SizedRange<const immutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<const immutable_badsized_range&>);
	CONCEPT_ASSERT(!ranges::View<const immutable_badsized_range&>);


	CONCEPT_ASSERT(ranges::Same<ns::iterator_t<std::vector<int>>, std::vector<int>::iterator>);
	CONCEPT_ASSERT(ranges::Same<ns::sentinel_t<std::vector<int>>, std::vector<int>::iterator>);
	CONCEPT_ASSERT(ranges::Range<std::vector<int>>);
	CONCEPT_ASSERT(ranges::SizedRange<std::vector<int>>);
	CONCEPT_ASSERT(ranges::_ContainerLike<std::vector<int>>);
	CONCEPT_ASSERT(!ranges::View<std::vector<int>>);


	CONCEPT_ASSERT(ranges::Range<strange_view>);
	CONCEPT_ASSERT(ranges::Range<strange_view&>);
	CONCEPT_ASSERT(ranges::View<strange_view>);
	CONCEPT_ASSERT(!ranges::View<strange_view&>);
	CONCEPT_ASSERT(!ranges::View<const strange_view>);

	CONCEPT_ASSERT(ranges::Range<strange_view2>);
	CONCEPT_ASSERT(ranges::Range<strange_view2&>);
	CONCEPT_ASSERT(ranges::View<strange_view2>);
	CONCEPT_ASSERT(!ranges::View<strange_view2&>);
	CONCEPT_ASSERT(!ranges::View<const strange_view2>);

	CONCEPT_ASSERT(ranges::Range<strange_view3>);
	CONCEPT_ASSERT(ranges::Range<strange_view3&>);
	CONCEPT_ASSERT(!ranges::View<strange_view3>);
	CONCEPT_ASSERT(!ranges::View<strange_view3&>);
	CONCEPT_ASSERT(!ranges::View<const strange_view3>);

	CONCEPT_ASSERT(ranges::Range<mutable_only_no_size_range&>);
	CONCEPT_ASSERT(ranges::Range<mutable_only_no_size_range>);
	CONCEPT_ASSERT(ranges::View<mutable_only_no_size_range>);
	CONCEPT_ASSERT(!ranges::View<mutable_only_no_size_range&>);
	CONCEPT_ASSERT(!ranges::View<mutable_only_no_size_range&&>);
	CONCEPT_ASSERT(!ranges::Range<const mutable_only_no_size_range&>);
	CONCEPT_ASSERT(!ranges::View<const mutable_only_no_size_range&>);
}

#if VALIDATE_RANGES
template<class I, class S,
	CONCEPT_REQUIRES_(ranges::InputIterator<I> && ranges::Sentinel<S, I>)>
#elif VALIDATE_STL2
template<ns::InputIterator I, ns::Sentinel<I> S>
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
template<class R, CONCEPT_REQUIRES_(ranges::Range<R>)>
#elif VALIDATE_STL2
template<ns::InputRange R>
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
	CONCEPT_ASSERT(ranges::Range<decltype(some_ints)>);
	CONCEPT_ASSERT(ranges::SizedRange<decltype(some_ints)>);
	CONCEPT_ASSERT(ranges::_ContainerLike<decltype(some_ints)>);
	CONCEPT_ASSERT(!ranges::View<decltype(some_ints)>);
	CHECK(complicated_algorithm(some_ints) == ns::end(some_ints));
	CONCEPT_ASSERT(ranges::Range<array_view<int>>);
	CONCEPT_ASSERT(ranges::SizedRange<array_view<int>>);
	CONCEPT_ASSERT(!ranges::_ContainerLike<array_view<int>>);
	CONCEPT_ASSERT(ranges::View<array_view<int>>);
	CHECK(complicated_algorithm(array_view<int>{some_ints}) == ns::end(some_ints));
}

int main() {
	ridiculously_exhaustive_range_property_test();
	complicated_algorithm_test();

	{
		using T = int[2];
		CONCEPT_ASSERT(ranges::CommonRange<T>);
		CONCEPT_ASSERT(ranges::OutputRange<T, int>);
		CONCEPT_ASSERT(ranges::OutputRange<T, const int&>);
		CONCEPT_ASSERT(ranges::InputRange<T>);
		CONCEPT_ASSERT(ranges::ForwardRange<T>);
		CONCEPT_ASSERT(ranges::BidirectionalRange<T>);
		CONCEPT_ASSERT(ranges::RandomAccessRange<T>);
#if VALIDATE_STL2
		CONCEPT_ASSERT(ranges::ContiguousRange<T>);
#endif
	}

	CONCEPT_ASSERT(!ranges::View<std::vector<int>>);
	CONCEPT_ASSERT(!ranges::View<std::set<int>>);
	CONCEPT_ASSERT(!ranges::View<std::multiset<int>>);
	CONCEPT_ASSERT(!ranges::View<std::unordered_set<int>>);
	CONCEPT_ASSERT(!ranges::View<std::unordered_multiset<int>>);

	return ::test_result();
}
