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
	template<class, size_t> struct array;
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
	META_CONCEPT _RangeImpl =
		requires(T&& t) {
			begin(static_cast<T&&>(t)); // not necessarily equality-preserving
			end(static_cast<T&&>(t));
		};

	template<class T>
	META_CONCEPT Range = _RangeImpl<T&>;

	template<class T>
	META_CONCEPT _ForwardingRange = Range<T> && _RangeImpl<T>;

	template<class R>
	META_CONCEPT SizedRange =
		Range<R> && !disable_sized_range<__uncvref<R>> &&
		requires(R& r) { size(r); };

	///////////////////////////////////////////////////////////////////////////
	// View [ranges.view]
	//
	struct view_base {};

	template<class T>
	META_CONCEPT _ContainerLike =
		Range<T> && Range<const T> &&
		!Same<iter_reference_t<iterator_t<T>>, iter_reference_t<iterator_t<const T>>>;

	template<class T>
	META_CONCEPT __enable_view_default = DerivedFrom<T, view_base> || !_ContainerLike<T>;

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
	META_CONCEPT View =
		Range<T> &&
		Semiregular<T> &&
		enable_view<__uncvref<T>>;

	///////////////////////////////////////////////////////////////////////////
	// CommonRange
	//
	template<class T>
	META_CONCEPT CommonRange =
		Range<T> && Same<iterator_t<T>, sentinel_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// OutputRange [ranges.output]
	//
	template<class R, class T>
	META_CONCEPT OutputRange =
		Range<R> && OutputIterator<iterator_t<R>, T>;

	///////////////////////////////////////////////////////////////////////////
	// InputRange [ranges.input]
	//
	template<class T>
	META_CONCEPT InputRange =
		Range<T> && InputIterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// ForwardRange [ranges.forward]
	//
	template<class T>
	META_CONCEPT ForwardRange =
		InputRange<T> && ForwardIterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// BidirectionalRange [ranges.bidirectional]
	//
	template<class T>
	META_CONCEPT BidirectionalRange =
		ForwardRange<T> && BidirectionalIterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// RandomAccessRange [ranges.random.access]
	//
	template<class T>
	META_CONCEPT RandomAccessRange =
		BidirectionalRange<T> && RandomAccessIterator<iterator_t<T>>;

	///////////////////////////////////////////////////////////////////////////
	// ContiguousRange [ranges.contiguous]
	//
	template<class R>
	META_CONCEPT ContiguousRange =
		RandomAccessRange<R> && ContiguousIterator<iterator_t<R>> &&
		requires(R& r) {
			{ data(r) } -> Same<std::add_pointer_t<iter_reference_t<iterator_t<R>>>>;
		};

	namespace ext {
		template<class R>
		META_CONCEPT SimpleView =
			View<R> && Range<const R> &&
			Same<iterator_t<R>, iterator_t<const R>> &&
			Same<sentinel_t<R>, sentinel_t<const R>>;
	}

	template<class Rng>
	META_CONCEPT ViewableRange =
		Range<Rng> &&
		(_RangeImpl<Rng> || View<Rng>);

	namespace ext {
		template<Range R>
		using range_value_t = iter_value_t<iterator_t<R>>;

		template<Range R>
		using range_difference_t = iter_difference_t<iterator_t<R>>;

		template<Range R>
		using range_reference_t = iter_reference_t<iterator_t<R>>;

		template<Range R>
		using range_rvalue_reference_t = iter_rvalue_reference_t<iterator_t<R>>;

		template<class, unsigned = 0>
		inline constexpr int static_extent = -1;

		// Extension (vs. P1419): implement static_extent for tiny-ranges
		template<SizedRange R>
		requires requires {
			// BUGSBUGSBUGS: should be { R::size() } Integral;
			R::size(); requires Integral<decltype(R::size())>;
			// BUGSBUGSBUGS: should be typename __require_constant<R::size()>;
			typename ::__stl2::__require_constant<R::size()>;
		}
		inline constexpr auto static_extent<R> = R::size();

		template<class T, std::size_t N, unsigned I>
		inline constexpr auto static_extent<T[N], I> = std::extent_v<T[N], I>;

		template<class T, std::size_t N>
		inline constexpr std::ptrdiff_t static_extent<std::array<T, N>, 0> =
			std::ptrdiff_t{N};

		// Extension: support nested std::array
		template<class T, std::size_t N, unsigned I>
		requires I > 0
		inline constexpr auto static_extent<std::array<T, N>, I> =
			static_extent<T, I - 1>;

		template<class R, unsigned I = 0>
		META_CONCEPT __has_static_extent = SizedRange<R> && requires {
			{ static_extent<__uncvref<R>, I> } -> Integral;
			typename __require_constant<static_extent<__uncvref<R>, I>>;
			requires static_extent<__uncvref<R>, I> >= 0;
		};

		template<class R, unsigned I = 0>
		requires __has_static_extent<R, I>
		inline constexpr auto static_extent_of = static_extent<__uncvref<R>, I>;
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
