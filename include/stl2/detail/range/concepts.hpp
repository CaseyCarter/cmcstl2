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
#ifndef __GLIBCXX__
#pragma message "These forward declarations will likely only work with libstdc++."
#endif
	template<class, class, class> class set;
	template<class, class, class> class multiset;
	template<class, class, class, class> class unordered_set;
	template<class, class, class, class> class unordered_multiset;
}

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Range [ranges.range]
	//
	template<class T>
	using iterator_t = __begin_t<T&>;

	template<class T>
	using sentinel_t = __end_t<T&>;

	template<class T>
	STL2_CONCEPT _RangeImpl =
		requires(T&& t) {
			begin(static_cast<T&&>(t)); // not necessarily equality-preserving
			end(static_cast<T&&>(t));
		};

	template<class T>
	STL2_CONCEPT Range = _RangeImpl<T&>;

	template<class T>
	STL2_CONCEPT _ForwardingRange = Range<T> && _RangeImpl<T>;

	template<class R>
	STL2_CONCEPT SizedRange =
		Range<R> && !disable_sized_range<__uncvref<R>> &&
		requires(R& r) { size(r); };

	///////////////////////////////////////////////////////////////////////////
	// View [ranges.view]
	//
	struct view_base {};

	template<class T>
	STL2_CONCEPT _ContainerLike =
		Range<T> && Range<const T> &&
		!Same<iter_reference_t<iterator_t<T>>, iter_reference_t<iterator_t<const T>>>;

	template<class T>
	STL2_CONCEPT __enable_view_default = DerivedFrom<T, view_base> || !_ContainerLike<T>;

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
	STL2_CONCEPT View =
		Range<T> &&
		Semiregular<T> &&
		enable_view<__uncvref<T>>;

	///////////////////////////////////////////////////////////////////////////
	// CommonRange
	//
	template<class T>
	STL2_CONCEPT CommonRange =
		Range<T> && Same<iterator_t<T>, sentinel_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// OutputRange [ranges.output]
	//
	template<class R, class T>
	STL2_CONCEPT OutputRange =
		Range<R> && OutputIterator<iterator_t<R>, T>;

	///////////////////////////////////////////////////////////////////////////
	// InputRange [ranges.input]
	//
	template<class T>
	STL2_CONCEPT InputRange =
		Range<T> && InputIterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// ForwardRange [ranges.forward]
	//
	template<class T>
	STL2_CONCEPT ForwardRange =
		Range<T> && ForwardIterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// BidirectionalRange [ranges.bidirectional]
	//
	template<class T>
	STL2_CONCEPT BidirectionalRange =
		Range<T> && BidirectionalIterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// RandomAccessRange [ranges.random.access]
	//
	template<class T>
	STL2_CONCEPT RandomAccessRange =
		Range<T> && RandomAccessIterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// ContiguousRange [ranges.contiguous]
	//
	template<class R>
	STL2_CONCEPT ContiguousRange =
		std::is_reference_v<iter_reference_t<iterator_t<R>>> &&
		Same<iter_value_t<iterator_t<R>>, __uncvref<iter_reference_t<iterator_t<R>>>> &&
		requires(R& r) {
			{ data(r) } -> Same<std::add_pointer_t<iter_reference_t<iterator_t<R>>>>;
		};

	namespace ext {
		template<class R>
		STL2_CONCEPT SimpleView =
			View<R> && Range<const R> &&
			Same<iterator_t<R>, iterator_t<const R>> &&
			Same<sentinel_t<R>, sentinel_t<const R>>;
	}

	template<class Rng>
	STL2_CONCEPT ViewableRange =
		Range<Rng> &&
		(_RangeImpl<Rng> || View<Rng>);
} STL2_CLOSE_NAMESPACE

#endif
