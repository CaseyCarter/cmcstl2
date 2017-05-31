// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_INDIRECT_HPP
#define STL2_VIEW_INDIRECT_HPP

#include <meta/meta.hpp>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <View Rng>
		requires
			InputRange<Rng> &&
			Readable<reference_t<iterator_t<Rng>>>
		class indirect_view : detail::ebo_box<Rng, indirect_view<Rng>> {
			using base_t = detail::ebo_box<Rng, indirect_view<Rng>>;
			using base_t::get;

			template <bool IsConst>
			using sentinel_t = __stl2::sentinel_t<__maybe_const<IsConst, Rng>>;
			template <bool IsConst>
			using iterator_t = __stl2::iterator_t<__maybe_const<IsConst, Rng>>;

			template <bool IsConst>
			struct sentinel {
				sentinel_t<IsConst> s_;
			};
			template <bool IsConst>
			struct cursor {
				iterator_t<IsConst> it_;

				decltype(auto) read() const { return **it_; }
				void next() { ++it_; }
				void prev()
				requires BidirectionalIterator<iterator_t<IsConst>>
				{ --it_; }

				bool equal(const cursor& that) const
				requires Sentinel<iterator_t<IsConst>, iterator_t<IsConst>>
				{ return it_ == that.it_; }
				bool equal(const sentinel<IsConst>& s) const
				{ return it_ == s.s_; }

				difference_type_t<iterator_t<IsConst>> distance_to(const cursor& that) const
				requires SizedSentinel<iterator_t<IsConst>, iterator_t<IsConst>>
				{ return that.it_ - it_; }
				difference_type_t<iterator_t<IsConst>>
				distance_to(const sentinel<IsConst>& that) const
				requires SizedSentinel<sentinel_t<IsConst>, iterator_t<IsConst>>
				{ return that.s_ - it_; }

				void advance(difference_type_t<iterator_t<IsConst>> n)
				requires RandomAccessIterator<iterator_t<IsConst>>
				{ it_ += n; }
			};
		public:
			indirect_view() requires DefaultConstructible<Rng> = default;
			indirect_view(Rng rng) : base_t{std::move(rng)} {}

			basic_iterator<cursor<false>> begin()
			requires !Range<Rng const>
			{ return {cursor<false>{__stl2::begin(get())}}; }

			sentinel<false> end()
			requires !Range<Rng const>
			{ return {__stl2::end(get())}; }
			basic_iterator<cursor<false>> end()
			requires !Range<Rng const> && BoundedRange<Rng>
			{ return {cursor<false>{__stl2::end(get())}}; }

			auto size()
			requires !Range<Rng const> && SizedRange<Rng>
			{ return __stl2::size(get()); }

			basic_iterator<cursor<true>> begin() const
			requires Range<Rng const>
			{ return {cursor<true>{__stl2::begin(get())}}; }

			sentinel<true> end() const
			requires Range<Rng const>
			{ return {__stl2::end(get())}; }
			basic_iterator<cursor<true>> end() const
			requires BoundedRange<Rng const>
			{ return {cursor<true>{__stl2::end(get())}}; }

			auto size() const
			requires SizedRange<Rng const>
			{ return __stl2::size(get()); }
		};
	} // namespace ext

	template <class V>
	struct enable_view<ext::indirect_view<V>> : std::true_type {};
} STL2_CLOSE_NAMESPACE

#endif
