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
#ifndef STL2_DETAIL_RANGE_CONCEPTS_HPP
#define STL2_DETAIL_RANGE_CONCEPTS_HPP

#include <initializer_list>

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>

namespace std {
#if !defined(__GLIBCXX__) && !defined(_MSVC_STL_VERSION)
#pragma message "These forward declarations will only work with standard " \
	"libraries that don't use inline namespaces for versioning."
#endif
	template<class, class, class> class set;
	template<class, class, class> class multiset;
	template<class, class, class, class> class unordered_set;
	template<class, class, class, class> class unordered_multiset;
}

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// range [ranges.range]
	//
	template<class T>
	using iterator_t = __begin_t<T&>;

	template<class T>
	using sentinel_t = __end_t<T&>;

	template<class T>
	META_CONCEPT _RangeImpl =
		requires(T&& t) {
			begin(static_cast<T&&>(t)); // not necessarily equality-preserving
			end(static_cast<T&&>(t));
		};

	template<class T>
	META_CONCEPT range = _RangeImpl<T&>;

	template<class T>
	META_CONCEPT _ForwardingRange = range<T> && _RangeImpl<T>;

	template<class R>
	META_CONCEPT sized_range =
		range<R> && !disable_sized_range<__uncvref<R>> &&
		requires(R& r) { size(r); };

	///////////////////////////////////////////////////////////////////////////
	// view [ranges.view]
	//
	struct view_base {};

	template<class T>
	META_CONCEPT _ContainerLike =
		range<T> && range<const T> &&
		!same_as<iter_reference_t<iterator_t<T>>, iter_reference_t<iterator_t<const T>>>;

	template<class T>
	META_CONCEPT __enable_view_default = derived_from<T, view_base> || !_ContainerLike<T>;

	template<class T>
	inline constexpr bool enable_view = __enable_view_default<T>;

	template<class T>
	inline constexpr bool enable_view<std::initializer_list<T>> = false;
	template<class Key, class Compare, class Alloc>
	inline constexpr bool enable_view<std::set<Key, Compare, Alloc>> = false;
	template<class Key, class Compare, class Alloc>
	inline constexpr bool enable_view<std::multiset<Key, Compare, Alloc>> = false;
	template<class Key, class Hash, class Pred, class Alloc>
	inline constexpr bool enable_view<std::unordered_set<Key, Hash, Pred, Alloc>> = false;
	template<class Key, class Hash, class Pred, class Alloc>
	inline constexpr bool enable_view<std::unordered_multiset<Key, Hash, Pred, Alloc>> = false;

	template<class T>
	META_CONCEPT view =
		range<T> &&
		semiregular<T> &&
		enable_view<__uncvref<T>>;

	///////////////////////////////////////////////////////////////////////////
	// common_range
	//
	template<class T>
	META_CONCEPT common_range =
		range<T> && same_as<iterator_t<T>, sentinel_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// output_range [ranges.output]
	//
	template<class R, class T>
	META_CONCEPT output_range =
		range<R> && output_iterator<iterator_t<R>, T>;

	///////////////////////////////////////////////////////////////////////////
	// input_range [ranges.input]
	//
	template<class T>
	META_CONCEPT input_range =
		range<T> && input_iterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// forward_range [ranges.forward]
	//
	template<class T>
	META_CONCEPT forward_range =
		input_range<T> && forward_iterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// bidirectional_range [ranges.bidirectional]
	//
	template<class T>
	META_CONCEPT bidirectional_range =
		forward_range<T> && bidirectional_iterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// random_access_range [ranges.random.access]
	//
	template<class T>
	META_CONCEPT random_access_range =
		bidirectional_range<T> && random_access_iterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// contiguous_range [ranges.contiguous]
	//
	template<class R>
	META_CONCEPT contiguous_range =
		random_access_range<R> && contiguous_iterator<iterator_t<R>> &&
		requires(R& r) {
			{ data(r) } -> same_as<std::add_pointer_t<iter_reference_t<iterator_t<R>>>>;
		};

	namespace ext {
		template<class R>
		META_CONCEPT simple_view =
			view<R> && range<const R> &&
			same_as<iterator_t<R>, iterator_t<const R>> &&
			same_as<sentinel_t<R>, sentinel_t<const R>>;
	}

	template<class Rng>
	META_CONCEPT viewable_range =
#if 1 // This is the PR of https://github.com/ericniebler/stl2/issues/623
		view<__uncvref<Rng>> || _ForwardingRange<Rng>;
#else
		range<Rng> &&
		(_RangeImpl<Rng> || view<std::decay_t<Rng>>);
#endif

	namespace ext {
		template<range R>
		using range_value_t = iter_value_t<iterator_t<R>>;

		template<range R>
		using range_difference_t = iter_difference_t<iterator_t<R>>;

		template<range R>
		using range_reference_t = iter_reference_t<iterator_t<R>>;

		template<range R>
		using range_rvalue_reference_t = iter_rvalue_reference_t<iterator_t<R>>;
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
